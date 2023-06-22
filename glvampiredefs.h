#ifndef GLVAMPIREDEFS_H
#define GLVAMPIREDEFS_H

#define glGetError() GLGetError(&vampContext);
#define gluOpenDisplay(BPP,maggieMode,width,height) GLUOpenDisplay(&vampContext,BPP,maggieMode,width,height);
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
#define GenerateGLError(type,msg) GLGenerateError(vampContext,type,msg);
#define glDebugMessageCallback(callback,userparam) GLDebugMessageCallback(&vampContext,callback,userparam);

#endif