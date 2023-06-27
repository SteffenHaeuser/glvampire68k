#ifndef GL_VAMPIRE_H
#define GL_VAMPIRE_H

#include <maggie_vec.h>
#include <intuition/intuition.h>

#define GLfloat float
#define GLint int
#define GLuint unsigned int
#define GLbitfield GLuint
#define GLubyte unsigned char
#define GLdouble double
#define GLdouble double
#define GLsizei size_t
#define GLenum unsigned int
#define GLushort unsigned short

// Matrix Manipulation

void glPushMatrix();
void glPopMatrix();
void glLoadMatrixf(const GLfloat *matrix);
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void glMatrixMode(GLenum mode);
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void glLoadIdentity();
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void glGetFloatv(GLenum pname, GLfloat *params);

// Colors

void glClear(GLbitfield mask);
void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void glColor3f(GLfloat red, GLfloat green, GLfloat blue);
void glColor4ubv(const GLubyte* color);
void glColor3fv(const GLfloat* color);
void glColor4fv(const GLfloat* color);

// Textures and Vertices

void glVertex2f(GLfloat x, GLfloat y);
void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void glTexCoord2f(GLfloat s, GLfloat t);
void glNormal3f(GLfloat x, GLfloat y, GLfloat z);
void glVertex3fv(const GLfloat *vec);
void glBindTexture(GLenum target, GLuint texture);
void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
void glDeleteTextures(GLsizei num, const GLuint *textures);
void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

// States

void glEnable(GLenum cap);
void glDisable(GLenum cap);

// Blending

void glBlendFunc(GLenum sfactor, GLenum dfactor);
void glAlphaFunc(GLenum func, GLfloat ref);
void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glBlendEquation(GLenum mode);

// TexEnv

void glTexEnvi(GLenum target, GLenum pname, GLint param);
void glTexEnvf(GLenum target, GLenum pname, GLfloat param);

// Stuff which is probably only used for Default settings and probably can be removed

void glDepthMask(int i); // DONE
void glCullFace(int i); // DONE (but not completely implemented)
void glDrawBuffer(int i); // DONE (does nothing)
void glPolygonMode(int i, int j); // DONE
void glShadeModel(int i); // switch between smooth and flat shading, not sure if a big problem if missing

// Handling, a lot of it can be removed

int glGetError(); // DONE
void glBegin(int i); // DONE
void glEnd(); // DONE

// glTexGen (might not be needed, not sure)

void glTexGeni(int i, int j, int k); // DONE (Partially)

// Z Buffer (only used to avoid models stuck in walls)

void glDepthRange(int i, int j);
void glDepthFunc(int i); // DONE

// TexParameter

void glTexParameteri(int i, int j, int k); // DONE (Partially)

// Optional (as in REALLY REALLY optional, can be removed)

void glFogi(int i, int j); // DONE
void glFogfv(int i, float *col); // DONE
void glReadPixels(int x, int y, int w, int h, int i, int j, void *k); // DONE
void glFogf(int i, float j);
char *glGetString(int i); // DONE


#define GL_NO_ERROR 0
#define GL_MODULATE 0x2100
#define GL_REPLACE 0x1E01
#define GL_TEXTURE_2D 0x0DE1
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_GREATER 0x0204
#define GL_BLEND 0x0BE2
#define GL_ALPHA_TEST 0x0BC0
#define MGL_FLATFAN 1
#define MGL_PERSPECTIVE_MAPPING 2
#define GL_QUADS 0x0007
#define GL_SMOOTH 0x1D01
#define GL_FLAT 0x1D00
#define GL_TRIANGLE_STRIP 0x0005
#define GL_S 0x2000
#define GL_T 0x2001
#define GL_SPHERE_MAP 0x2402
#define GL_TEXTURE_GEN_MODE 0x2500
#define GL_TRIANGLE_FAN 0x0006
#define GL_TEXTURE_GEN_S 0x0C60
#define GL_TEXTURE_GEN_T 0x0C61
#define GL_FOG 0x0B60
#define GL_RGBA 0x1908
#define GL_UNSIGNED_BYTE 0x1401
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_ONE 1
#define GL_DEPTH_TEST 0x0B71
#define GL_CULL_FACE 0x0B44
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_PROJECTION 0x1701
#define GL_MODELVIEW 0x1700
#define GL_DECAL 0x2101
#define GL_DITHER 0x0BD0
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_FRONT_AND_BACK 0x0408
#define GL_FILL 0x1B02
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_REPEAT 0x2901
#define GL_RGB 0x1907
#define GL_POLYGON 0x0009
#define GL_LINE_STRIP 0x0003
#define GL_ZERO 0
#define GL_SRC_COLOR 0x0300
#define GL_INTENSITY8 0x804B
#define GL_LUMINANCE8 0x8040
#define MGL_Z_OFFSET 3
#define GL_FOG_END 0x0B64
#define GL_FOG_START 0x0B63
#define GL_FOG_DENSITY 0x0B62
#define GL_VENDOR 0x1F00
#define GL_TRUE 1
#define GL_FALSE 0
#define GL_GEQUAL 0x0206
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_LEQUAL 0x0203
#define GL_FOG_COLOR 0x0B66
#define GL_FOG_MODE 0x0B65
#define GL_EXP 0x0800
#define GL_EXP2 0x0801
#define GL_MODELVIEW_MATRIX 0x0BA6
#define GL_SCISSOR_TEST 0x0C11
#define GL_LINES 0x0001
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_SOLID_FORMAT 0x0D4C
#define GL_DXT1 0x83F1
#define GL_NORMAL_MAP 0x8511
#define GL_UNSIGNED_SHORT 0x1403
#define GL_FLOAT 0x1406
#define GL_BGRA 0x80E1
#define GL_BGR 0x80E0
#define GL_FOG_COORD_SRC 0x8450
#define GL_ADD 0x0104
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_DST_COLOR 0x0306
#define GL_FUNC_ADD 0x8006
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_ALWAYS 0x0207
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_NOTEQUAL 0x0205
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_SRC1_COLOR 0x88F9
#define GL_SRC1_ALPHA 0x8589
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_COMBINE_TEXTURE 0x8575
#define GL_COMBINE_PREVIOUS 0x8578
#define GL_COMBINE_CONSTANT 0x8576
#define GL_COMBINE_RGB 0x8571
#define GL_COMBINE_ALPHA 0x8572
#define GL_TEXTURE_ENV_COLOR 0x2201
#define GL_RGB_SCALE 0x8573
#define GL_ALPHA_SCALE 0x0D1C

#define GL_INVALID_ENUM 0x500
#define GL_INVALID_VALUE 0x501
#define GL_INVALID_OPERATION 0x502
#define GL_STACK_OVERFLOW 0x503
#define GL_STACK_UNDERFLOW 0x504
#define GL_OUT_OF_MEMORY 0x505
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x506
#define GL_CONTEXT_LOST 0x507
#define GL_TABLE_TOO_LARGE 0x8031

typedef void (*DebugMessageCallbackFunc)(int source, int type, unsigned int id, int severity, int length, const char* message, const void* userParam);

enum FogMode {
    FOG_LINEAR,
    FOG_EXP,
    FOG_EXP2
};

enum FogCoordSrc {
	GL_FOG_COORD,
	GL_FRAGMENT_DEPTH
};

struct FogParams {
    enum FogMode mode;
	enum FogCoordSrc fogCoordSrc;
    float density;
    float start;
    float end;
    unsigned int color;
};

struct GLVampContext
{
	int vampBpp;
	int vampCurrentBuffer;
	int currentTexture;
	int vampDrawModes;
	mat4 modelViewMatrix;     
	mat4 projectionMatrix;  
	mat4 worldMatrix;
	mat4 *currentMatrix;
	int vampWidth;
	int vampHeight;
	int maxVampTex;
	UBYTE *screenMem;
	int screenMemSize;
	void *vampTextureMap;
	void *matrixStack; 
	UWORD *vampScreenPixels[3];	
	void *vertices;	
	int currentMode;	
	int maggieMode;	
	int oldMode;
	int oldScreen;
	int glError;
	int useInterrupt;
	void *interrupt;
	struct Window *window;
	int useWindow;
	int manualDraw;
	struct FogParams fogParams;
	int useFogging;
	int useFogCoordSrc;
	int texenv;
	ULONG blendColor;
	int blendFuncSrc;
	int blendFuncDest;
	int useBlending;
	int blendEquation;
	int separateBlendFuncEnabled;
	int blendFuncSrcRGB;
	int blendFuncDestRGB;
	int blendFuncSrcAlpha;
	int blendFuncDestAlpha;
	int alphaFunc;
	float alphaRef;
	int useAlphaFunc;
	unsigned int *buffer32;
	GLint combineRGB;     
    GLint combineAlpha;  
    GLfloat rgbScale;      
    GLfloat alphaScale;   
	ULONG texEnvColor;
	ULONG combineRGBConstant;
	DebugMessageCallbackFunc glDebugMessage;
};

#include "glvampiredefs.h"

#endif