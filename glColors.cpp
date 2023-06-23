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

extern void magClearColor(unsigned int l);  // To make it compile, will be removed once added to the maggie.library

extern "C" void GLColor4f(struct GLVampContext *vampContext, float r, float g, float b, float a)
{
	unsigned char rdec = (unsigned char)r;
	unsigned char gdec = (unsigned char)g;
	unsigned char bdec = (unsigned char)b;
	unsigned char adec = (unsigned char)a;
	
	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);
}

extern "C" void GLColor3f(struct GLVampContext *vampContext, float x, float y, float z)
{
	unsigned char rdec = (unsigned char)x;
	unsigned char gdec = (unsigned char)y;
	unsigned char bdec = (unsigned char)z;
	unsigned char adec = 1;
	
	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);	
}

extern "C" void GLColor4ub(struct GLVampContext *vampContext, int i, int j, int k, int l)
{
	unsigned char rdec = (unsigned char)i;
	unsigned char gdec = (unsigned char)j;
	unsigned char bdec = (unsigned char)k;
	unsigned char adec = (unsigned char)l;

	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);	
}

extern "C" void GLColor4ubv(struct GLVampContext *vampContext, unsigned char *col)
{
	if (col!=0)
	{
		GLColor4ub(vampContext, col[0],col[1],col[2],col[3]);
	}
	else 
	{
		vampContext->glError = GL_INVALID_VALUE;
		GenerateGLError(vampContext->glError,"Vector for glColor4ubv is null\n");	
	}
}

extern "C" void GLColor4fv(struct GLVampContext *vampContext, float *v)
{
	if (v!=0)
	{
		GLColor4f(vampContext, v[0],v[1],v[2],v[3]);
	}
	else 
	{
		vampContext->glError = GL_INVALID_VALUE;
		GenerateGLError(vampContext->glError,"Vector for glColor4fv is null\n");			
	}
}

extern "C" void GLColor3fv(struct GLVampContext *vampContext, float *v)
{
	if (v!=0)
	{
		GLColor3f(vampContext,v[0],v[1],v[2]);
	}
	else 
	{
		vampContext->glError = GL_INVALID_VALUE;
		GenerateGLError(vampContext->glError,"Vector for glColor3fv is null\n");			
	}
}

extern "C" void GLClearColor(struct GLVampContext *vampContext, float i, float j, float k, float l)
{
	unsigned int rgb;
	
	unsigned char a = (unsigned char)(255*l);
	unsigned char r = (unsigned char)(255*i);
	unsigned char g = (unsigned char)(255*j);
	unsigned char b = (unsigned char)(255*k);
	rgb = (a<<24)|(r<<16)|(g<<8)|(b);
	magClearColor(rgb);
}

extern "C" void GLClear(struct GLVampContext *vampContext, unsigned int i)
{
	unsigned short clearMode = 0;
	if (i&GL_COLOR_BUFFER_BIT) clearMode|=MAG_CLEAR_COLOUR;
	if (i&GL_DEPTH_BUFFER_BIT) clearMode|=MAG_CLEAR_DEPTH;
	if (clearMode>0) magClear(clearMode);
	else
	{
		vampContext->glError = GL_INVALID_ENUM;
		GenerateGLError(vampContext->glError,"Maggie Chip only support GL_LEQUAL for glDepthFunc\n");
	}
}
