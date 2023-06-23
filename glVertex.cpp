#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>

#include <proto/exec.h>
#include <proto/Maggie.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

extern struct Library *MaggieBase;

extern "C" void GLVertex3f(struct GLVampContext *vampContext, float x, float y, float z)
{
    MaggieVertex vertex;
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
    vertex.pos = {x, y, z};
    vertices->push_back(vertex);	
	magVertex(x,y,z);
}

extern "C" void GLVertex2f(struct GLVampContext *vampContext, float x, float y)
{
    MaggieVertex vertex;
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
    vertex.pos = {x, y, 0.0f};
    vertices->push_back(vertex);	
	magVertex(x,y,0.0f);
}

extern "C" void GLNormal3f(struct GLVampContext *vampContext, float x, float y, float z)
{
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
    if (!vertices->empty()) {
        MaggieVertex& currentVertex = vertices->back();
        currentVertex.normal = {x, y, z};
    }	
	else
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,"glNormal3f was called before glVertex was called\n");
	}
	magNormal(x,y,z);
}

extern "C" void GLVertex3fv(struct GLVampContext *vampContext, float *vec)
{
	std::vector<MaggieVertex> *vertices;
	
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;
	
	if (vec!=0)
	{
		MaggieVertex vertex;
		vertex.pos = {vec[0],vec[1],vec[2]};
		vertices->push_back(vertex);		
		magVertex(vec[0],vec[1],vec[2]);
	}
	else
	{
		vampContext->glError = GL_INVALID_VALUE;
		GenerateGLError(vampContext->glError,"Vector for glVertex3fv was empty\n");
	}
}

extern "C" void GLTexCoord2f(struct GLVampContext *vampContext, float x, float y)
{
	if (vampContext->currentTexture!=-1)
	{
		magTexCoord(vampContext->currentTexture,x,y);
	}
	else 
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,"No Texture was bound on call of glTexCoord2f\n");		
	}
}