#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/lowlevel.h>
#include <proto/Picasso96.h>
#include <exec/execbase.h>

#include <proto/Maggie.h>
#include <maggie_vec.h>
#include <maggie_vertex.h>
#include <maggie_flags.h>
#include "glvampire.h"
#include "glvampiredefs.h"

# define MAGGIE_MODE 0x0b02		// 640x360x16bpp

/*****************************************************************************/

static volatile int vblPassed;

/*****************************************************************************/

static int VBLInterrupt()
{
	vblPassed = 1;
	return 0;
}

/*****************************************************************************/

void WaitVBLPassed()
{
	while(!vblPassed)
		;
	vblPassed = 0;
}


/*****************************************************************************/

int LoadTexture(const char *filename)
{
	UBYTE *data = NULL;
	FILE *fp = fopen(filename, "rb");
	if(!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 128, SEEK_SET);

	data = (UBYTE*)malloc(size - 128);
	fread(data, 1, size - 128, fp);
	fclose(fp);
	
	glTexImage2D(GL_TEXTURE_2D,0,GL_SOLID_FORMAT,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,data);	

	free(data);

	return 1;
}

/*****************************************************************************/

static struct MaggieVertex CubeVertices[6 * 4] = 
{
	{ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
	{ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
	{ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
	{ { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
	{ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
	{ { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
	{ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
	{ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
	{ {-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
	{ { 1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
	{ { 1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
	{ {-1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
	{ {-1.0f,  1.0f, -1.0f}, {0.0f,  1.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
	{ {-1.0f,  1.0f,  1.0f}, {0.0f,  1.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
	{ { 1.0f,  1.0f,  1.0f}, {0.0f,  1.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
	{ { 1.0f,  1.0f, -1.0f}, {0.0f,  1.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
	{ {-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
	{ {-1.0f, -1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
	{ {-1.0f,  1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
	{ {-1.0f,  1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
	{ { 1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
	{ { 1.0f,  1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
	{ { 1.0f,  1.0f,  1.0f}, { 1.0f, 0.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
	{ { 1.0f, -1.0f,  1.0f}, { 1.0f, 0.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
};

/*****************************************************************************/

UWORD CubeIndices[5 * 6 - 1] =
{
	 0,  1,  2,  3, 0xffff,
	 4,  5,  6,  7, 0xffff,
	 8,  9, 10, 11, 0xffff,
	12, 13, 14, 15, 0xffff,
	16, 17, 18, 19, 0xffff,
	20, 21, 22, 23
};

/*****************************************************************************/

int main(int argc, char *argv[])
{
	int texNum = 1;
	int i;
	struct TagItem maggieTags[6];
	
	maggieTags[0].ti_Tag = VAMPOD_BPP;
	maggieTags[0].ti_Data = 2;
	maggieTags[1].ti_Tag = VAMPOD_MODE;
	maggieTags[1].ti_Data = MAGGIE_MODE;
	maggieTags[2].ti_Tag = VAMPOD_WIDTH;
	maggieTags[2].ti_Data = 640;
	maggieTags[3].ti_Tag = VAMPOD_HEIGHT;
	maggieTags[3].ti_Data = 360;
	maggieTags[4].ti_Tag = VAMPOD_USEINT;
	maggieTags[4].ti_Data = 1;
	maggieTags[5].ti_Tag = TAG_DONE;
	maggieTags[5].ti_Data = 0;
	
	int res = gluOpenDisplayTags(maggieTags);
	if (!res)
	{
		printf("gluOpenDisplay() failed\n");
		return 0;
	}
	glViewport(0,0,640,360);

	float targetRatio = 9.0f / 16.0f;

	mat4 worldMatrix, viewMatrix, perspective;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,9.0f);
	glMatrixMode(GL_PROJECTION);
	
	mat4_perspective(&perspective, 60.0f, targetRatio, 0.01f, 100.0f);
	for (i=0;i<16;i++)
	{
		vampContext.projectionMatrix.m[i/4][i%4] = perspective.m[i / 4][i % 4];
	}	

	res = LoadTexture("TestTexture.dds");
	if (!res)
	{
		gluCloseDisplay();	

		return 0;
	}
	glBindTexture(GL_TEXTURE_2D,1);

	float xangle = 0.0f;
	float yangle = 0.0f;
	while(!(ReadJoyPort(0) & JPF_BUTTON_RED)) // While left mouse button not pressed
	{
		unsigned char r,g,b,a;
		mat4 xRot, yRot;		
		
		mat4_rotateX(&xRot, xangle);
		mat4_rotateY(&yRot, yangle);
		mat4_mul(&worldMatrix, &xRot, &yRot);
		
		for (i=0;i<16;i++)
		{
			vampContext.worldMatrix.m[i/4][i%4] = worldMatrix.m[i/4][i%4];
		}			
		
		gluBeginFrame();

		glClear(GL_COLOR_BUFFER_BIT);

		const int polyOffset[6] =
		{
			0, 1, 2, 0, 2, 3
		};

		a = (CubeVertices[0].colour&&0xff000000)>>24;
		r = (CubeVertices[0].colour&&0xff0000)>>16;
		g = (CubeVertices[0].colour&&0xff00)>>8;
		b = (CubeVertices[0].colour&&0xff);
		
		glBegin(GL_POLYGON);
		
		glColor4f(r,g,b,a);
		for(int i = 0; i < 6; ++i)
		{
			for(int j = 0; j < 6; ++j)
			{
				int vIndx = i * 4 + polyOffset[j];
				glVertex3f(CubeVertices[vIndx].pos.x, CubeVertices[vIndx].pos.y, CubeVertices[vIndx].pos.z);
				glTexCoord2f(CubeVertices[vIndx].tex[0].u, CubeVertices[vIndx].tex[0].v);				
			}
			glNormal3f(CubeVertices[i * 4].normal.x, CubeVertices[i * 4].normal.y, CubeVertices[i * 4].normal.z);			
		}
		glEnd();

		gluEndFrame();

		xangle += 0.01f;
		yangle += 0.0123f;
	}
	glDeleteTextures(1,&texNum);

	gluCloseDisplay();

	return 0;
}
