#ifndef GL_VAMPIRE_H
#define GL_VAMPIRE_H

#include <proto/Maggie.h>

typedef enum {
    GL_MODULATE=11,
    GL_REPLACE
} GLenum;

// Matrix Manipulation

void glPushMatrix(); // DONE
void glPopMatrix(); // DONE
void glLoadMatrixf(float *f); // DONE
void glScissor(int x, int y, int w, int h);
void glViewport(int x, int y, int w, int h);
void glMatrixMode(int i); // DONE
void glTranslatef(float x, float y, float z); // DONE
void glRotatef(float i, float x, float y, float z); // DONE
void glFrustum(int xmin, int mmax, int ymin, int ymax, int znear, int zfar); // DONE
void glLoadIdentity(); // DONE
void glOrtho(int i,int w, int h, int j, int k, int l); // DONE
void glGetFloatv(int i, float *w); // Only used to acquire the World Matrix, DONE with this restriction

// Colors

void glClear(unsigned int i); // DONE
void glClearColor(float i, float j, float k, float l); // DONE
void glColor4f(float r, float g, float b, float a); // DONE
void glColor4ub(int i, int j, int k, int l); // DONE
void glColor3f(float x, float y, float z); // DONE
void glColor4ubv(unsigned char *col); // DONE
void glColor3fv(float *col); // DONE
void glColor4fv(float *v); // DONE


// Blending

void glBlendFunc(int i, int j);
void glAlphaFunc(int i, float j);
void glShadeModel(int i); // switch between smooth and flat shading, not sure if a big problem if missing

// Textures and Vertices

void glVertex2f(float x, float y); // DONE
void glVertex3f(float x, float y, float z); // DONE
void glTexCoord2f(float x, float y); // DONE
void glNormal3f(float x, float y, float z); // DONE
void glVertex3fv(float *vec); // DONE
void glBindTexture(int i, int j); // DONE
void glTexImage2D(int i, int j, int k, int l, int m, int n, int o, int p, void *pixels); 
void glDeleteTextures(int num, void *v); // DONE
void glTexSubImage2D(int i, int j, int xoff, int yoff, int w, int h, int form, int type, void *pixels);

// Stuff which is probably only used for Default settings and probably can be removed

void glDepthMask(int i); // DONE
void glCullFace(int i); // DONE (but not completely implemented)
void glDrawBuffer(int i); // DONE (does nothing)
void glPolygonMode(int i, int j); // DONE

// States

void glEnable(int i);
void glDisable(int i);

// Handling, a lot of it can be removed

int glGetError();
void glBegin(int i);
void glEnd();

// glTexGen (might not be needed, not sure)

void glTexGeni(int i, int j, int k);

// Z Buffer (only used to avoid models stuck in walls)

void glDepthRange(int i, int j);
void glDepthFunc(int i); // DONE

// TexEnv

void glTexEnvi(int i, int j, int k);
void glTexEnvf(int i, int j, int k);

// TexParameter

void glTexParameteri(int i, int j, int k);

// Optional (as in REALLY REALLY optional, can be removed)

void glFogi(int i, int j);
void glFogfv(int i, float *col);
void glReadPixels(int x, int y, int w, int h, int i, int j, void *k);
void glFogf(int i, float j);
char *glGetString(int i);


#define GL_NO_ERROR 0
#define GL_TEXTURE_2D 1
#define GL_SRC_ALPHA 2
#define GL_ONE_MINUS_SRC_ALPHA 3
#define GL_GREATER 4
#define GL_BLEND 5
#define GL_ALPHA_TEXT 6
#define GL_COLOR_BUFFER_BIT 7
#define GL_ALPHA_TEST 8
#define MGL_FLATFAN 9
#define MGL_PERSPECTIVE_MAPPING 10
#define GL_QUADS 13
#define GL_SMOOTH 14
#define GL_FLAT 15
#define GL_TRIANGLE_STRIP 16
#define GL_S 17
#define GL_T 18
#define GL_SPHERE_MAP 19
#define GL_TEXTURE_GEN_MODE 20
#define GL_TRIANGLE_FAN 21
#define GL_TEXTURE_GEN_S 22
#define GL_TEXTURE_GEN_T 23
#define GL_FOG 24
#define GL_RGBA 25
#define GL_UNSIGNED_BYTE 26
#define GL_TEXTURE_MIN_FILTER 27
#define GL_TEXTURE_MAG_FILTER 28
#define GL_NEAREST_MIPMAP_NEAREST 29
#define GL_LINEAR_MIPMAP_NEAREST 30
#define GL_NEAREST_MIPMAP_LINEAR 31
#define GL_LINEAR_MIPMAP_LINEAR 32
#define GL_TEXTURE_ENV 33
#define GL_TEXTURE_ENV_MODE 34
#define GL_NEAREST 35
#define GL_LINEAR 36
#define GL_ONE 37
#define GL_DEPTH_TEST 38
#define GL_CULL_FACE 39
#define GL_FRONT 40
#define GL_BACK 41
#define GL_PROJECTION 42
#define GL_MODELVIEW 43
#define GL_DECAL 44
#define GL_DITHER 45
#define GL_RENDERER 46
#define GL_VERSION 47
#define GL_EXTENSIONS 48
#define GL_UNPACK_ALIGNMENT 49
#define GL_PACK_ALIGNMENT 50
#define GL_FRONT_AND_BACK 51
#define GL_FILL 52
#define GL_TEXTURE_WRAP_S 53
#define GL_TEXTURE_WRAP_T 54
#define GL_REPEAT 55
#define GL_RGB 56
#define GL_POLYGON 57
#define GL_LINE_STRIP 58
#define GL_ZERO 59
#define GL_SRC_COLOR 60
#define GL_INTENSITY8 61
#define GL_LUMINANCE8 62
#define MGL_Z_OFFSET 63
#define GL_FOG_END 64
#define GL_FOG_START 65
#define GL_FOG_DENSITY 66
#define GL_VENDOR 67
#define GL_TRUE 68
#define GL_FALSE 69
#define GL_GEQUAL 70
#define GL_DEPTH_BUFFER_BIT 71
#define GL_LEQUAL 72
#define GL_FOG_COLOR 73
#define GL_FOG_MODE 74
#define GL_EXP 75
#define GL_EXP2 76
#define GL_MODELVIEW_MATRIX 77
#define GL_SCISSOR_TEST 78
#define GL_LINES 79

#define GLfloat float
#define GLint int
#define GLdouble double
#endif