#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>
#include <stdlib.h>

#include <proto/exec.h>
#include <exec/exec.h>
#include <proto/Maggie.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

extern struct Library *MaggieBase;

extern "C" void GLBegin(struct GLVampContext *vampContext, int mode) 
{
	char error[1024];
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
    switch (mode) {
        case GL_QUADS:
        case GL_POLYGON:
        case GL_TRIANGLE_FAN:
        case GL_LINE_STRIP:
        case GL_LINES:
        case GL_TRIANGLE_STRIP:
            vampContext->currentMode = static_cast<int>(mode);
            vertices->clear();
            break;
        default:
			vampContext->glError = GL_INVALID_ENUM;
			sprintf(error,"Unknown mode for glBegin: %d\n",mode);
			GenerateGLError(GL_INVALID_ENUM, error);
            break;
    }
	if ((mode==GL_QUADS)||(mode==GL_POLYGON)||(mode==GL_LINE_STRIP)||(mode==GL_LINES)) vampContext->manualDraw = 1;
	else vampContext->manualDraw = 0;
	if (!vampContext->manualDraw) magBegin();
}

ULONG PackColor(ULONG r, ULONG g, ULONG b, ULONG a)
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

extern "C" void GLFogfv(GLVampContext* vampContext, int i, float* col)
{
    switch (i) {
        case GL_FOG_COLOR:
            vampContext->fogParams.color = static_cast<unsigned int>(*col);
            break;
        case GL_FOG_DENSITY:
            vampContext->fogParams.density = *col;
            break;
        case GL_FOG_START:
            vampContext->fogParams.start = *col;
            break;
        case GL_FOG_END:
            vampContext->fogParams.end = *col;
            break;
        case GL_FOG_MODE:
            switch (static_cast<int>(*col)) {
                case FOG_LINEAR:
                    vampContext->fogParams.mode = FOG_LINEAR;
                    break;
                case FOG_EXP:
                    vampContext->fogParams.mode = FOG_EXP;
                    break;
                case FOG_EXP2:
                    vampContext->fogParams.mode = FOG_EXP2;
                    break;
                default:
                    vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(GL_INVALID_ENUM, "Invalid fog mode");				
                    break;
            }
            break;
        case GL_FOG_COORD_SRC:
            switch (static_cast<int>(*col)) {
                case GL_FOG_COORD:
                    vampContext->fogParams.fogCoordSrc = GL_FOG_COORD;
                    break;
                case GL_FRAGMENT_DEPTH:
                    vampContext->fogParams.fogCoordSrc = GL_FRAGMENT_DEPTH;
                    break;
                default:
                    vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(GL_INVALID_ENUM, "Invalid fog Coord Src Mode");				
                    break;
            }
            break;
        default:
            break;
    }
}

extern "C" void GLFogi(GLVampContext* vampContext, int pname, int param)
{
    switch (pname) {
        case GL_FOG_MODE: {
            FogMode mode;

            switch (param) {
                case GL_LINEAR:
                    mode = FOG_LINEAR;
                    break;
                case GL_EXP:
                    mode = FOG_EXP;
                    break;
                case GL_EXP2:
                    mode = FOG_EXP2;
                    break;
                default:
                    vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(GL_INVALID_ENUM, "Invalid fog mode");
                    return;
            }

            vampContext->fogParams.mode = mode;
            break;
        }

        case GL_FOG_DENSITY:
            vampContext->fogParams.density = static_cast<float>(param);
            break;

        case GL_FOG_START:
            vampContext->fogParams.start = static_cast<float>(param);
            break;

        case GL_FOG_END:
            vampContext->fogParams.end = static_cast<float>(param);
            break;

        case GL_FOG_COLOR:
            vampContext->fogParams.color = PackColor(
                static_cast<ULONG>((param >> 24) & 0xFF),
                static_cast<ULONG>((param >> 16) & 0xFF),
                static_cast<ULONG>((param >> 8) & 0xFF),
                static_cast<ULONG>(param & 0xFF)
            );
            break;

        case GL_FOG_COORD_SRC: {
            switch (param) {
                case GL_FOG_COORD:
                    vampContext->fogParams.fogCoordSrc = GL_FOG_COORD;
                    break;
                case GL_FRAGMENT_DEPTH:
                    vampContext->fogParams.fogCoordSrc = GL_FRAGMENT_DEPTH;
                    break;
                default:
                    vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(GL_INVALID_ENUM, "Invalid fog coordinate source");
                    return;
            }
            break;
        }

        default:
            GenerateGLError(GL_INVALID_ENUM, "Invalid fog parameter");
            return;
    }
}

extern "C" void GLFogf(GLVampContext* vampContext, int pname, float param)
{
    switch (pname) {
        case GL_FOG_MODE: {
            FogMode mode;

            if (param == GL_LINEAR)
                mode = FOG_LINEAR;
            else if (param == GL_EXP)
                mode = FOG_EXP;
            else if (param == GL_EXP2)
                mode = FOG_EXP2;
            else {
                vampContext->glError = GL_INVALID_ENUM;
                GenerateGLError(GL_INVALID_ENUM, "Invalid fog mode");
                return;
            }

            vampContext->fogParams.mode = mode;
            break;
        }

        case GL_FOG_DENSITY:
            vampContext->fogParams.density = param;
            break;

        case GL_FOG_START:
            vampContext->fogParams.start = param;
            break;

        case GL_FOG_END:
            vampContext->fogParams.end = param;
            break;

        case GL_FOG_COLOR:
            vampContext->fogParams.color = PackColor(
                static_cast<ULONG>((param * 255.0f) + 0.5f),
                static_cast<ULONG>((param * 255.0f) + 0.5f),
                static_cast<ULONG>((param * 255.0f) + 0.5f),
                255
            );
            break;

        case GL_FOG_COORD_SRC: {
            if (param == GL_FOG_COORD)
                vampContext->fogParams.fogCoordSrc = GL_FOG_COORD;
            else if (param == GL_FRAGMENT_DEPTH)
                vampContext->fogParams.fogCoordSrc = GL_FRAGMENT_DEPTH;
            else {
                vampContext->glError = GL_INVALID_ENUM;
                GenerateGLError(GL_INVALID_ENUM, "Invalid fog coordinate source");
                return;
            }
            break;
        }

        default:
			vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid fog parameter");
            return;
    }
}

ULONG InterpolateColor(ULONG startColor, ULONG endColor, float factor)
{
    ULONG r = static_cast<ULONG>(static_cast<float>((endColor >> 24) & 0xFF) * factor + static_cast<float>((startColor >> 24) & 0xFF) * (1.0f - factor));
    ULONG g = static_cast<ULONG>(static_cast<float>((endColor >> 16) & 0xFF) * factor + static_cast<float>((startColor >> 16) & 0xFF) * (1.0f - factor));
    ULONG b = static_cast<ULONG>(static_cast<float>((endColor >> 8) & 0xFF) * factor + static_cast<float>((startColor >> 8) & 0xFF) * (1.0f - factor));
    ULONG a = static_cast<ULONG>(static_cast<float>(endColor & 0xFF) * factor + static_cast<float>(startColor & 0xFF) * (1.0f - factor));

    return (r << 24) | (g << 16) | (b << 8) | a;
}

void ApplyFogging(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices)
{
    if (vampContext->useFogging) {
        // Iterate over the vertices and modify the color values based on the fog factor
        for (size_t i = 0; i < vertices->size(); i++) {
            MaggieVertex& vertex = (*vertices)[i];

            // Calculate the fog factor based on the Z-coordinate or fog coordinate
            float fogFactor = 0.0f;
            if (vampContext->useFogCoordSrc) {
                float fogCoord = vertex.pos.z;
                if (vampContext->fogParams.mode == FOG_LINEAR) {
                    fogFactor = (fogCoord - vampContext->fogParams.start) / (vampContext->fogParams.end - vampContext->fogParams.start);
                } else if (vampContext->fogParams.mode == FOG_EXP) {
                    float density = vampContext->fogParams.density;
                    fogFactor = 1.0f - exp(-density * fogCoord);
                } else if (vampContext->fogParams.mode == FOG_EXP2) {
                    float density = vampContext->fogParams.density;
                    fogFactor = 1.0f - exp(-density * density * fogCoord * fogCoord);
                }
            } else {
                float z = vertex.pos.z;
                if (vampContext->fogParams.mode == FOG_LINEAR) {
                    fogFactor = (z - vampContext->fogParams.start) / (vampContext->fogParams.end - vampContext->fogParams.start);
                } else if (vampContext->fogParams.mode == FOG_EXP) {
                    float density = vampContext->fogParams.density;
                    fogFactor = 1.0f - exp(-density * z);
                } else if (vampContext->fogParams.mode == FOG_EXP2) {
                    float density = vampContext->fogParams.density;
                    fogFactor = 1.0f - exp(-density * density * z * z);
                }
            }

            fogFactor = (fogFactor < 0.0f) ? 0.0f : ((fogFactor > 1.0f) ? 1.0f : fogFactor);

            // Apply the fog factor to the color of the vertex
            ULONG fogColor = InterpolateColor(vampContext->fogParams.color, vertex.colour, fogFactor);
            vertex.colour = fogColor;
        }
    }
}

void ApplyTexEnv(struct GLVampContext *vampContext, std::vector<MaggieVertex>& vertices, int texenv)
{
    if (texenv == 0) {
        // GL_REPLACE
        // Iterate over each vertex and set its texture coordinates to (0, 0)
        for (auto& vertex : vertices) {
            vertex.tex[0].u = 0.0f;
            vertex.tex[0].v = 0.0f;
        }
    } else if (texenv == 1) {
        // GL_MODULATE
        // Iterate over each vertex and multiply its texture coordinates with the vertex color
        for (auto& vertex : vertices) {
            vertex.tex[0].u *= ((vertex.colour >> 16) & 0xFF) / 255.0f;
            vertex.tex[0].v *= ((vertex.colour >> 8) & 0xFF) / 255.0f;
        }
    } else if (texenv == 2) {
        // GL_DECAL
        // Iterate over each vertex and blend its texture coordinates with the vertex color using alpha blending
        for (auto& vertex : vertices) {
            float alpha = ((vertex.colour >> 24) & 0xFF) / 255.0f;
            vertex.tex[0].u = (1.0f - alpha) * vertex.tex[0].u + alpha * ((vertex.colour >> 16) & 0xFF) / 255.0f;
            vertex.tex[0].v = (1.0f - alpha) * vertex.tex[0].v + alpha * ((vertex.colour >> 8) & 0xFF) / 255.0f;
        }
    } else if (texenv == 3) {
        // GL_BLEND
        // Iterate over each vertex and blend its texture coordinates with the vertex color using blending functions
        for (auto& vertex : vertices) {
            float alpha = ((vertex.colour >> 24) & 0xFF) / 255.0f;
            vertex.tex[0].u = vertex.tex[0].u * (1.0f - alpha) + ((vertex.colour >> 16) & 0xFF) / 255.0f * alpha;
            vertex.tex[0].v = vertex.tex[0].v * (1.0f - alpha) + ((vertex.colour >> 8) & 0xFF) / 255.0f * alpha;
        }
    } else if (texenv == 4) {
        // GL_ADD
        // Iterate over each vertex and add its texture coordinates to the vertex color
        for (auto& vertex : vertices) {
            vertex.tex[0].u += ((vertex.colour >> 16) & 0xFF) / 255.0f;
            vertex.tex[0].v += ((vertex.colour >> 8) & 0xFF) / 255.0f;
        }
    } else {
                vampContext->glError = GL_INVALID_ENUM;
                GenerateGLError(GL_INVALID_ENUM, "Invalid TexEnv mode");
    }
}


void DrawQuads(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices, int texenv)
{
    if (vertices->size() % 4 != 0) {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(GL_INVALID_VALUE, "Invalid number of vertices for GL_QUADS\n");
        return;
    }

    std::vector<unsigned short> indices(vertices->size());
    for (size_t i = 0; i < indices.size(); i++) {
        indices[i] = static_cast<unsigned short>(i);
    }

    ApplyFogging(vampContext, vertices);
    ApplyTexEnv(vampContext,*vertices, texenv);

    // Draw the indexed polygons using the Maggie-3D chip
    magDrawIndexedPolygonsUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
}

void DrawPolygons(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices, int texenv)
{
    if (vertices->size() < 3) {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(GL_INVALID_VALUE, "Not enough vertices for polygon\n");
        return;
    }

    std::vector<unsigned short> indices(vertices->size());
    for (size_t i = 0; i < indices.size(); i++) {
        indices[i] = static_cast<unsigned short>(i);
    }

    ApplyFogging(vampContext, vertices);
    ApplyTexEnv(vampContext,*vertices, texenv);

    // Draw the indexed polygons using the Maggie-3D chip
    magDrawIndexedPolygonsUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
}

void DrawTriangleFan(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices, int texenv)
{
    if (vertices->size() < 3) {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(GL_INVALID_VALUE, "Not enough vertices for GL_TRIANGLE_FAN\n");
        return;
    }

    ApplyFogging(vampContext, vertices);
    ApplyTexEnv(vampContext,*vertices, texenv);

    // Create an index buffer for triangle fan drawing
    std::vector<unsigned short> indices(vertices->size() - 2);
    for (size_t i = 0; i < indices.size(); i++) {
        indices[i] = 0;
        indices[i + 1] = static_cast<unsigned short>(i + 1);
        indices[i + 2] = static_cast<unsigned short>(i + 2);
    }

    // Draw the triangle fan using the modified vertex data and index buffer
    magDrawIndexedTrianglesUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
}

void DrawTriangleStrip(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices, int texenv)
{
    if (vertices->size() < 3) {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(GL_INVALID_VALUE, "Not enough vertices for GL_TRIANGLE_STRIP\n");
        return;
    }

    ApplyFogging(vampContext, vertices);
    ApplyTexEnv(vampContext,*vertices, texenv);

    // Create an index buffer for triangle strip drawing
    std::vector<unsigned short> indices(vertices->size() * 2);
    for (size_t i = 0; i < vertices->size() - 2; i++) {
        indices[i * 2] = static_cast<unsigned short>(i);
        indices[i * 2 + 1] = static_cast<unsigned short>(i + 1);
    }

    // Draw the triangle strip using the modified vertex data and index buffer
    magDrawIndexedTrianglesUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
}

void DrawLineStrip(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices, int texenv)
{
    if (vertices->size() < 2) {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(GL_INVALID_VALUE, "Not enough vertices for GL_LINE_STRIP\n");
        return;
    }

    ApplyFogging(vampContext, vertices);
    ApplyTexEnv(vampContext,*vertices, texenv);

    // Iterate over the vertices and draw line segments between adjacent vertices
    for (size_t i = 0; i < vertices->size() - 1; i++) {
        const MaggieVertex& startVertex = (*vertices)[i];
        const MaggieVertex& endVertex = (*vertices)[i + 1];

        struct SpanPosition start, end;
        start.u = startVertex.pos.x;
        start.v = startVertex.pos.y;
        end.u = endVertex.pos.x;
        end.v = endVertex.pos.y;

        magDrawLinearSpan(&start, &end);
    }
}

void DrawLines(GLVampContext* vampContext, std::vector<MaggieVertex>* vertices, int texenv)
{
    if (vertices->size() % 2 != 0) {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(vampContext->glError, "Number of Vertices for GL_LINES not divisible by 2\n");
        return;
    }

    ApplyFogging(vampContext, vertices);
    ApplyTexEnv(vampContext,*vertices, texenv);

    // Iterate over the vertices and draw line segments between pairs of vertices
    for (size_t i = 0; i < vertices->size() - 1; i += 2) {
        const MaggieVertex& startVertex = (*vertices)[i];
        const MaggieVertex& endVertex = (*vertices)[i + 1];

        struct SpanPosition start, end;
        start.u = startVertex.pos.x;
        start.v = startVertex.pos.y;
        end.u = endVertex.pos.x;
        end.v = endVertex.pos.y;

        magDrawLinearSpan(&start, &end);
    }
}

extern "C" void GLEnd(struct GLVampContext *vampContext) 
{	
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
    switch (vampContext->currentMode) {
        case GL_QUADS:
			DrawQuads(vampContext,vertices, vampContext->texenv);
			break;
        case GL_POLYGON:
            DrawPolygons(vampContext,vertices, vampContext->texenv);
            break;
        case GL_TRIANGLE_FAN:
			DrawTriangleFan(vampContext,vertices, vampContext->texenv);
            break;
        case GL_LINE_STRIP:
            DrawLineStrip(vampContext,vertices, vampContext->texenv);
            break;
        case GL_LINES:
			DrawLines(vampContext,vertices, vampContext->texenv);
            break;
        case GL_TRIANGLE_STRIP:
			DrawTriangleStrip(vampContext,vertices, vampContext->texenv);	
            break;
        default:
            break;
    }
	if (!vampContext->manualDraw) magEnd();
}

void GLReadPixels(struct GLVampContext *vampContext, int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels)
{
    unsigned char* screenPixels = (unsigned char *)vampContext->vampScreenPixels[vampContext->vampCurrentBuffer];
    int bpp = vampContext->vampBpp;
    int screen_width = vampContext->vampWidth;

    // Calculate the starting index in the screenPixels array based on the specified coordinates
    int start_index = (y * screen_width + x) * bpp;

    // Determine the order of color components based on the format parameter
    int alpha_offset = (format == GL_BGRA) ? 3 : -1;

    // Iterate over the specified region and copy the pixels to the provided memory location
    for (int row = 0; row < height; row++)
    {
        // Calculate the offset in the screenPixels array for the current row
        int row_offset = start_index + (row * screen_width * bpp);

        // Calculate the offset in the destination pixels array for the current row
        int dest_row_offset = row * width * bpp;

        // Copy the pixels from screenPixels to the destination pixels array
        for (int col = 0; col < width; col++)
        {
            // Calculate the offset in the screenPixels array for the current column
            int col_offset = col * bpp;

            // Calculate the offset in the destination pixels array for the current column
            int dest_col_offset = col * bpp;

            // Copy and convert the pixels from screenPixels to the destination pixels array
            for (int i = 0; i < bpp; i++)
            {
                // Calculate the index in the screenPixels array for the current pixel
                int pixel_index = row_offset + col_offset + i;

                // Calculate the index in the destination pixels array for the current pixel
                int dest_pixel_index = dest_row_offset + dest_col_offset + i;

                // Determine the color component to copy based on the format parameter
                unsigned char color_component = screenPixels[pixel_index];

                // Handle the alpha component when present
                if (alpha_offset != -1 && i == alpha_offset)
                {
                    // Copy the alpha component from screenPixels
                    color_component = screenPixels[pixel_index];
                }
                else if (alpha_offset == -1 && i == bpp - 1)
                {
                    // Set alpha to maximum value (opaque) when there is no alpha component
                    color_component = 255;
                }

                // Convert and store the color component in the destination pixels array based on the specified type
                if (type == GL_UNSIGNED_BYTE)
                    ((unsigned char*)pixels)[dest_pixel_index] = color_component;
                else if (type == GL_UNSIGNED_SHORT)
                    ((unsigned short*)pixels)[dest_pixel_index] = color_component << 8;
                else if (type == GL_FLOAT)
                    ((float*)pixels)[dest_pixel_index] = (float)color_component / 255.0f;
            }
        }
    }
}