#ifndef GLVAMPIREDEFS_H
#define GLVAMPIREDEFS_H

#include <utility/tagitem.h>

#ifdef __cplusplus
extern "C" {
#endif
void GLMatrixMode(struct GLVampContext* vampContext, GLenum mode);
void GLLoadIdentity(struct GLVampContext* vampContext);
void GLLoadMatrix(struct GLVampContext* vampContext, const GLfloat* matrix);
void GLRotatef(struct GLVampContext* vampContext, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void GLFrustum(struct GLVampContext* vampContext, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearVal, GLdouble farVal);
void GLPushMatrix(struct GLVampContext* vampContext);
void GLPopMatrix(struct GLVampContext* vampContext);
void GLTranslatef(struct GLVampContext* vampContext, GLfloat x, GLfloat y, GLfloat z);

void GLClearColor(struct GLVampContext *vampContext, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void GLClear(struct GLVampContext *vampContext, GLbitfield mask);
void GLColor4f(struct GLVampContext *vampContext, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void GLColor4ub(struct GLVampContext *vampContext, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void GLColor3f(struct GLVampContext *vampContext, GLfloat red, GLfloat green, GLfloat blue);
void GLColor4ubv(struct GLVampContext *vampContext, GLubyte *color);
void GLColor3fv(struct GLVampContext *vampContext, GLfloat *color);
void GLColor4fv(struct GLVampContext *vampContext, GLfloat *color);

void GLEnable(struct GLVampContext* vampContext, GLenum cap);
void GLDisable(struct GLVampContext* vampContext, GLenum cap);

void GLVertex2f(struct GLVampContext *vampContext, GLfloat x, GLfloat y);
void GLVertex3f(struct GLVampContext *vampContext, GLfloat x, GLfloat y, GLfloat z);
void GLTexCoord2f(struct GLVampContext *vampContext, GLfloat s, GLfloat t);
void GLNormal3f(struct GLVampContext *vampContext, GLfloat x, GLfloat y, GLfloat z);
void GLVertex3fv(struct GLVampContext *vampContext, const GLfloat *vec);
void GLBindTexture(struct GLVampContext *vampContext, GLenum target, GLuint texture);
void GLTexImage2D(struct GLVampContext *vampContext, GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, void *pixels);
void GLDeleteTextures(struct GLVampContext *vampContext, GLsizei num, GLuint *textures);
void GLTexSubImage2D(struct GLVampContext *vampContext, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
void GLBlendFunc(struct GLVampContext* vampContext, GLenum sfactor, GLenum dfactor);
void GLBlendFuncSeparate(struct GLVampContext* vampContext, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void GLAlphaFunc(struct GLVampContext* vampContext, GLenum func, GLfloat ref);
void GLBlendColor(struct GLVampContext* vampContext, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void GLBlendEquation(struct GLVampContext* vampContext, GLenum mode);
void GLTexEnvi(struct GLVampContext* vampContext, GLenum target, GLenum pname, GLint param);
void GLTexEnvf(struct GLVampContext* vampContext, GLenum target, GLenum pname, GLfloat param);
void GLTexEnvfv(GLVampContext* vampContext, GLenum target, GLenum pname, const GLfloat* params);

void GLGenerateError(struct GLVampContext *vampContext, int type, const char* message);
int GLUOpenDisplayTags(struct GLVampContext *vampContext, struct TagItem *tags);
void GLUCloseDisplay(struct GLVampContext *vampContext);
void GLUBeginFrame(struct GLVampContext *vampContext);
void GLUEndFrame(struct GLVampContext *vampContext);
void GLDebugMessageCallback(struct GLVampContext *vampContext, DebugMessageCallbackFunc callback, const void* userParam);
int GLGetError(struct GLVampContext *vampContext);
void GLViewport(struct GLVampContext *vampContext, int x, int y, int width, int height);
void GLBegin(struct GLVampContext *vampContext, int mode);
void GLEnd(struct GLVampContext *vampContext);
void GLDepthFunc(struct GLVampContext *vampContext, int i);
void GLDepthMask(struct GLVampContext *vampContext, int i);
void GLDrawBuffer(struct GLVampContext *vampContext, int i);
void GLPolygonMode(struct GLVampContext *vampContext, int i, int j);
void GLTexGeni(struct GLVampContext *vampContext, int i, int j, int k);
void GLTexParameteri(struct GLVampContext *vampContext, int i, int j, int k);
void GLFogi(struct GLVampContext* vampContext, int pname, int param);
void GLFogf(struct GLVampContext* vampContext, int pname, float param);
void GLFogfv(struct GLVampContext* vampContext, int i, float* col);
void GLReadPixels(struct GLVampContext *vampContext, int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
const char* GLGetString(struct GLVampContext *vampContext, unsigned int name);
#ifdef __cplusplus
}
#endif

#define VAMPOD_BPP TAG_USER+100
#define VAMPOD_MODE TAG_USER+101
#define VAMPOD_WIDTH TAG_USER+102
#define VAMPOD_HEIGHT TAG_USER+103
#define VAMPOD_WINDOW TAG_USER+104
#define VAMPOD_WINHANDLE TAG_USER+105
#define VAMPOD_USEINT TAG_USER+106
#define VAMPOD_WINDOWTITLE TAG_USER+107

#define glGetError() GLGetError(&vampContext);
#define gluOpenDisplayTags(tags) GLUOpenDisplayTags(&vampContext,tags);
#define gluCloseDisplay() GLUCloseDisplay(&vampContext);
#define glViewport(x,y,width,height) GLViewport(&vampContext,x,y,width,height);
#define glBegin(mode) GLBegin(&vampContext,mode);
#define glEnd() GLEnd(&vampContext);
#define glVertex3f(x,y,z) GLVertex3f(&vampContext,x,y,z);
#define glVertex2f(x,y) GLVertex2f(&vampContext,x,y);
#define glNormal3f(x,y,z) GLNormal3f(&vampContext,x,y,z);
#define glVertex3fv(vec) GLVertex3fv(&vampContext,vec);
#define glCullFace(i) GLCullFace(&vampContext,i);
#define glDepthFunc(i) GLDepthFunc(&vampContext,i);
#define glDepthMask(i) GLDepthMask(&vampContext,i);
#define glDrawBuffer(i) GLDrawBuffer(&vampContext,i);
#define gluBeginFrame() GLUBeginFrame(&vampContext);
#define gluEndFrame() GLUEndFrame(&vampContext);
#define glPolygonMode(i,j) GLPolygonMode(&vampContext,i,j);
#define glClearColor(i,j,k,l) GLClearColor(&vampContext,i,j,k,l);
#define glClear(i) GLClear(&vampContext,i);
#define glMatrixMode(mode) GLMatrixMode(&vampContext,mode);
#define glLoadIdentity() GLLoadIdentity(&vampContext);
#define glLoadMatrix(matrix) GLLoadMatrix(&vampContext,matrix);
#define glGetFloatv(pname,params) GLGetFloatv(&vampContext,pname,params);
#define glOrtho(left,right,bottom,top,nearVal,farVal) GLOrtho(&vampContext,left,right,bottom,top,nearVal,farVal);
#define glRotatef(angle,x,y,z) GLRotatef(&vampContext,angle,x,y,z);
#define glFrustum(left,right,bottom,top,nearVal,farVal) GLFrustum(&vampContext,left,right,bottom,top,nearVal,farVal);
#define glPushMatrix() GLPushMatrix(&vampContext);
#define glPopMatrix() GLPopMatrix(&vampContext);
#define glTranslatef(x,y,z) GLTranslatef(&vampContext,x,y,z);
#define glTexCoord2f(x,y) GLTexCoord2f(&vampContext,x,y);
#define glBindTexture(i,j) GLBindTexture(&vampContext,i,j);
#define glTexImage2D(i,j,k,l,m,n,o,p,pixels) GLTexImage2D(&vampContext,i,j,k,l,m,n,o,p,pixels);
#define glDeleteTextures(num,v) GLDeleteTextures(&vampContext,num,v);
#define glColor4f(r,g,b,a) GLColor4f(&vampContext,r,g,b,a);
#define glColor3f(x,y,z) GLColor3f(&vampContext,x,y,z);
#define glColor4ub(i,j,k,l) GLColor4ub(&vampContext,i,j,k,l);
#define glColor4ubv(col) GLColor4ubv(&vampContext,col);
#define glColor4fv(v) GLColor4fv(&vampContext,v);
#define glColor3fv(v) GLColor3fv(&vampContext,v);
#define glTexGeni(i,j,k) GLTexGeni(&vampContext,i,j,k);
#define glTexParameteri(i,j,k) GLTexParameteri(&vampContext,i,j,k);
#define glGetString(name) GLGetString(&vampContext,name);
#define glReadPixels(x,y,width,height,format,type,pixels) GLReadPixels(&vampContext,x,y,width,height,format,type,pixels);
#define glFogi(pname,param) GLFogi(&vampContext,pname,param);
#define glFogf(pname,param) GLFogf(&vampContext,pname,param);
#define glFogfv(i,col) GLFogfv(&vampContext,i,col);
#define glTexEnvi(i,j,k) GLTexEnvi(&vampContext,i,j,k);
#define glTexEnvf(i,j,k) GLTexEnvf(&vampContext,i,j,k);
#define glTexEnvfv(i,j,k) GLTexEnvfv(&vampContext,i,j,k);
#define glBlendFunc(i,j) GLBlendFunc(&vampContext,i,j);
#define glBlendFuncSeparate(i,j,k,l) GLBlendFuncSeparate(&vampContext,i,j,k,l);
#define glAlphaFunc(i,j) GLAlphaFunc(&vampContext,i,j);
#define glBlendColor(r,g,b,a) GLBlendColor(&vampContext,r,g,b,a);
#define glBlendEquation(mode) GLBlendEquation(&vampContext,mode);
#define GenerateGLError(type,msg) GLGenerateError(vampContext,type,msg);
#define glDebugMessageCallback(callback,userparam) GLDebugMessageCallback(&vampContext,callback,userparam);

extern struct GLVampContext vampContext;

#endif