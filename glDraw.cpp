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

extern "C" void GLBegin(struct GLVampContext *vampContext, GLenum mode) 
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
	//if ((mode==GL_QUADS)||(mode==GL_POLYGON)||(mode==GL_LINE_STRIP)||(mode==GL_LINES)) vampContext->manualDraw = 1;
	//else vampContext->manualDraw = 0;
	vampContext->manualDraw = 1;
	if (!vampContext->manualDraw) magBegin();
}

ULONG PackColor(ULONG r, ULONG g, ULONG b, ULONG a)
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

extern "C" void GLFogfv(struct GLVampContext* vampContext, GLenum pname, const GLfloat* params)
{
    switch (pname) {
        case GL_FOG_COLOR:
            vampContext->fogParams.color = static_cast<unsigned int>(*params);
            break;
        case GL_FOG_DENSITY:
            vampContext->fogParams.density = *params;
            break;
        case GL_FOG_START:
            vampContext->fogParams.start = *params;
            break;
        case GL_FOG_END:
            vampContext->fogParams.end = *params;
            break;
        case GL_FOG_MODE:
            switch (static_cast<int>(*params)) {
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
            switch (static_cast<int>(*params)) {
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

extern "C" void GLFogi(struct GLVampContext* vampContext, GLenum pname, GLint param)
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
            vampContext->fogParams.density = static_cast<GLfloat>(param);
            break;

        case GL_FOG_START:
            vampContext->fogParams.start = static_cast<GLfloat>(param);
            break;

        case GL_FOG_END:
            vampContext->fogParams.end = static_cast<GLfloat>(param);
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

extern "C" void GLFogf(struct GLVampContext* vampContext, GLenum pname, GLfloat param)
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

void ApplyBlendFunc(GLVampContext* vampContext, std::vector<MaggieVertex>& vertices);
void ApplyBlendEquation(struct GLVampContext* vampContext, std::vector<MaggieVertex>& vertices);
void ApplyAlphaFunc(struct GLVampContext* vampContext, std::vector<MaggieVertex>& vertices);

float ARGB32ColorToFloat(ULONG color)
{
    float red = ((color >> 16) & 0xFF) / 255.0f;
    float green = ((color >> 8) & 0xFF) / 255.0f;
    float blue = (color & 0xFF) / 255.0f;
    float alpha = ((color >> 24) & 0xFF) / 255.0f;

    return red * alpha + green * alpha + blue * alpha;
}

void ApplyTexEnv(struct GLVampContext* vampContext, std::vector<MaggieVertex>& vertices, int texenv)
{
    // Check if the texEnvColor is the default value (0xFFFFFFFF)
    bool isDefaultTexEnvColor = (vampContext->texEnvColor == 0xFFFFFFFF);

    switch (texenv) {
        case GL_REPLACE:
            // GL_REPLACE
            // No modification needed, keep the original texture coordinates
            break;
            
        case GL_MODULATE:
            // GL_MODULATE
            // Iterate over each vertex and multiply its texture coordinates with the vertex color
            for (auto& vertex : vertices) {
                vertex.tex[0].u *= ((vertex.colour >> 16) & 0xFF) / 255.0f;
                vertex.tex[0].v *= ((vertex.colour >> 8) & 0xFF) / 255.0f;
            }
            break;
            
        case GL_DECAL:
            // GL_DECAL
            // Iterate over each vertex and blend its texture coordinates with the vertex color using alpha blending
            for (auto& vertex : vertices) {
                float alpha = ((vertex.colour >> 24) & 0xFF) / 255.0f;
                vertex.tex[0].u = (1.0f - alpha) * vertex.tex[0].u + alpha * ((vertex.colour >> 16) & 0xFF) / 255.0f;
                vertex.tex[0].v = (1.0f - alpha) * vertex.tex[0].v + alpha * ((vertex.colour >> 8) & 0xFF) / 255.0f;
            }
            break;
            
        case GL_BLEND:
            // GL_BLEND
            // Iterate over each vertex and blend its texture coordinates with the vertex color using blending functions
            for (auto& vertex : vertices) {
                float alpha = ((vertex.colour >> 24) & 0xFF) / 255.0f;
                vertex.tex[0].u = vertex.tex[0].u * (1.0f - alpha) + ((vertex.colour >> 16) & 0xFF) / 255.0f * alpha;
                vertex.tex[0].v = vertex.tex[0].v * (1.0f - alpha) + ((vertex.colour >> 8) & 0xFF) / 255.0f * alpha;
            }
            break;
            
        case GL_ADD:
            // GL_ADD
            // Iterate over each vertex and add its texture coordinates to the vertex color
            for (auto& vertex : vertices) {
                vertex.tex[0].u += ((vertex.colour >> 16) & 0xFF) / 255.0f;
                vertex.tex[0].v += ((vertex.colour >> 8) & 0xFF) / 255.0f;
            }
            break;
        
        case GL_COMBINE_RGB:
            // GL_COMBINE_RGB
            // Perform RGB combining based on source factors and operands
            for (auto& vertex : vertices) {
                // Apply RGB combining based on specified source factors and operands
                float srcRGB = 1.0f;
                float operandRGB = 0.0f;

                // Assign the appropriate values based on the specified combineRGB values
                switch (vampContext->combineRGB) {
                    case GL_COMBINE_CONSTANT:
                        srcRGB = ARGB32ColorToFloat(vampContext->combineRGBConstant);
                        break;
                    case GL_COMBINE_PREVIOUS:
                        srcRGB = vertex.tex[0].u;
                        break;
                    case GL_COMBINE_TEXTURE:
                        srcRGB = vertex.tex[0].v;
                        break;
                    // Add cases for other combineRGB source options as needed
                }

                switch (vampContext->combineAlpha) {
                    case GL_SRC_COLOR:
                        operandRGB = ((vertex.colour >> 16) & 0xFF) / 255.0f;
                        break;
                    case GL_ONE_MINUS_SRC_COLOR:
                        operandRGB = 1.0f - ((vertex.colour >> 16) & 0xFF) / 255.0f;
                        break;
                    // Add cases for other combineRGB operand options as needed
                }

                // Perform the RGB combining calculation
                vertex.tex[0].u = srcRGB * operandRGB + vampContext->alphaScale;
                vertex.tex[0].v = srcRGB * operandRGB + vampContext->alphaScale;

                // Apply texEnvColor modulation if GL_COMBINE_RGB involves GL_COMBINE_CONSTANT and texEnvColor is not the default
                if (!isDefaultTexEnvColor && vampContext->combineRGB == GL_COMBINE_CONSTANT) {
                    float red = ((vampContext->texEnvColor >> 16) & 0xFF) / 255.0f;
                    float green = ((vampContext->texEnvColor >> 8) & 0xFF) / 255.0f;
                    float blue = (vampContext->texEnvColor & 0xFF) / 255.0f;

                    vertex.tex[0].u *= red * vampContext->rgbScale;
                    vertex.tex[0].v *= green * vampContext->rgbScale;
                    vertex.tex[0].w *= blue * vampContext->alphaScale;
                }
            }
            break;

        // Existing cases

        default:
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid TexEnv mode");
            break;
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
	if (vampContext->useBlending) 
	{
		ApplyBlendFunc(vampContext,*vertices);
		ApplyBlendEquation(vampContext,*vertices);
	}
	if (vampContext->useAlphaFunc)
	{
		ApplyAlphaFunc(vampContext,*vertices);
	}

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
	if (vampContext->useBlending) 
	{
		ApplyBlendFunc(vampContext,*vertices);
		ApplyBlendEquation(vampContext,*vertices);
	}
	if (vampContext->useAlphaFunc)
	{
		ApplyAlphaFunc(vampContext,*vertices);
	}	

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
	if (vampContext->useBlending) 
	{
		ApplyBlendFunc(vampContext,*vertices);
		ApplyBlendEquation(vampContext,*vertices);
	}
	if (vampContext->useAlphaFunc)
	{
		ApplyAlphaFunc(vampContext,*vertices);
	}
	
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
	if (vampContext->useBlending) 
	{
		ApplyBlendFunc(vampContext,*vertices);
		ApplyBlendEquation(vampContext,*vertices);
	}
	if (vampContext->useAlphaFunc)
	{
		ApplyAlphaFunc(vampContext,*vertices);
	}	

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
	if (vampContext->useBlending) 
	{
		ApplyBlendFunc(vampContext,*vertices);
		ApplyBlendEquation(vampContext,*vertices);
	}
	if (vampContext->useAlphaFunc)
	{
		ApplyAlphaFunc(vampContext,*vertices);
	}	

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
	if (vampContext->useBlending) 
	{
		ApplyBlendFunc(vampContext,*vertices);
		ApplyBlendEquation(vampContext,*vertices);
	}
	if (vampContext->useAlphaFunc)
	{
		ApplyAlphaFunc(vampContext,*vertices);
	}	
	
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

void GLReadPixels(struct GLVampContext* vampContext, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
    unsigned char* screenPixels = reinterpret_cast<unsigned char*>(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer]);
    GLint bpp = vampContext->vampBpp;
    GLint screen_width = vampContext->vampWidth;

    // Calculate the starting index in the screenPixels array based on the specified coordinates
    GLint start_index = (y * screen_width + x) * bpp;

    // Determine the order of color components based on the format parameter
    GLint alpha_offset = (format == GL_BGRA) ? 3 : -1;

    // Iterate over the specified region and copy the pixels to the provided memory location
    for (GLsizei row = 0; row < height; row++)
    {
        // Calculate the offset in the screenPixels array for the current row
        GLint row_offset = start_index + (row * screen_width * bpp);

        // Calculate the offset in the destination pixels array for the current row
        GLint dest_row_offset = row * width * bpp;

        // Copy the pixels from screenPixels to the destination pixels array
        for (GLsizei col = 0; col < width; col++)
        {
            // Calculate the offset in the screenPixels array for the current column
            GLint col_offset = col * bpp;

            // Calculate the offset in the destination pixels array for the current column
            GLint dest_col_offset = col * bpp;

            // Copy and convert the pixels from screenPixels to the destination pixels array
            for (GLint i = 0; i < bpp; i++)
            {
                // Calculate the index in the screenPixels array for the current pixel
                GLint pixel_index = row_offset + col_offset + i;

                // Calculate the index in the destination pixels array for the current pixel
                GLint dest_pixel_index = dest_row_offset + dest_col_offset + i;

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
                    reinterpret_cast<unsigned char*>(pixels)[dest_pixel_index] = color_component;
                else if (type == GL_UNSIGNED_SHORT)
                    reinterpret_cast<unsigned short*>(pixels)[dest_pixel_index] = color_component << 8;
                else if (type == GL_FLOAT)
                    reinterpret_cast<float*>(pixels)[dest_pixel_index] = static_cast<float>(color_component) / 255.0f;
            }
        }
    }
}
