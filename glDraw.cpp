#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>
#include <stdlib.h>

#include <proto/exec.h>
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

extern "C" void GLEnd(struct GLVampContext *vampContext) 
{	
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
    switch (vampContext->currentMode) {
        case GL_QUADS:
            if (vertices->size() % 4 != 0) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(GL_INVALID_VALUE,"Number of vertices for GL_QUADS not divisible by 4\n");
                break;
            }
            {
                std::vector<unsigned short> indices(vertices->size());
                for (size_t i = 0; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedPolygonsUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
			break;
        case GL_POLYGON:
            if (vertices->size() < 3) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for polygon\n");
                break;
            }
            {
                std::vector<unsigned short> indices(vertices->size());
                for (size_t i = 0; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedPolygonsUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
            break;
        case GL_TRIANGLE_FAN:
#if 0 // Not needed		
            if (vertices->size() < 3) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for GL_TRIANGLE_FAN\n");
                break;
            }
            {
                std::vector<unsigned short> indices(vertices->size());
                indices[0] = 0;
                for (size_t i = 1; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedTrianglesUP(&((*vertices)[0]), static_cast<unsigned short>(vertices->size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
#endif			
            break;
        case GL_LINE_STRIP:
            if (vertices->size() < 2) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for GL_LINE_STRIP\n");
                break;
            }
            {
                struct SpanPosition start, end;
                // Initialisierung der Start- und Endposition entsprechend der Vertices
                start.u = (*vertices)[0].pos.x;
                start.v = (*vertices)[0].pos.y;
                end.u = (*vertices)[vertices->size() - 1].pos.x;
                end.v = (*vertices)[vertices->size() - 1].pos.y;
                magDrawLinearSpan(&start, &end);
            }
            break;
        case GL_LINES:
#if 0		
            if (vertices->size() % 2 != 0) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Number of Vertices for GL_LINES not divisible by 2\n");
                break;
            }
            {
                struct MaggieClippedVertex start, end;
                // Initialisierung des Start- und Endvertices entsprechend den Vertices
                start.pos = (*vertices)[0].pos;
                end.pos = (*vertices)[vertices->size() - 1].pos;
                magDrawSpan(&start, &end);
            }
#else
            if (vertices->size() % 2 != 0) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Number of Vertices for GL_LINES not divisible by 2\n");
                break;
            }
            {
                for (size_t i = 0; i < vertices->size(); i += 2) {
                    struct SpanPosition start, end;
                    // Initialisierung der Start- und Endposition entsprechend der Vertices
                    start.u = (*vertices)[i].pos.x;
                    start.v = (*vertices)[i].pos.y;
                    end.u = (*vertices)[i + 1].pos.x;
                    end.v = (*vertices)[i + 1].pos.y;
                    magDrawLinearSpan(&start, &end);
                }
            }
            break;	
#endif			
            break;
        case GL_TRIANGLE_STRIP:
#if 0 // Not needed		
            if (vertices->size() < 3) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for GL_TRIANGLE_STRIP\n");
                break;
            }
            {
                struct SpanPosition start, end;
                // Initialisierung der Start- und Endposition entsprechend der Vertices
                start.u = (*vertices)[0].pos.x;
                start.v = (*vertices)[0].pos.y;
                end.u = (*vertices)[vertices->size() - 1].pos.x;
                end.v = (*vertices)[vertices->size() - 1].pos.y;
                magDrawLinearSpan(&start, &end);
            }
#endif			
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