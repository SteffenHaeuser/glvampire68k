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
	magBegin();
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
            break;
        default:
            break;
    }
	magEnd();
}
