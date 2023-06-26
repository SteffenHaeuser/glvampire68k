#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>

#include <proto/exec.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

extern struct Library *MaggieBase;

ULONG MultiplyColorByAlpha(ULONG color)
{
    float alpha = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
    ULONG red = static_cast<ULONG>((color >> 16) & 0xFF);
    ULONG green = static_cast<ULONG>((color >> 8) & 0xFF);
    ULONG blue = static_cast<ULONG>(color & 0xFF);

    red = static_cast<ULONG>(red * alpha);
    green = static_cast<ULONG>(green * alpha);
    blue = static_cast<ULONG>(blue * alpha);

    return (red << 16) | (green << 8) | blue | ((color & 0xFF) << 24);
}

ULONG getCurrentColor(const MaggieVertex& vertex, struct GLVampContext* vampContext) 
{
    // Apply modelview and projection transformations to vertex coordinates
    vec4 transformedVertex;
    transformedVertex.x = vampContext->projectionMatrix.m[0][0] * (vampContext->modelViewMatrix.m[0][0] * vertex.pos.x + vampContext->modelViewMatrix.m[1][0] * vertex.pos.y + vampContext->modelViewMatrix.m[2][0] * vertex.pos.z + vampContext->modelViewMatrix.m[3][0]);
    transformedVertex.y = vampContext->projectionMatrix.m[1][1] * (vampContext->modelViewMatrix.m[0][1] * vertex.pos.x + vampContext->modelViewMatrix.m[1][1] * vertex.pos.y + vampContext->modelViewMatrix.m[2][1] * vertex.pos.z + vampContext->modelViewMatrix.m[3][1]);
    transformedVertex.z = vampContext->projectionMatrix.m[2][2] * (vampContext->modelViewMatrix.m[0][2] * vertex.pos.x + vampContext->modelViewMatrix.m[1][2] * vertex.pos.y + vampContext->modelViewMatrix.m[2][2] * vertex.pos.z + vampContext->modelViewMatrix.m[3][2]);
    transformedVertex.w = vampContext->projectionMatrix.m[3][2] * (vampContext->modelViewMatrix.m[0][3] * vertex.pos.x + vampContext->modelViewMatrix.m[1][3] * vertex.pos.y + vampContext->modelViewMatrix.m[2][3] * vertex.pos.z + vampContext->modelViewMatrix.m[3][3]);

    // Convert normalized device coordinates to screen coordinates
    int screenX = static_cast<int>((transformedVertex.x / transformedVertex.w + 1.0f) * 0.5f * vampContext->vampWidth);
    int screenY = static_cast<int>((1.0f - transformedVertex.y / transformedVertex.w) * 0.5f * vampContext->vampHeight);

    // Clamp screen coordinates to valid range
    screenX = (screenX < 0) ? 0 : ((screenX >= vampContext->vampWidth) ? vampContext->vampWidth - 1 : screenX);
    screenY = (screenY < 0) ? 0 : ((screenY >= vampContext->vampHeight) ? vampContext->vampHeight - 1 : screenY);

    // Calculate the index of the pixel in the frame buffer
    int pixelIndex = screenY * vampContext->vampWidth + screenX;

    // Retrieve the color from the frame buffer
    ULONG color = 0;
    if (vampContext->vampBpp == 2) {
        // Format: RGB16
        USHORT* frameBuffer = reinterpret_cast<USHORT*>(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer]);
        color = frameBuffer[pixelIndex];
    } else if (vampContext->vampBpp == 4) {
        // Format: ARGB32
        ULONG* frameBuffer = reinterpret_cast<ULONG*>(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer]);
        color = frameBuffer[pixelIndex];
    }

    return color;
}

void GLBlendColor(struct GLVampContext* vampContext, float r, float g, float b, float a)
{
    // Normalize the color components from the range 0.0-1.0 to 0-255
    unsigned char red = static_cast<unsigned char>(r * 255.0f);
    unsigned char green = static_cast<unsigned char>(g * 255.0f);
    unsigned char blue = static_cast<unsigned char>(b * 255.0f);
    unsigned char alpha = static_cast<unsigned char>(a * 255.0f);
    
    // Pack the color components into a single ULONG value using ARGB32 format
    vampContext->blendColor = (alpha << 24) | (red << 16) | (green << 8) | blue;
}

void GLBlendFunc(struct GLVampContext* vampContext, int i, int j)
{
	vampContext->blendFuncSrc = i;
	vampContext->blendFuncDest = j;
}

ULONG getConstantAlpha(struct GLVampContext *vampContext)
{
    // Extract the alpha component from the blendColor value
    ULONG alpha = (vampContext->blendColor >> 24) & 0xFF;
    return alpha;
}

ULONG getConstantColor(struct GLVampContext *vampContext)
{
    // Return the constant color value as fully opaque white
    return vampContext->blendColor;
}

void ApplyBlendFunc(struct GLVampContext* vampContext, std::vector<MaggieVertex>& vertices)
{
    // Apply the blend function
    switch (vampContext->blendFuncSrc) {
        case GL_ZERO:
            // Blending logic for GL_ZERO source
            // No blending, set color to transparent black (0x00000000)
            for (auto& vertex : vertices) {
                vertex.colour = 0x00000000;
            }
            break;
        case GL_ONE:
            // Blending logic for GL_ONE source
            // No blending, preserve original color
            break;
        case GL_SRC_COLOR:
            // Blending logic for GL_SRC_COLOR source
            // Multiply vertex color by source color
            for (auto& vertex : vertices) {
                ULONG srcColor = vertex.colour & 0x00FFFFFF; // Get the source color
                vertex.colour = MultiplyColorByAlpha(srcColor); // Multiply by vertex alpha
            }
            break;
        case GL_ONE_MINUS_SRC_COLOR:
            // Blending logic for GL_ONE_MINUS_SRC_COLOR source
            // Multiply vertex color by (1 - source color)
            for (auto& vertex : vertices) {
                ULONG srcColor = vertex.colour & 0x00FFFFFF; // Get the source color
                ULONG invSrcColor = 0xFFFFFFFF - srcColor; // Compute (1 - source color)
                vertex.colour = MultiplyColorByAlpha(invSrcColor); // Multiply by vertex alpha
            }
            break;
        case GL_DST_COLOR:
            // Blending logic for GL_DST_COLOR source
            // Multiply vertex color by destination color
            for (MaggieVertex vertex : vertices) {
                ULONG destColor = getCurrentColor(vertex,vampContext); // Get the destination color
                vertex.colour = MultiplyColorByAlpha(destColor); // Multiply by vertex alpha
            }
            break;
        case GL_ONE_MINUS_DST_COLOR:
            // Blending logic for GL_ONE_MINUS_DST_COLOR source
            // Multiply vertex color by (1 - destination color)
            for (auto& vertex : vertices) {
                ULONG destColor = getCurrentColor(vertex,vampContext); // Get the destination color
                ULONG invDestColor = 0xFFFFFFFF - destColor; // Compute (1 - destination color)
                vertex.colour = MultiplyColorByAlpha(invDestColor); // Multiply by vertex alpha
            }
            break;
        case GL_SRC_ALPHA:
            // Blending logic for GL_SRC_ALPHA source
            // Multiply vertex color by vertex alpha
            for (auto& vertex : vertices) {
                vertex.colour = MultiplyColorByAlpha(vertex.colour);
            }
            break;
        case GL_ONE_MINUS_SRC_ALPHA:
            // Blending logic for GL_ONE_MINUS_SRC_ALPHA source
            // Multiply vertex color by (1 - vertex alpha)
            for (auto& vertex : vertices) {
                ULONG alpha = vertex.colour & 0xFF000000; // Get the vertex alpha
                ULONG invAlpha = 0x00FFFFFF - alpha; // Compute (1 - vertex alpha)
                vertex.colour = MultiplyColorByAlpha(invAlpha);
            }
            break;
        case GL_DST_ALPHA:
            // Blending logic for GL_DST_ALPHA source
            // Multiply vertex color by destination alpha
            for (auto& vertex : vertices) {
                ULONG destAlpha = getCurrentColor(vertex,vampContext) & 0xFF000000; // Get the destination alpha
                vertex.colour = MultiplyColorByAlpha(destAlpha);
            }
            break;
        case GL_ONE_MINUS_DST_ALPHA:
            // Blending logic for GL_ONE_MINUS_DST_ALPHA source
            // Multiply vertex color by (1 - destination alpha)
            for (auto& vertex : vertices) {
                ULONG destAlpha = getCurrentColor(vertex,vampContext) & 0xFF000000; // Get the destination alpha
                ULONG invDestAlpha = 0x00FFFFFF - destAlpha; // Compute (1 - destination alpha)
                vertex.colour = MultiplyColorByAlpha(invDestAlpha);
            }
            break;
        case GL_CONSTANT_COLOR:
            // Blending logic for GL_CONSTANT_COLOR source
            // Multiply vertex color by constant color
            for (auto& vertex : vertices) {
                ULONG constantColor = getConstantColor(vampContext); // Get the constant color
                vertex.colour = MultiplyColorByAlpha(constantColor);
            }
            break;
        case GL_ONE_MINUS_CONSTANT_COLOR:
            // Blending logic for GL_ONE_MINUS_CONSTANT_COLOR source
            // Multiply vertex color by (1 - constant color)
            for (auto& vertex : vertices) {
                ULONG constantColor = getConstantColor(vampContext); // Get the constant color
                ULONG invConstantColor = 0x00FFFFFF - constantColor; // Compute (1 - constant color)
                vertex.colour = MultiplyColorByAlpha(invConstantColor);
            }
            break;
        case GL_CONSTANT_ALPHA:
            // Blending logic for GL_CONSTANT_ALPHA source
            // Multiply vertex color by constant alpha
            for (auto& vertex : vertices) {
                ULONG constantAlpha = getConstantAlpha(vampContext); // Get the constant alpha
                vertex.colour = MultiplyColorByAlpha(constantAlpha);
            }
            break;
        case GL_ONE_MINUS_CONSTANT_ALPHA:
            // Blending logic for GL_ONE_MINUS_CONSTANT_ALPHA source
            // Multiply vertex color by (1 - constant alpha)
            for (auto& vertex : vertices) {
                ULONG constantAlpha = getConstantAlpha(vampContext); // Get the constant alpha
                ULONG invConstantAlpha = 0x00FFFFFF - constantAlpha; // Compute (1 - constant alpha)
                vertex.colour = MultiplyColorByAlpha(invConstantAlpha);
            }
            break;
        default:
            // Invalid blendFuncSrc value
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid blendFuncSrc value");
            return;
    }

    switch (vampContext->blendFuncDest) {
        case GL_ZERO:
            // Blending logic for GL_ZERO destination
            // No blending, set color to transparent black (0x00000000)
            for (auto& vertex : vertices) {
                vertex.colour = 0x00000000;
            }
            break;
        case GL_ONE:
            // Blending logic for GL_ONE destination
            // Blend using source color
            for (auto& vertex : vertices) {
                ULONG srcColor = vertex.colour & 0x00FFFFFF; // Get the source color
                vertex.colour = MultiplyColorByAlpha(srcColor); // Multiply by vertex alpha
            }
            break;
        case GL_SRC_COLOR:
            // Blending logic for GL_SRC_COLOR destination
            // Blend using source color
            for (auto& vertex : vertices) {
                ULONG srcColor = vertex.colour & 0x00FFFFFF; // Get the source color
                vertex.colour = MultiplyColorByAlpha(srcColor); // Multiply by vertex alpha
            }
            break;
        case GL_ONE_MINUS_SRC_COLOR:
            // Blending logic for GL_ONE_MINUS_SRC_COLOR destination
            // Blend using (1 - source color)
            for (auto& vertex : vertices) {
                ULONG srcColor = vertex.colour & 0x00FFFFFF; // Get the source color
                ULONG invSrcColor = 0xFFFFFFFF - srcColor; // Compute (1 - source color)
                vertex.colour = MultiplyColorByAlpha(invSrcColor); // Multiply by vertex alpha
            }
            break;
        case GL_DST_COLOR:
            // Blending logic for GL_DST_COLOR destination
            // Blend using destination color
            for (auto& vertex : vertices) {
                ULONG destColor = getCurrentColor(vertex,vampContext); // Get the destination color
                vertex.colour = MultiplyColorByAlpha(destColor); // Multiply by vertex alpha
            }
            break;
        case GL_ONE_MINUS_DST_COLOR:
            // Blending logic for GL_ONE_MINUS_DST_COLOR destination
            // Blend using (1 - destination color)
            for (MaggieVertex vertex : vertices) {
                ULONG destColor = getCurrentColor(vertex,vampContext); // Get the destination color
                ULONG invDestColor = 0xFFFFFFFF - destColor; // Compute (1 - destination color)
                vertex.colour = MultiplyColorByAlpha(invDestColor); // Multiply by vertex alpha
            }
            break;
        case GL_SRC_ALPHA:
            // Blending logic for GL_SRC_ALPHA destination
            // Multiply vertex color by vertex alpha
            for (auto& vertex : vertices) {
                vertex.colour = MultiplyColorByAlpha(vertex.colour);
            }
            break;
        case GL_ONE_MINUS_SRC_ALPHA:
            // Blending logic for GL_ONE_MINUS_SRC_ALPHA destination
            // Multiply vertex color by (1 - vertex alpha)
            for (auto& vertex : vertices) {
                ULONG alpha = vertex.colour & 0xFF000000; // Get the vertex alpha
                ULONG invAlpha = 0x00FFFFFF - alpha; // Compute (1 - vertex alpha)
                vertex.colour = MultiplyColorByAlpha(invAlpha);
            }
            break;
        case GL_DST_ALPHA:
            // Blending logic for GL_DST_ALPHA destination
            // Multiply vertex color by destination alpha
            for (auto& vertex : vertices) {
                ULONG destAlpha = getCurrentColor(vertex,vampContext) & 0xFF000000; // Get the destination alpha
                vertex.colour = MultiplyColorByAlpha(destAlpha);
            }
            break;
        case GL_ONE_MINUS_DST_ALPHA:
            // Blending logic for GL_ONE_MINUS_DST_ALPHA destination
            // Multiply vertex color by (1 - destination alpha)
            for (auto& vertex : vertices) {
                ULONG destAlpha = getCurrentColor(vertex,vampContext) & 0xFF000000; // Get the destination alpha
                ULONG invDestAlpha = 0x00FFFFFF - destAlpha; // Compute (1 - destination alpha)
                vertex.colour = MultiplyColorByAlpha(invDestAlpha);
            }
            break;
        case GL_CONSTANT_COLOR:
            // Blending logic for GL_CONSTANT_COLOR destination
            // Multiply vertex color by constant color
            for (auto& vertex : vertices) {
                ULONG constantColor = getConstantColor(vampContext); // Get the constant color
                vertex.colour = MultiplyColorByAlpha(constantColor);
            }
            break;
        case GL_ONE_MINUS_CONSTANT_COLOR:
            // Blending logic for GL_ONE_MINUS_CONSTANT_COLOR destination
            // Multiply vertex color by (1 - constant color)
            for (auto& vertex : vertices) {
                ULONG constantColor = getConstantColor(vampContext); // Get the constant color
                ULONG invConstantColor = 0x00FFFFFF - constantColor; // Compute (1 - constant color)
                vertex.colour = MultiplyColorByAlpha(invConstantColor);
            }
            break;
        case GL_CONSTANT_ALPHA:
            // Blending logic for GL_CONSTANT_ALPHA destination
            // Multiply vertex color by constant alpha
            for (auto& vertex : vertices) {
                ULONG constantAlpha = getConstantAlpha(vampContext); // Get the constant alpha
                vertex.colour = MultiplyColorByAlpha(constantAlpha);
            }
            break;
        case GL_ONE_MINUS_CONSTANT_ALPHA:
            // Blending logic for GL_ONE_MINUS_CONSTANT_ALPHA destination
            // Multiply vertex color by (1 - constant alpha)
            for (auto& vertex : vertices) {
                ULONG constantAlpha = getConstantAlpha(vampContext); // Get the constant alpha
                ULONG invConstantAlpha = 0x00FFFFFF - constantAlpha; // Compute (1 - constant alpha)
                vertex.colour = MultiplyColorByAlpha(invConstantAlpha);
            }
            break;
        default:
            // Invalid blendFuncDest value
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid blendFuncDest value");
            return;
    }
}
