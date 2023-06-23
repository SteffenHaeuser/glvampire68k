#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>

#include <proto/exec.h>
#include <proto/lowlevel.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <cybergraphics/cybergraphics.h>
#include <intuition/intuition.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

struct Library *MaggieBase = 0;
struct Library *CyberGfxBase = 0;

struct GLVampContext vampContext;

static volatile int vblPassed;

static int VBLInterrupt()
{
	vblPassed = 1;
	return 0;
}

void WaitVBLPassed()
{
	while(!vblPassed)
		;
	vblPassed = 0;
}

extern "C" void GLUBeginFrame(struct GLVampContext *vampContext)
{
	volatile ULONG *SAGA_ChunkyData = (ULONG *)0xdff1ec;
	
	int i;
	float matrix[16];
	if (vampContext->vampScreenPixels[vampContext->vampCurrentBuffer]==0) 
	{
		// glViewport was not yet called !!!
		return;
	}
	
	if (vampContext->interrupt)
	{
		WaitVBLPassed();
	}
	
	magBeginScene();
	if (vampContext->window==0)
	{
		*SAGA_ChunkyData = (ULONG)vampContext->vampScreenPixels[vampContext->vampCurrentBuffer];
	}
	vampContext->vampCurrentBuffer = (vampContext->vampCurrentBuffer + 1) % 3;
	APTR pixels = vampContext->vampScreenPixels[vampContext->vampCurrentBuffer];	
	magSetDrawMode(vampContext->vampDrawModes);
	magSetScreenMemory((void **)pixels, vampContext->vampWidth, vampContext->vampHeight);		
	for (i=0;i<16;i++)
	{
		matrix[i] = vampContext->projectionMatrix.m[i / 4][i % 4];
	}
	magSetPerspectiveMatrix(matrix);
	for (i=0;i<16;i++)
	{
		matrix[i] = vampContext->modelViewMatrix.m[i / 4][i % 4];
	}
	magSetViewMatrix(matrix);	
	for (i=0;i<16;i++)
	{
		matrix[i] = vampContext->worldMatrix.m[i / 4][i % 4];
	}	
	magSetWorldMatrix(matrix);
	if (vampContext->currentTexture!=-1)
	{
		magSetTexture(0,vampContext->currentTexture);
	}
}

extern "C" void GLUEndFrame(struct GLVampContext *vampContext)
{
	int borderWidth, borderHeight;
	
	if (vampContext->window)
	{
		borderWidth = vampContext->window->BorderLeft + vampContext->window->BorderRight;
		borderHeight = vampContext->window->BorderTop + vampContext->window->BorderBottom;
	}
	
	magEndScene();
	if (vampContext->window)
	{
		if (vampContext->vampBpp==4)
		{
			WritePixelArray(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer], 0, 0, vampContext->vampWidth * 4, vampContext->window->RPort, vampContext->window->BorderLeft, vampContext->window->BorderTop, vampContext->window->Width - borderWidth, vampContext->window->Height - borderHeight, RECTFMT_RGBA);
		}
		else
		{
			WritePixelArray(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer], 0, 0, vampContext->vampWidth * 2, vampContext->window->RPort, vampContext->window->BorderLeft, vampContext->window->BorderTop, vampContext->window->Width - borderWidth, vampContext->window->Height - borderHeight, RECTFMT_RGB);			
		}
	}
}

extern "C" int GLUOpenDisplayTags(struct GLVampContext *vampContext, struct TagItem *tags)
{
	std::vector<MaggieVertex> *vertices;
	std::map<int,int> *vampTextureMap;
	
	volatile UWORD *SAGA_ScreenMode = (UWORD *)0xdff1f4;
	volatile UWORD *SAGA_ScreenModeRead = (UWORD *)0xdfe1f4;
	volatile ULONG *SAGA_ChunkyDataRead = (ULONG *)0xdfe1ec;	
	int i;
	int BPP = -1;
	int maggieMode = -1;
	int width = -1;
	int height = -1;
	char *name;
	
	if (!tags)
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"No Tags given for GLUOpenDisplayTags\n");
		return 0;
	}
	
	vampContext->useInterrupt = 0;
	
	while ((tags[i].ti_Tag!=TAG_DONE)&&(tags[i].ti_Tag!=TAG_END))
	{
		switch (tags[i].ti_Tag)
		{
			case VAMPOD_BPP:
				BPP = tags[i].ti_Data;
				if ((BPP!=4)&&(BPP!=2)) BPP = -1;
				break;
			case VAMPOD_MODE:
				maggieMode = tags[i].ti_Data;
				switch(maggieMode)
				{
					case 0x0b02:
						width = 640;
						height = 360;
						break;
					default:
						break;
				}
				break;
			case VAMPOD_WIDTH:
				width = tags[i].ti_Data;
				break;
			case VAMPOD_HEIGHT:
				height = tags[i].ti_Data;
				break;
			case VAMPOD_WINDOW:
				vampContext->useWindow = tags[i].ti_Data;
				break;
			case VAMPOD_WINHANDLE:
				vampContext->window = (struct Window *)tags[i].ti_Data;
				break;
			case VAMPOD_USEINT:
				vampContext->useInterrupt = tags[i].ti_Data;
				break;
			case VAMPOD_WINDOWTITLE:
				name = (char*)tags[i].ti_Data;
				break;
			default:
				break;
		}
	}
	
	if ((width==640)&&(height==360)&&(maggieMode!=0x0b02))
	{
		maggieMode = 0x0b02;
	}	
	
	if ((BPP==-1)||(maggieMode==-1)||(width==-1)||(height==-1))
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"Inconsistent Tags in GLUOpenDisplayTags\n");
		
		return 0;
	}
	
	MaggieBase = OpenLibrary("maggie.library", 0);

	if(!MaggieBase)
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"Could not open maggie.library\n");
		return 0;
	}
	
	CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
	if (!CyberGfxBase)
	{
		if (MaggieBase) CloseLibrary(MaggieBase);
		MaggieBase = 0;
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"Could not open cybergraphics.library\n");
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
	vampContext->matrixStack = (void*)(new std::stack<mat4*>());
	if (vampContext->matrixStack==0)
	{
			vampContext->glError = GL_OUT_OF_MEMORY;
			GenerateGLError(GL_OUT_OF_MEMORY,"Could not allocate memory\n");
			return 0;			
	}
	vampContext->vertices = (void *)(new std::vector<MaggieVertex>());
	if (vampContext->vertices==0)
	{
			delete (std::stack<mat4*>*)vampContext->matrixStack;
			vampContext->matrixStack = 0;
			vampContext->glError = GL_OUT_OF_MEMORY;
			GenerateGLError(GL_OUT_OF_MEMORY,"Could not allocate memory\n");
			return 0;			
	}
	vertices = (std::vector<MaggieVertex>*)vampContext->vertices;		
	vertices->clear();
	vampContext->vampTextureMap = (void*)new std::map<int,int>();
	if (vampContext->vampTextureMap==0)
	{
			delete (std::stack<mat4*>*)vampContext->matrixStack;
			vampContext->matrixStack = 0;
			delete (std::vector<MaggieVertex>*)vampContext->vertices;
			vampContext->vertices = 0;
			vampContext->glError = GL_OUT_OF_MEMORY;
			GenerateGLError(GL_OUT_OF_MEMORY,"Could not allocate memory\n");
			return 0;				
	}
	vampTextureMap = (std::map<int,int>*)vampContext->vampTextureMap;
	vampTextureMap->clear();
	vampContext->glError = GL_NO_ERROR;
	vampContext->vampScreenPixels[0] = vampContext->vampScreenPixels[1] = vampContext->vampScreenPixels[2] = 0;
	
	if (vampContext->useInterrupt)
	{
		SystemControl(SCON_TakeOverSys, TRUE, TAG_DONE);
	}	
	
	if (vampContext->useWindow)
	{
		char thename[1024];
		if (name==0) strcpy(thename,"Window");
		else strncpy(thename,name,1000);
		vampContext->window = OpenWindowTags(NULL,
										WA_InnerWidth, width,
										WA_InnerHeight, height,
										WA_Activate, TRUE,
										WA_SimpleRefresh, TRUE,
										WA_ReportMouse, TRUE,
										WA_DragBar, TRUE,
										WA_RMBTrap, TRUE,
										WA_PubScreen, (ULONG)NULL,
										WA_Title, (ULONG)thename,
										WA_MouseQueue, 1,
										WA_IDCMP,	IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_DELTAMOVE |
													IDCMP_REFRESHWINDOW | IDCMP_ACTIVEWINDOW,
										TAG_DONE);	
										
		if (!vampContext->window)
		{
			if (vampContext->useInterrupt)
			{
				SystemControl(SCON_TakeOverSys, FALSE, TAG_DONE); 
			}
			vampContext->glError = GL_INVALID_OPERATION;
			GenerateGLError(GL_INVALID_OPERATION,"Could not open Window\n");
			return 0;		
		}			
	}
	
	if (vampContext->window==0)
	{
		vampContext->oldMode = *SAGA_ScreenModeRead;
		vampContext->oldScreen = *SAGA_ChunkyDataRead;
	}
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
	
	if (vampContext->window==0)
	{
		*SAGA_ScreenMode = maggieMode;	
	}
	
	if (vampContext->useInterrupt)
	{
		vampContext->interrupt = (void*)AddVBlankInt((void*)VBLInterrupt, NULL);
	}
	
	return 1;
}

extern  "C" void GLUCloseDisplay(struct GLVampContext *vampContext)
{
	volatile UWORD *SAGA_ScreenMode = (UWORD *)0xdff1f4;
	volatile ULONG *SAGA_ChunkyData = (ULONG *)0xdff1ec;
	
	if (vampContext->useInterrupt)
	{
		if (vampContext->interrupt) RemVBlankInt(vampContext->interrupt);		
		vampContext->interrupt = 0;
		SystemControl(SCON_TakeOverSys, FALSE, TAG_DONE); 
		vampContext->useInterrupt = 0;
	}
	
	if (vampContext->window==0)
	{
		*SAGA_ScreenMode = vampContext->oldMode;
		*SAGA_ChunkyData = vampContext->oldScreen;	
	}
	if (vampContext->useWindow)
	{
		if (vampContext->window) CloseWindow(vampContext->window);
		vampContext->window = 0;
		vampContext->useWindow = 0;
	}
	if (vampContext->screenMem)
	{
		FreeMem(vampContext->screenMem, vampContext->screenMemSize);
		vampContext->screenMemSize = 0;
		vampContext->screenMem = 0;
	}
	if (vampContext->vampTextureMap)
	{
		delete (std::map<int,int>*)vampContext->vampTextureMap;
		vampContext->vampTextureMap = 0;
	}
	if (vampContext->vertices)
	{
		delete (std::vector<MaggieVertex>*)vampContext->vertices;
		vampContext->vertices = 0;
	}
	if (vampContext->matrixStack)
	{
		delete (std::stack<mat4*>*)vampContext->matrixStack;
		vampContext->matrixStack = 0;
	}
	if (MaggieBase) CloseLibrary(MaggieBase);
	MaggieBase = 0;
	if (CyberGfxBase) CloseLibrary(CyberGfxBase);
	CyberGfxBase = 0;
	delete (std::stack<mat4*>*)vampContext->matrixStack;
}