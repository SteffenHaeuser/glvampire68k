#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>

#include <proto/exec.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

extern struct Library *MaggieBase;

extern "C" void GLMatrixMode(struct GLVampContext* vampContext, GLenum mode)
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
        GenerateGLError(vampContext->glError, "glMatrixMode called with invalid parameter, only GL_MODELVIEW and GL_PROJECTION are valid\n");	
    }
}

extern "C" void GLLoadIdentity(struct GLVampContext* vampContext)
{
    mat4_identity(vampContext->currentMatrix);
}

extern "C" void GLLoadMatrixf(struct GLVampContext* vampContext, const GLfloat* matrix)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            vampContext->currentMatrix->m[i][j] = matrix[i * 4 + j];
        }
    }
}

extern "C" void GLRotatef(struct GLVampContext* vampContext, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLLoadIdentity(vampContext);

    mat4_rotateX(vampContext->currentMatrix, x * angle);
    mat4_rotateY(vampContext->currentMatrix, y * angle);
    mat4_rotateZ(vampContext->currentMatrix, z * angle);
}

extern "C" void GLPushMatrix(struct GLVampContext* vampContext)
{
    std::stack<mat4*>* matrixStack = reinterpret_cast<std::stack<mat4*>*>(vampContext->matrixStack);
    matrixStack->push(vampContext->currentMatrix);
}

extern "C" void GLPopMatrix(struct GLVampContext* vampContext)
{
    std::stack<mat4*>* matrixStack = reinterpret_cast<std::stack<mat4*>*>(vampContext->matrixStack);
    if (!matrixStack->empty())
    {
        vampContext->currentMatrix = matrixStack->top();
        matrixStack->pop();
    }
    else 
    {
        vampContext->glError = GL_INVALID_OPERATION;
        GenerateGLError(vampContext->glError, "Matrix Stack was empty on call of glPopMatrix\n");		
    }
}

extern "C" void GLTranslatef(struct GLVampContext* vampContext, GLfloat x, GLfloat y, GLfloat z)
{
    mat4_translate(vampContext->currentMatrix, x, y, z);
}
