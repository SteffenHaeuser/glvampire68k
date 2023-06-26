#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>

#include <proto/exec.h>
#include <proto/lowlevel.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/Maggie.h>
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
	magSetPerspectiveMatrix((float*)(&vampContext->projectionMatrix));
	magSetViewMatrix((float*)(&vampContext->modelViewMatrix));	
	magSetWorldMatrix((float*)(&vampContext->worldMatrix));
	if (vampContext->currentTexture!=-1)
	{
		magSetTexture(0,vampContext->currentTexture);
	}
}

unsigned int Convert16BitTo32Bit(unsigned short pixel16)
{
    UBYTE red5 = (pixel16 >> 11) & 0x1F;
    UBYTE green6 = (pixel16 >> 5) & 0x3F;
    UBYTE blue5 = pixel16 & 0x1F;
    
    ULONG red8 = (red5 << 3) | (red5 >> 2);
    ULONG green8 = (green6 << 2) | (green6 >> 4);
    ULONG blue8 = (blue5 << 3) | (blue5 >> 2);
    
    ULONG pixel32 = (red8 << 16) | (green8 << 8) | blue8;
    return pixel32;
}

void Copy16BitTo32Bit(unsigned short * srcBuffer, unsigned int * destBuffer, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned short pixel16 = srcBuffer[y * width + x];
            unsigned int pixel32 = Convert16BitTo32Bit(pixel16);
            destBuffer[y * width + x] = pixel32;
        }
    }
}

extern "C" void GLUEndFrame(struct GLVampContext *vampContext)
{
    int borderWidth = 0;
    int borderHeight = 0;
    
    if (vampContext->window)
    {
        borderWidth = vampContext->window->BorderLeft + vampContext->window->BorderRight;
        borderHeight = vampContext->window->BorderTop + vampContext->window->BorderBottom;
    }
    
    magEndScene();
    
    if (vampContext->window)
    {
        if (vampContext->vampBpp == 2)
        {
            // Copy 16-bit buffer to 32-bit buffer
            Copy16BitTo32Bit(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer],
                             vampContext->buffer32,
                             vampContext->vampWidth,
                             vampContext->vampHeight);
        }
        
		WritePixelArray((vampContext->vampBpp == 4) ? reinterpret_cast<unsigned int*>(vampContext->vampScreenPixels[vampContext->vampCurrentBuffer]) : vampContext->buffer32,
                0,
                0,
                vampContext->vampWidth * vampContext->vampBpp,
                vampContext->window->RPort,
                vampContext->window->BorderLeft,
                vampContext->window->BorderTop,
                vampContext->window->Width - borderWidth,
                vampContext->window->Height - borderHeight,
                RECTFMT_RGBA);
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
	char *name=0;
	int reso = -1;
	
	if (!tags)
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(GL_INVALID_OPERATION,"No Tags given for GLUOpenDisplayTags\n");
		return 0;
	}
	
	vampContext->useInterrupt = 0;
	
	i = 0;
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
				reso = maggieMode&0xFF00;
				if (maggieMode&0x02) BPP = 2;
				else if (maggieMode&0x05) BPP = 4;
				else BPP = -1;
				switch(reso)
				{
					case 0x0100:
						width = 320;
						height = 200;
						break;
					case 0x0200:
						width = 320;
						height = 240;
						break;
					case 0x0300:
						width = 320;
						height = 256;
						break;
					case 0x0400:
						width = 640;
						height = 400;
						break;
					case 0x0500:
						width = 640;
						height = 480;
						break;
					case 0x0600:
						width = 640;
						height = 512;
						break;
					case 0x0700:
						width = 960;
						height = 512;
						break;
					case 0x0800:
						width = 480;
						height = 270;
						break;
					case 0x0900:
						width = 304;
						height = 224;
						break;
					case 0x0A00:
						width = 1280;
						height = 720;
						break;
					case 0x0B00:
						width = 640;
						height = 360;
						break;
					case 0x0C00:
						width = 800;
						height = 600;
						break;
					case 0x0D00:
						width = 1024;
						height = 768;
						break;
					case 0x0E00:
						width = 720;
						height = 576;
						break;
					case 0x0F00:
						width = 848;
						height = 480;
						break;
					case 0x1000:
						width = 640;
						height = 200;
						break;
					case 0x1100:
						width = 1920;
						height = 1080;
						break;
					case 0x1200:
						width = 1280;
						height = 1024;
						break;
					case 0x1300:
						width = 1280;
						height = 800;
						break;
					case 0x1400:
						width = 1440;
						height = 900;
						break;
					default:
						width = -1;
						height = -1;
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
		i++;
	}
	
	if ((width==640)&&(height==360)&&(reso!=0x0B00)&&(BPP==2))
	{
		maggieMode = 0x0B02;
	}	
	else if ((width==640)&&(height==360)&&(reso!=0x0B00)&&(BPP==4))
	{
		maggieMode = 0x0B05;
	}
	else if ((width==320)&&(height==200)&&(reso!=0x0100)&&(BPP==2))
	{
		maggieMode = 0x0102;
	}	
	else if ((width==320)&&(height==200)&&(reso!=0x0100)&&(BPP==4))
	{
		maggieMode = 0x0105;
	}
	else if ((width==320)&&(height==240)&&(reso!=0x0200)&&(BPP==2))
	{
		maggieMode = 0x0202;
	}	
	else if ((width==320)&&(height==240)&&(reso!=0x0200)&&(BPP==4))
	{
		maggieMode = 0x0205;
	}	
	else if ((width==320)&&(height==256)&&(reso!=0x0300)&&(BPP==2))
	{
		maggieMode = 0x0302;
	}	
	else if ((width==320)&&(height==256)&&(reso!=0x0300)&&(BPP==4))
	{
		maggieMode = 0x0305;
	}	
	else if ((width==640)&&(height==400)&&(reso!=0x0400)&&(BPP==2))
	{
		maggieMode = 0x0402;
	}	
	else if ((width==640)&&(height==400)&&(reso!=0x0400)&&(BPP==4))
	{
		maggieMode = 0x0405;
	}	
	else if ((width==640)&&(height==480)&&(reso!=0x0500)&&(BPP==2))
	{
		maggieMode = 0x0502;
	}	
	else if ((width==640)&&(height==480)&&(reso!=0x0500)&&(BPP==4))
	{
		maggieMode = 0x0505;
	}	
	else if ((width==640)&&(height==512)&&(reso!=0x0600)&&(BPP==2))
	{
		maggieMode = 0x0602;
	}	
	else if ((width==640)&&(height==512)&&(reso!=0x0600)&&(BPP==4))
	{
		maggieMode = 0x0605;
	}	
	else if ((width==960)&&(height==540)&&(reso!=0x0700)&&(BPP==2))
	{
		maggieMode = 0x0702;
	}	
	else if ((width==960)&&(height==540)&&(reso!=0x0700)&&(BPP==4))
	{
		maggieMode = 0x0705;
	}	
	else if ((width==480)&&(height==270)&&(reso!=0x0800)&&(BPP==2))
	{
		maggieMode = 0x0802;
	}	
	else if ((width==480)&&(height==270)&&(reso!=0x0800)&&(BPP==4))
	{
		maggieMode = 0x0805;
	}	
	else if ((width==304)&&(height==224)&&(reso!=0x0900)&&(BPP==2))
	{
		maggieMode = 0x0902;
	}	
	else if ((width==304)&&(height==224)&&(reso!=0x0900)&&(BPP==4))
	{
		maggieMode = 0x0905;
	}	
	else if ((width==1280)&&(height==720)&&(reso!=0x0A00)&&(BPP==2))
	{
		maggieMode = 0x0A02;
	}	
	else if ((width==1280)&&(height==720)&&(reso!=0x0A00)&&(BPP==4))
	{
		maggieMode = 0x0105;
	}	
	else if ((width==800)&&(height==600)&&(reso!=0x0C00)&&(BPP==2))
	{
		maggieMode = 0x0C02;
	}	
	else if ((width==800)&&(height==600)&&(reso!=0x0C00)&&(BPP==4))
	{
		maggieMode = 0x0C05;
	}	
	else if ((width==1024)&&(height==768)&&(reso!=0x0D00)&&(BPP==2))
	{
		maggieMode = 0x0D02;
	}	
	else if ((width==1024)&&(height==768)&&(reso!=0x0D00)&&(BPP==4))
	{
		maggieMode = 0x0D05;
	}	
	else if ((width==720)&&(height==576)&&(reso!=0x0E00)&&(BPP==2))
	{
		maggieMode = 0x0E02;
	}	
	else if ((width==720)&&(height==576)&&(reso!=0x0E00)&&(BPP==4))
	{
		maggieMode = 0x0E05;
	}	
	else if ((width==848)&&(height==480)&&(reso!=0x0F00)&&(BPP==2))
	{
		maggieMode = 0x0F02;
	}	
	else if ((width==848)&&(height==480)&&(reso!=0x0F00)&&(BPP==4))
	{
		maggieMode = 0x0F05;
	}	
	else if ((width==640)&&(height==200)&&(reso!=0x1000)&&(BPP==2))
	{
		maggieMode = 0x1002;
	}	
	else if ((width==640)&&(height==200)&&(reso!=0x1000)&&(BPP==4))
	{
		maggieMode = 0x1005;
	}	
	else if ((width==1920)&&(height==1080)&&(reso!=0x1100)&&(BPP==2))
	{
		maggieMode = 0x1102;
	}	
	else if ((width==1920)&&(height==1080)&&(reso!=0x1100)&&(BPP==4))
	{
		maggieMode = 0x1105;
	}	
	else if ((width==1280)&&(height==1024)&&(reso!=0x1200)&&(BPP==2))
	{
		maggieMode = 0x1202;
	}	
	else if ((width==1280)&&(height==1024)&&(reso!=0x1200)&&(BPP==4))
	{
		maggieMode = 0x1205;
	}	
	else if ((width==1280)&&(height==800)&&(reso!=0x1300)&&(BPP==2))
	{
		maggieMode = 0x1302;
	}	
	else if ((width==1280)&&(height==800)&&(reso!=0x1300)&&(BPP==4))
	{
		maggieMode = 0x1305;
	}	
	else if ((width==1440)&&(height==900)&&(reso!=0x1400)&&(BPP==2))
	{
		maggieMode = 0x1402;
	}	
	else if ((width==1440)&&(height==900)&&(reso!=0x1400)&&(BPP==4))
	{
		maggieMode = 0x1405;
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
	vampContext->texenv = 0;
	vampContext->blendColor = 0xFFFFFFFF;
	vampContext->blendFuncSrc = GL_ONE;
	vampContext->blendFuncDest = GL_ONE;
	vampContext->blendFuncSrcRGB = GL_ONE;
	vampContext->blendFuncDestRGB = GL_ONE;
	vampContext->blendFuncSrcAlpha = GL_ONE;
	vampContext->blendFuncDestAlpha = GL_ONE;
	vampContext->separateBlendFuncEnabled = 0;
	vampContext->alphaFunc = GL_ALWAYS;
	vampContext->alphaRef = 0.0f;
	vampContext->useAlphaFunc = 0;
	vampContext->useFogging = 0;
	vampContext->useFogCoordSrc = 0;
	vampContext->fogParams.mode = FOG_LINEAR;
	vampContext->fogParams.fogCoordSrc = GL_FRAGMENT_DEPTH;
	vampContext->fogParams.density = 1.0f;
	vampContext->fogParams.start = 0.0f;
	vampContext->fogParams.end = 1.0f;
	vampContext->fogParams.color = 0xFFFFFFFF;
	vampContext->useBlending = 0;
	vampContext->maxVampTex = 1;
	vampContext->blendEquation = GL_FUNC_ADD;
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