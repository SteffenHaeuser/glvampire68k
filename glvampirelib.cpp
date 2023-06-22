#include "glvampire.h"
#include "glvampiredefs.h"

#include <proto/exec.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

struct GLVampContext vampContext;
void GLGenerateError(GLVampContext *vampContext, int type, const char* message);

struct GLVampContext *GLUOpenDisplay(struct GLVampContext *vampContext, int BPP, int maggieMode, int width, int height)
{
	volatile UWORD *SAGA_ScreenMode = (UWORD *)0xdff1f4;
	volatile UWORD *SAGA_ScreenModeRead = (UWORD *)0xdfe1f4;
	volatile ULONG *SAGA_ChunkyDataRead = (ULONG *)0xdfe1ec;	
	
	vampContext->MaggieBase = OpenLibrary("maggie.library", 0);

	if(!vampContext->MaggieBase)
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"Could not open maggie.library\n");
		return 0;
	}
	
	vampContext->vampBpp = BPP;
	vampContext->vampCurrentBuffer = 0;
	vampContext->currentTexture = -1;
	vampContext->vampDrawModes = 0;
	vampContext->currentMatrix = &vampContext->projectionMatrix;
	vampContext->vampWidth = width;
	vampContext->vampHeight = height;
	vampContext->maxVampTex = 1;
	vampContext->vampTextureMap = new std::map<int,int>();
	vampContext->glError = GL_NO_ERROR;
	vampContext->vampScreenPixels[0] = vampContext->vampScreenPixels[1] = vampContext->vampScreenPixels[2] = 0;
	vampContext->oldMode = *SAGA_ScreenModeRead;
	vampContext->oldScreen = *SAGA_ChunkyDataRead;
	vampContext->screenMemSize = 0;
	vampContext->screenMem = 0;
	vampContext->glDebugMessage = 0;
	if (vampContext->vampBpp==4)
	{
		vampContext->vampDrawModes|= MAG_DRAWMODE_32BIT;
	}
	else if (vampContext->vampBpp==2)
	{
		vampContext->vampDrawModes &= ~MAG_DRAWMODE_32BIT;
	}		
	mat4_identity(&vampContext->worldMatrix);

	vampContext->maggieMode = maggieMode; // Z.B. 0x0b02 für 640x360x16bpp
	*SAGA_ScreenMode = maggieMode;	
	
	return vampContext;
}

void GLUCloseDisplay(struct GLVampContext *vampContext)
{
	volatile UWORD *SAGA_ScreenMode = (UWORD *)0xdff1f4;
	volatile ULONG *SAGA_ChunkyData = (ULONG *)0xdff1ec;
	
	*SAGA_ScreenMode = vampContext->oldMode;
	*SAGA_ChunkyData = vampContext->oldScreen;	
	if (vampContext->screenMem)
	{
		FreeMem(vampContext->screenMem, vampContext->screenMemSize);
		vampContext->screenMemSize = 0;
		vampContext->screenMem = 0;
	}
	if (vampContext->vampTextureMap)
	{
		delete vampContext->vampTextureMap;
	}
	vampContext->vampTextureMap = 0;
	if (vampContext->MaggieBase) CloseLibrary(vampContext->MaggieBase);
	vampContext->MaggieBase = 0;
}