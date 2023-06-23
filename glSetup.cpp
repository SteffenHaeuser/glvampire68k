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

extern "C" void GLViewport(struct GLVampContext *vampContext, int x, int y, int width, int height)
{
	if ((width!=vampContext->vampWidth)||(height!=vampContext->vampHeight))
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"Called glViewport with invalid size, needs to be the same size as when calling gluOpenDisplayTags\n");
        return;
	}
	if (vampContext->screenMemSize!=0)
	{
		FreeMem(vampContext->screenMem, vampContext->screenMemSize);		
	}
	vampContext->screenMem = (UBYTE*)AllocMem(width * height * vampContext->vampBpp * 3, MEMF_ANY | MEMF_CLEAR);
    if (vampContext->screenMem== NULL)
    {
		vampContext->glError = GL_OUT_OF_MEMORY;
		GenerateGLError(GL_OUT_OF_MEMORY,"Could not allocate Screen Memory\n");
        return;
    }
	vampContext->screenMemSize = width*height*vampContext->vampBpp*3;

	vampContext->vampScreenPixels[0] = (UWORD *)vampContext->screenMem;
	for(LONG i = 1; i < 3; ++i)
		vampContext->vampScreenPixels[i] = vampContext->vampScreenPixels[i - 1] + width * height;
	
	APTR pixels = vampContext->vampScreenPixels[vampContext->vampCurrentBuffer];
	magSetScreenMemory((void **)pixels, width, height);
	vampContext->vampWidth = width;
	vampContext->vampHeight = height;
}

extern "C" void GLCullFace(struct GLVampContext *vampContext,int i)
{
	if (i==GL_FRONT)
	{
		vampContext->vampDrawModes|= MAG_DRAWMODE_CULL_CCW;
	}
	else
	{
		vampContext->vampDrawModes &= ~MAG_DRAWMODE_CULL_CCW;
	}
}

extern "C" void GLDrawBuffer(struct GLVampContext *vampContext, int i)
{
}

extern "C" void GLPolygonMode(struct GLVampContext *vampContext, int i, int j)
{
}

extern "C" void GLGetFloatv(struct GLVampContext *vampContext, int pname, float* params)
{
	if (params==0)
	{
		vampContext->glError = GL_INVALID_VALUE;
		GenerateGLError(vampContext->glError,"Second Parameter for glGetFloatv was null\n");		
	}
    if (pname == GL_MODELVIEW_MATRIX)
    {
        // Copy the values from the provided matrix to params
        const mat4& matrix = *reinterpret_cast<const mat4*>(params);
        for (int i = 0; i < 16; i++)
        {
            params[i] = matrix.m[i / 4][i % 4];
        }
    }
	else 
	{
		char error[1024];
		
		vampContext->glError = GL_INVALID_ENUM;
		sprintf(error,"glGetFloatv was called with incompatible parameter %d\n",pname);
		GenerateGLError(vampContext->glError,error);	
	}
}

extern "C" void GLFrustum(struct GLVampContext *vampContext, float left, float right, float bottom, float top, float nearVal, float farVal) 
{
    float A = (right + left) / (right - left);
    float B = (top + bottom) / (top - bottom);
    float C = -(farVal + nearVal) / (farVal - nearVal);
    float D = -(2.0f * farVal * nearVal) / (farVal - nearVal);

    GLLoadIdentity(vampContext);

    vampContext->currentMatrix->m[0][0] = (2.0f * nearVal) / (right - left);
    vampContext->currentMatrix->m[1][1] = (2.0f * nearVal) / (top - bottom);
    vampContext->currentMatrix->m[2][0] = A;
    vampContext->currentMatrix->m[2][1] = B;
    vampContext->currentMatrix->m[2][2] = C;
    vampContext->currentMatrix->m[2][3] = -1.0f;
    vampContext->currentMatrix->m[3][2] = D;
}

extern "C" void GLOrtho(struct GLVampContext *vampContext, float left, float right, float bottom, float top, float nearVal, float farVal) 
{
    GLLoadIdentity(vampContext);

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(farVal + nearVal) / (farVal - nearVal);

    vampContext->currentMatrix->m[0][0] = 2.0f / (right - left);
    vampContext->currentMatrix->m[1][1] = 2.0f / (top - bottom);
    vampContext->currentMatrix->m[2][2] = -2.0f / (farVal - nearVal);
    vampContext->currentMatrix->m[3][0] = tx;
    vampContext->currentMatrix->m[3][1] = ty;
    vampContext->currentMatrix->m[3][2] = tz;
}
