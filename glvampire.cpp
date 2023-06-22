#include "glvampire.h"
#include "glvampiredefs.h"

#include <proto/exec.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

void GLDebugMessageCallback(struct GLVampContext *vampContext, DebugMessageCallbackFunc callback, const void* userParam)
{
    vampContext->glDebugMessage = callback;
}

void GLGenerateError(GLVampContext *vampContext, int type, const char* message)
{
    if (vampContext->glDebugMessage != 0)
    {
        vampContext->glDebugMessage(GL_DEBUG_SOURCE_API, type, 0, GL_DEBUG_SEVERITY_HIGH, strlen(message), message, nullptr);
    }
}

extern void magClearColor(unsigned int l);  // To make it compile, will be removed once added to the maggie.library

int GLGetError(struct GLVampContext *vampContext)
{
	return vampContext->glError;
}

void GLViewport(struct GLVampContext *vampContext, int x, int y, int width, int height)
{
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

void GLBegin(struct GLVampContext *vampContext, int mode) 
{
	char error[1024];
	
    switch (mode) {
        case GL_QUADS:
        case GL_POLYGON:
        case GL_TRIANGLE_FAN:
        case GL_LINE_STRIP:
        case GL_LINES:
        case GL_TRIANGLE_STRIP:
            vampContext->currentMode = static_cast<int>(mode);
            vampContext->vertices.clear();
            break;
        default:
			vampContext->glError = GL_INVALID_ENUM;
			sprintf(error,"Unknown mode for glBegin: %d\n",mode);
			GenerateGLError(GL_INVALID_ENUM, error);
            break;
    }
	magBegin();
}

void GLEnd(struct GLVampContext *vampContext) 
{	
    switch (vampContext->currentMode) {
        case GL_QUADS:
            if (vampContext->vertices.size() % 4 != 0) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(GL_INVALID_VALUE,"Number of vertices for GL_QUADS not divisible by 4\n");
                break;
            }
            {
                std::vector<unsigned short> indices(vampContext->vertices.size());
                for (size_t i = 0; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedPolygonsUP(&vampContext->vertices[0], static_cast<unsigned short>(vampContext->vertices.size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
			break;
        case GL_POLYGON:
            if (vampContext->vertices.size() < 3) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for polygon\n");
                break;
            }
            {
                std::vector<unsigned short> indices(vampContext->vertices.size());
                for (size_t i = 0; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedPolygonsUP(&vampContext->vertices[0], static_cast<unsigned short>(vampContext->vertices.size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
            break;
        case GL_TRIANGLE_FAN:
            if (vampContext->vertices.size() < 3) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for GL_TRIANGLE_FAN\n");
                break;
            }
            {
                std::vector<unsigned short> indices(vampContext->vertices.size());
                indices[0] = 0;
                for (size_t i = 1; i < indices.size(); i++) {
                    indices[i] = static_cast<unsigned short>(i);
                }
                magDrawIndexedTrianglesUP(&vampContext->vertices[0], static_cast<unsigned short>(vampContext->vertices.size()), &indices[0], static_cast<unsigned short>(indices.size()));
            }
            break;
        case GL_LINE_STRIP:
            if (vampContext->vertices.size() < 2) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for GL_LINE_STRIP\n");
                break;
            }
            {
                struct SpanPosition start, end;
                // Initialisierung der Start- und Endposition entsprechend der Vertices
                start.u = vampContext->vertices[0].pos.x;
                start.v = vampContext->vertices[0].pos.y;
                end.u = vampContext->vertices[vampContext->vertices.size() - 1].pos.x;
                end.v = vampContext->vertices[vampContext->vertices.size() - 1].pos.y;
                magDrawLinearSpan(&start, &end);
            }
            break;
        case GL_LINES:
#if 0		
            if (vampContext->vertices.size() % 2 != 0) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Number of Vertices for GL_LINES not divisible by 2\n");
                break;
            }
            {
                struct MaggieClippedVertex start, end;
                // Initialisierung des Start- und Endvertices entsprechend den Vertices
                start.pos = vampContext->vertices[0].pos;
                end.pos = vampContext->vertices[vampContext->vertices.size() - 1].pos;
                magDrawSpan(&start, &end);
            }
#else
            if (vampContext->vertices.size() % 2 != 0) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Number of Vertices for GL_LINES not divisible by 2\n");
                break;
            }
            {
                for (size_t i = 0; i < vampContext->vertices.size(); i += 2) {
                    struct SpanPosition start, end;
                    // Initialisierung der Start- und Endposition entsprechend der Vertices
                    start.u = vampContext->vertices[i].pos.x;
                    start.v = vampContext->vertices[i].pos.y;
                    end.u = vampContext->vertices[i + 1].pos.x;
                    end.v = vampContext->vertices[i + 1].pos.y;
                    magDrawLinearSpan(&start, &end);
                }
            }
            break;	
#endif			
            break;
        case GL_TRIANGLE_STRIP:
            if (vampContext->vertices.size() < 3) {
				vampContext->glError = GL_INVALID_VALUE;
				GenerateGLError(vampContext->glError,"Not enough vertices for GL_TRIANGLE_STRIP\n");
                break;
            }
            {
                struct SpanPosition start, end;
                // Initialisierung der Start- und Endposition entsprechend der Vertices
                start.u = vampContext->vertices[0].pos.x;
                start.v = vampContext->vertices[0].pos.y;
                end.u = vampContext->vertices[vampContext->vertices.size() - 1].pos.x;
                end.v = vampContext->vertices[vampContext->vertices.size() - 1].pos.y;
                magDrawLinearSpan(&start, &end);
            }
            break;
        default:
            break;
    }
	magEnd();
}

void GLVertex3f(struct GLVampContext *vampContext, float x, float y, float z)
{
    MaggieVertex vertex;
    vertex.pos = {x, y, z};
    vampContext->vertices.push_back(vertex);	
	magVertex(x,y,z);
}

void GLVertex2f(struct GLVampContext *vampContext, float x, float y)
{
    MaggieVertex vertex;
    vertex.pos = {x, y, 0.0f};
    vampContext->vertices.push_back(vertex);	
	magVertex(x,y,0.0f);
}

void GLNormal3f(struct GLVampContext *vampContext, float x, float y, float z)
{
    if (!vampContext->vertices.empty()) {
        MaggieVertex& currentVertex = vampContext->vertices.back();
        currentVertex.normal = {x, y, z};
    }	
	else
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,"glNormal3f was called before glVertex was called\n");
	}
	magNormal(x,y,z);
}

void GLVertex3fv(struct GLVampContext *vampContext, float *vec)
{
	if (vec!=0)
	{
		MaggieVertex vertex;
		vertex.pos = {vec[0],vec[1],vec[2]};
		vampContext->vertices.push_back(vertex);		
		magVertex(vec[0],vec[1],vec[2]);
	}
	else
	{
		vampContext->glError = GL_INVALID_VALUE;
		GenerateGLError(vampContext->glError,"Vector for glVertex3fv was empty\n");
	}
}

void GLCullFace(struct GLVampContext *vampContext,int i)
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

void GLDepthFunc(struct GLVampContext *vampContext, int i)
{
	if (i!=GL_LEQUAL)
	{
		vampContext->glError = GL_INVALID_ENUM;
		GenerateGLError(vampContext->glError,"Maggie Chip only support GL_LEQUAL for glDepthFunc\n");
	}
}

void GLDepthMask(struct GLVampContext *vampContext, int i)
{
	// Not really correct implementation, DepthMask should only affect the writing, which currently is not possible on the Maggie Chipset
	
	if (i==GL_TRUE)
	{
		vampContext->vampDrawModes|= MAG_DRAWMODE_DEPTHBUFFER;
	}
	else
	{
		vampContext->vampDrawModes &= ~MAG_DRAWMODE_DEPTHBUFFER;
	}
}

void GLDrawBuffer(struct GLVampContext *vampContext, int i)
{
}

void GLUBeginFrame(struct GLVampContext *vampContext)
{
	volatile ULONG *SAGA_ChunkyData = (ULONG *)0xdff1ec;
	
	int i;
	float matrix[16];
	if (vampContext->vampScreenPixels[vampContext->vampCurrentBuffer]==0) 
	{
		// glViewport was not yet called !!!
		return;
	}
	magBeginScene();
	*SAGA_ChunkyData = (ULONG)vampContext->vampScreenPixels[vampContext->vampCurrentBuffer];
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

void GLUEndFrame(struct GLVampContext *vampContext)
{
	magEndScene();
}

void GLPolygonMode(struct GLVampContext *vampContext, int i, int j)
{
}

void GLClearColor(struct GLVampContext *vampContext, float i, float j, float k, float l)
{
	unsigned int rgb;
	
	unsigned char a = (unsigned char)(255*l);
	unsigned char r = (unsigned char)(255*i);
	unsigned char g = (unsigned char)(255*j);
	unsigned char b = (unsigned char)(255*k);
	rgb = (a<<24)|(r<<16)|(g<<8)|(b);
	magClearColor(rgb);
}

void GLClear(struct GLVampContext *vampContext, unsigned int i)
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

void GLMatrixMode(struct GLVampContext *vampContext, int mode)
{
    if (mode == GL_MODELVIEW)
    {
        vampContext->currentMatrix = &vampContext->modelViewMatrix;
    }
    else if (mode == GL_PROJECTION)
    {
        vampContext->currentMatrix = &vampContext->projectionMatrix;
    }
	else
	{
		vampContext->glError = GL_INVALID_ENUM;
		GenerateGLError(vampContext->glError,"glMatrixMode called with invalid parameter, only GL_MODELVIEW and GL_PROJECTION are valid\n");	
	}
}

void GLLoadIdentity(struct GLVampContext *vampContext)
{
	mat4_identity(vampContext->currentMatrix);
}

void GLLoadMatrix(struct GLVampContext *vampContext, float *matrix) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            vampContext->currentMatrix->m[i][j] = matrix[i * 4 + j];
        }
    }
}

void GLGetFloatv(struct GLVampContext *vampContext, int pname, float* params)
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

void GLOrtho(struct GLVampContext *vampContext, float left, float right, float bottom, float top, float nearVal, float farVal) 
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


void GLRotatef(struct GLVampContext *vampContext, float angle, float x, float y, float z) 
{
    GLLoadIdentity(vampContext);

    mat4_rotateX(vampContext->currentMatrix, x * angle);
    mat4_rotateY(vampContext->currentMatrix, y * angle);
    mat4_rotateZ(vampContext->currentMatrix, z * angle);
}

void GLFrustum(struct GLVampContext *vampContext, float left, float right, float bottom, float top, float nearVal, float farVal) 
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

void GLPushMatrix(struct GLVampContext *vampContext)
{
    vampContext->matrixStack.push(vampContext->currentMatrix);
}

void GLPopMatrix(struct GLVampContext *vampContext)
{
    if (!vampContext->matrixStack.empty())
    {
        vampContext->currentMatrix = vampContext->matrixStack.top();
        vampContext->matrixStack.pop();
    }
	else 
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,"Matrix Stack was empty on call of glPopMatrix\n");		
	}
}

void GLTranslatef(struct GLVampContext *vampContext,float x, float y, float z)
{
	mat4_translate(vampContext->currentMatrix, x, y, z);
}

void GLTexCoord2f(struct GLVampContext *vampContext, float x, float y)
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

void GLBindTexture(struct GLVampContext *vampContext, int i, int j)
{
	if (i==GL_TEXTURE_2D)
	{
		auto num = vampContext->vampTextureMap->find(j);
		if (num!=vampContext->vampTextureMap->end())
		{
			vampContext->currentTexture = num->second;
			magSetTexture(0,vampContext->currentTexture);
		}
		else 
		{
			char error[1024];
			
			sprintf(error,"No Texture %d was found on glBindTexture\n",i);
			vampContext->glError = GL_INVALID_OPERATION;
			GenerateGLError(vampContext->glError,error);			
		}
	}
}

void GLTexImage2D(struct GLVampContext *vampContext, int i, int j, int k, int l, int m, int n, int o, int p, void *pixels)
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
		if (!texHandle) 
		{
			char error[1024];
			
			sprintf(error,"Could not allocate Texture in call to glexImage2D(%d,%d,%d,%d,%d,%d,%d,%d,ptr)",i,j,k,l,m,n,o,p);
			vampContext->glError = GL_OUT_OF_MEMORY;
			GenerateGLError(vampContext->glError,error);	
	
			return;
		}
		vampContext->vampTextureMap->insert(std::make_pair(vampContext->maxVampTex, texHandle));
		vampContext->maxVampTex++;
		magUploadTexture(texHandle, j, pixels, 0);
	}
}

void GLDeleteTextures(struct GLVampContext *vampContext, int num, void *v)
{
	if (num==1)
	{
		int texnum = *((int*)(v));
		
		if (texnum!=0)
		{
			auto it = vampContext->vampTextureMap->find(texnum);
			if (it != vampContext->vampTextureMap->end()) 
			{
				int number = it->second;
				vampContext->vampTextureMap->erase(it);
				magFreeTexture(number);
			}
			else 
			{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"Could not find texture %d\n",texnum);
				GenerateGLError(vampContext->glError,error);					
			}
		}
	}
	else 
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,"glDeleteTextures currently only supports if first parameter is 1\n");
	}
}

void GLColor4f(struct GLVampContext *vampContext, float r, float g, float b, float a)
{
	unsigned char rdec = (unsigned char)r;
	unsigned char gdec = (unsigned char)g;
	unsigned char bdec = (unsigned char)b;
	unsigned char adec = (unsigned char)a;
	
	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);
}

void GLColor3f(struct GLVampContext *vampContext, float x, float y, float z)
{
	unsigned char rdec = (unsigned char)x;
	unsigned char gdec = (unsigned char)y;
	unsigned char bdec = (unsigned char)z;
	unsigned char adec = 1;
	
	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);	
}

void GLColor4ub(struct GLVampContext *vampContext, int i, int j, int k, int l)
{
	unsigned char rdec = (unsigned char)i;
	unsigned char gdec = (unsigned char)j;
	unsigned char bdec = (unsigned char)k;
	unsigned char adec = (unsigned char)l;

	int color = (rdec<<16)|(gdec<<8)|(bdec)|(adec<<24);
	
	magColour(color);	
}

void GLColor4ubv(struct GLVampContext *vampContext, unsigned char *col)
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

void GLColor4fv(struct GLVampContext *vampContext, float *v)
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

void GLColor3fv(struct GLVampContext *vampContext, float *v)
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
