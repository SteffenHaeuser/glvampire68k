#include "glvampire.h"

#include <proto/exec.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

#include <map>
#include <stack>
#include <vector>

std::map<int,int> *vampTextureMap = new std::map<int,int>();
int currentTexture = -1;
int maxVampTex = 1;
using MatrixStack = std::stack<mat4*>;

mat4 modelViewMatrix;     // aktuelle Modelview-Matrix
mat4 projectionMatrix;    // aktuelle Projektionsmatrix

// Die aktuelle Matrix, auf die Operationen angewendet werden sollen
mat4* currentMatrix = &modelViewMatrix;
MatrixStack matrixStack; 
int vampDrawModes = 0;
UWORD *vampScreenPixels[3];
int vampCurrentBuffer = 0;
int vampBpp = 4;
int vampWidth = 640;
int vampHeight = 480;

extern void magClearColor(unsigned int l);  // To make it compile, will be removed once added to the maggie.library

int currentMode;
std::vector<MaggieVertex> vertices;

void gluOpenDisplay(int BPP)
{
	vampBpp = BPP;
	if (vampBpp==4)
	{
		vampDrawModes|= MAG_DRAWMODE_32BIT;
	}
	else if (vampBpp==2)
	{
		vampDrawModes &= ~MAG_DRAWMODE_32BIT;
	}		
}

void glViewport(int x, int y, int width, int height)
{
    // Calculate the size of the screen memory buffer
    int bufferSize = width * height * sizeof(UWORD);

	UBYTE *screenMem = (UBYTE*)AllocMem(width * height * vampBpp * 3, MEMF_ANY | MEMF_CLEAR);
    if (screenMem== NULL)
    {
        return;
    }

	vampScreenPixels[0] = (UWORD *)screenMem;
	for(LONG i = 1; i < 3; ++i)
		vampScreenPixels[i] = vampScreenPixels[i - 1] + width * height;
	
	APTR pixels = vampScreenPixels[vampCurrentBuffer];
	magSetScreenMemory((void **)pixels, width, height);
	vampWidth = width;
	vampHeight = height;
}

void glBegin(GLenum mode) {
    switch (mode) {
        case GL_QUADS:
        case GL_POLYGON:
        case GL_TRIANGLE_FAN:
        case GL_LINE_STRIP:
        case GL_LINES:
        case GL_TRIANGLE_STRIP:
            currentMode = static_cast<int>(mode);
            vertices.clear();
            break;
        default:
            break;
    }
}

void glEnd() 
{	
    switch (currentMode) {
        case GL_QUADS:
            if (vertices.size() % 4 != 0) {
                // Fehler: Anzahl der Vertices ist nicht durch 4 teilbar
                break;
            }
            {
                std::vector<unsigned short> indices(vertices.size());
                for (size_t i = 0; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedPolygonsUP(&vertices[0], static_cast<unsigned short>(vertices.size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
			break;
        case GL_POLYGON:
            if (vertices.size() < 3) {
                // Fehler: Nicht genügend Vertices für Polygon
                break;
            }
            {
                std::vector<unsigned short> indices(vertices.size());
                for (size_t i = 0; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedPolygonsUP(&vertices[0], static_cast<unsigned short>(vertices.size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
            break;
        case GL_TRIANGLE_FAN:
            if (vertices.size() < 3) {
                // Fehler: Nicht genügend Vertices für Triangle Fan
                break;
            }
            {
                std::vector<unsigned short> indices(vertices.size());
                indices[0] = 0;
                for (size_t i = 1; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedTrianglesUP(&vertices[0], static_cast<unsigned short>(vertices.size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
            break;
        case GL_LINE_STRIP:
            if (vertices.size() < 2) {
                // Fehler: Nicht genügend Vertices für Line Strip
                break;
            }
            {
                struct SpanPosition start, end;
                // Initialisierung der Start- und Endposition entsprechend der Vertices
                start.u = vertices[0].pos.x;
                start.v = vertices[0].pos.y;
                end.u = vertices[vertices.size() - 1].pos.x;
                end.v = vertices[vertices.size() - 1].pos.y;
                magDrawLinearSpan(&start, &end);
            }
            break;
        case GL_LINES:
#if 0		
            if (vertices.size() % 2 != 0) {
                // Fehler: Anzahl der Vertices ist nicht durch 2 teilbar
                break;
            }
            {
                struct MaggieClippedVertex start, end;
                // Initialisierung des Start- und Endvertices entsprechend den Vertices
                start.pos = vertices[0].pos;
                end.pos = vertices[vertices.size() - 1].pos;
                magDrawSpan(&start, &end);
            }
#else
            if (vertices.size() % 2 != 0) {
                // Fehler: Anzahl der Vertices ist nicht durch 2 teilbar
                break;
            }
            {
                for (size_t i = 0; i < vertices.size(); i += 2) {
                    struct SpanPosition start, end;
                    // Initialisierung der Start- und Endposition entsprechend der Vertices
                    start.u = vertices[i].pos.x;
                    start.v = vertices[i].pos.y;
                    end.u = vertices[i + 1].pos.x;
                    end.v = vertices[i + 1].pos.y;
                    magDrawLinearSpan(&start, &end);
                }
            }
            break;	
#endif			
            break;
        case GL_TRIANGLE_STRIP:
            if (vertices.size() < 3) {
                // Fehler: Nicht genügend Vertices für Triangle Strip
                break;
            }
            {
                struct SpanPosition start, end;
                // Initialisierung der Start- und Endposition entsprechend der Vertices
                start.u = vertices[0].pos.x;
                start.v = vertices[0].pos.y;
                end.u = vertices[vertices.size() - 1].pos.x;
                end.v = vertices[vertices.size() - 1].pos.y;
                magDrawLinearSpan(&start, &end);
            }
            break;
        default:
            break;
    }
}

void glVertex3f(float x, float y, float z)
{
    MaggieVertex vertex;
    vertex.pos = {x, y, z};
    vertices.push_back(vertex);	
	magVertex(x,y,z);
}

void glVertex2f(float x, float y)
{
    MaggieVertex vertex;
    vertex.pos = {x, y, 0.0f};
    vertices.push_back(vertex);	
	magVertex(x,y,0.0f);
}

void glNormal3f(float x, float y, float z)
{
    if (!vertices.empty()) {
        MaggieVertex& currentVertex = vertices.back();
        currentVertex.normal = {x, y, z};
    }	
	magNormal(x,y,z);
}

void glVertex3fv(float *vec)
{
	if (vec!=0)
	{
		MaggieVertex vertex;
		vertex.pos = {vec[0],vec[1],vec[2]};
		vertices.push_back(vertex);		
		magVertex(vec[0],vec[1],vec[2]);
	}
}

void glCullFace(int i)
{
	if (i==GL_FRONT)
	{
		vampDrawModes|= MAG_DRAWMODE_CULL_CCW;
	}
	else
	{
		vampDrawModes &= ~MAG_DRAWMODE_CULL_CCW;
	}
}

void glDepthFunc(int i)
{
	// Maggie only supports GL_LEQUAL
}

void glDepthMask(int i)
{
	// Not really correct implementation, DepthMask should only affect the writing, which currently is not possible on the Maggie Chipset
	
	if (i==GL_TRUE)
	{
		vampDrawModes|= MAG_DRAWMODE_DEPTHBUFFER;
	}
	else
	{
		vampDrawModes &= ~MAG_DRAWMODE_DEPTHBUFFER;
	}
}

void glDrawBuffer(int i)
{
}

void gluBeginFrame()
{
	int i;
	float matrix[16];
	magBeginScene();
	vampCurrentBuffer = (vampCurrentBuffer + 1) % 3;
	APTR pixels = vampScreenPixels[vampCurrentBuffer];	
	magSetDrawMode(vampDrawModes);
	magSetScreenMemory((void **)pixels, vampWidth, vampHeight);		
	for (i=0;i<16;i++)
	{
		matrix[i] = projectionMatrix.m[i / 4][i % 4];
	}
	magSetPerspectiveMatrix(matrix);
	for (i=0;i<16;i++)
	{
		matrix[i] = modelViewMatrix.m[i / 4][i % 4];
	}
	magSetViewMatrix(matrix);	
	if (currentTexture!=-1)
	{
		magSetTexture(0,currentTexture);
	}
}

void gluEndFrame()
{
	magEndScene();
}

void glPolygonMode(int i, int j)
{
}

void glClearColor(float i, float j, float k, float l)
{
	unsigned int rgb;
	
	unsigned char a = (unsigned char)(255*l);
	unsigned char r = (unsigned char)(255*i);
	unsigned char g = (unsigned char)(255*j);
	unsigned char b = (unsigned char)(255*k);
	rgb = (a<<24)|(r<<16)|(g<<8)|(b);
	magClearColor(rgb);
}

void glClear(unsigned int i)
{
	unsigned short clearMode = 0;
	if (i&GL_COLOR_BUFFER_BIT) clearMode|=MAG_CLEAR_COLOUR;
	if (i&GL_DEPTH_BUFFER_BIT) clearMode|=MAG_CLEAR_DEPTH;
	if (clearMode>0) magClear(clearMode);
}

void glMatrixMode(int mode)
{
    if (mode == GL_MODELVIEW)
    {
        currentMatrix = &modelViewMatrix;
    }
    else if (mode == GL_PROJECTION)
    {
        currentMatrix = &projectionMatrix;
    }
}

void glLoadIdentity()
{
	mat4_identity(currentMatrix);
}

void glLoadMatrix(float *matrix) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            currentMatrix->m[i][j] = matrix[i * 4 + j];
        }
    }
}

void glGetFloatv(int pname, float* params)
{
    if (pname == GL_MODELVIEW_MATRIX)
    {
        // Copy the values from the provided matrix to params
        const mat4& matrix = *reinterpret_cast<const mat4*>(params);
        for (int i = 0; i < 16; i++)
        {
            params[i] = matrix.m[i / 4][i % 4];
        }
    }
}

void glOrtho(float left, float right, float bottom, float top, float nearVal, float farVal) 
{
    glLoadIdentity();

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(farVal + nearVal) / (farVal - nearVal);

    currentMatrix->m[0][0] = 2.0f / (right - left);
    currentMatrix->m[1][1] = 2.0f / (top - bottom);
    currentMatrix->m[2][2] = -2.0f / (farVal - nearVal);
    currentMatrix->m[3][0] = tx;
    currentMatrix->m[3][1] = ty;
    currentMatrix->m[3][2] = tz;
}


void glRotatef(float angle, float x, float y, float z) 
{
    glLoadIdentity();

    mat4_rotateX(currentMatrix, x * angle);
    mat4_rotateY(currentMatrix, y * angle);
    mat4_rotateZ(currentMatrix, z * angle);
}

void glFrustum(float left, float right, float bottom, float top, float nearVal, float farVal) 
{
    float A = (right + left) / (right - left);
    float B = (top + bottom) / (top - bottom);
    float C = -(farVal + nearVal) / (farVal - nearVal);
    float D = -(2.0f * farVal * nearVal) / (farVal - nearVal);

    glLoadIdentity();

    currentMatrix->m[0][0] = (2.0f * nearVal) / (right - left);
    currentMatrix->m[1][1] = (2.0f * nearVal) / (top - bottom);
    currentMatrix->m[2][0] = A;
    currentMatrix->m[2][1] = B;
    currentMatrix->m[2][2] = C;
    currentMatrix->m[2][3] = -1.0f;
    currentMatrix->m[3][2] = D;
}



void glPushMatrix()
{
    matrixStack.push(currentMatrix);
}

void glPopMatrix()
{
    if (!matrixStack.empty())
    {
        currentMatrix = matrixStack.top();
        matrixStack.pop();
    }
}


void glTranslatef(float x, float y, float z)
{
	mat4_translate(currentMatrix, x, y, z);
}

void glTexCoord2f(float x, float y)
{
	if (currentTexture!=-1)
	{
		magTexCoord(currentTexture,x,y);
	}
}

void glBindTexture(int i, int j)
{
	if (i==GL_TEXTURE_2D)
	{
		auto num = vampTextureMap->find(j);
		if (num!=vampTextureMap->end())
		{
			currentTexture = num->second;
			magSetTexture(0,currentTexture);
		}
	}
}

void glTexImage2D(int i, int j, int k, int l, int m, int n, int o, int p, void *pixels)
{
	int texHandle = -1;
	int pixsize = 0;
	switch(o)
	{
		case GL_RGBA:
			if (p==GL_UNSIGNED_BYTE) pixsize = 4;
			break;
	}
	if (i==GL_TEXTURE_2D)
	{
		if (l*m==64*64) texHandle = magAllocateTexture(6);
		else if (l*m==128*128) texHandle = magAllocateTexture(7);
		else if (l*m==256*256) texHandle = magAllocateTexture(8);
		if (!texHandle) return;
		vampTextureMap->insert(std::make_pair(maxVampTex, texHandle));
		maxVampTex++;
		magUploadTexture(texHandle, j, pixels, 0);
	}
}

void glDeleteTextures(int num, void *v)
{
	if (num==1)
	{
		int texnum = *((int*)(v));
		
		if (texnum!=0)
		{
			auto it = vampTextureMap->find(texnum);
			if (it != vampTextureMap->end()) 
			{
				int number = it->second;
				vampTextureMap->erase(it);
				magFreeTexture(number);
			}
		}
	}
}

void glColor4f(float r, float g, float b, float a)
{
	unsigned char rdec = (unsigned char)r;
	unsigned char gdec = (unsigned char)g;
	unsigned char bdec = (unsigned char)b;
	unsigned char adec = (unsigned char)a;
	
	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);
}

void glColor3f(float x, float y, float z)
{
	unsigned char rdec = (unsigned char)x;
	unsigned char gdec = (unsigned char)y;
	unsigned char bdec = (unsigned char)z;
	unsigned char adec = 1;
	
	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);	
}

void glColor4ub(int i, int j, int k, int l)
{
	unsigned char rdec = (unsigned char)i;
	unsigned char gdec = (unsigned char)j;
	unsigned char bdec = (unsigned char)k;
	unsigned char adec = (unsigned char)l;

	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);	
}

void glColor4ubv(unsigned char *col)
{
	if (col!=0)
	{
		glColor4ub(col[0],col[1],col[2],col[3]);
	}
}

void glColor4fv(float *v)
{
	if (v!=0)
	{
		glColor4f(v[0],v[1],v[2],v[3]);
	}
}

void glColor3fv(float *v)
{
	if (v!=0)
	{
		glColor3f(v[0],v[1],v[2]);
	}
}
