#ifndef GLVAMPIREDEFS_H
#define GLVAMPIREDEFS_H

#include <utility/tagitem.h>

#ifdef __cplusplus
extern "C" {
#endif
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
void GLVertex3f(struct GLVampContext *vampContext, float x, float y, float z);
void GLVertex2f(struct GLVampContext *vampContext, float x, float y);
void GLNormal3f(struct GLVampContext *vampContext, float x, float y, float z);
void GLVertex3fv(struct GLVampContext *vampContext, float *vec);
void GLCullFace(struct GLVampContext *vampContext,int i);
void GLDepthFunc(struct GLVampContext *vampContext, int i);
void GLDepthMask(struct GLVampContext *vampContext, int i);
void GLDrawBuffer(struct GLVampContext *vampContext, int i);
void GLPolygonMode(struct GLVampContext *vampContext, int i, int j);
void GLClearColor(struct GLVampContext *vampContext, float i, float j, float k, float l);
void GLClear(struct GLVampContext *vampContext, unsigned int i);
void GLMatrixMode(struct GLVampContext *vampContext, int mode);
void GLLoadIdentity(struct GLVampContext *vampContext);
void GLLoadMatrix(struct GLVampContext *vampContext, float *matrix);
void GLGetFloatv(struct GLVampContext *vampContext, int pname, float* params);
void GLOrtho(struct GLVampContext *vampContext, float left, float right, float bottom, float top, float nearVal, float farVal);
void GLRotatef(struct GLVampContext *vampContext, float angle, float x, float y, float z);
void GLFrustum(struct GLVampContext *vampContext, float left, float right, float bottom, float top, float nearVal, float farVal);
void GLPushMatrix(struct GLVampContext *vampContext);
void GLPopMatrix(struct GLVampContext *vampContext);
void GLTranslatef(struct GLVampContext *vampContext,float x, float y, float z);
void GLTexCoord2f(struct GLVampContext *vampContext, float x, float y);
void GLBindTexture(struct GLVampContext *vampContext, int i, int j);
void GLTexImage2D(struct GLVampContext *vampContext, int i, int j, int k, int l, int m, int n, int o, int p, void *pixels);
void GLDeleteTextures(struct GLVampContext *vampContext, int num, void *v);
void GLColor4f(struct GLVampContext *vampContext, float r, float g, float b, float a);
void GLColor3f(struct GLVampContext *vampContext, float x, float y, float z);
void GLColor4ub(struct GLVampContext *vampContext, int i, int j, int k, int l);
void GLColor4ubv(struct GLVampContext *vampContext, unsigned char *col);
void GLColor4fv(struct GLVampContext *vampContext, float *v);
void GLColor3fv(struct GLVampContext *vampContext, float *v);
void GLTexGeni(struct GLVampContext *vampContext, int i, int j, int k);
void GLTexParameteri(struct GLVampContext *vampContext, int i, int j, int k);
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
#define GenerateGLError(type,msg) GLGenerateError(vampContext,type,msg);
#define glDebugMessageCallback(callback,userparam) GLDebugMessageCallback(&vampContext,callback,userparam);

extern struct GLVampContext vampContext;

#endif