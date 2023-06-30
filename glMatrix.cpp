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

extern "C" void GLMultMatrixf(struct GLVampContext* vampContext, const GLfloat* matrix)
{
    mat4 multMatrix;
    memcpy(&multMatrix, matrix, sizeof(mat4));

    mat4 *currentMatrix = vampContext->currentMatrix;
    if (currentMatrix != NULL)
    {
        // Multiply the current matrix by the new matrix
        mat4_mul(currentMatrix, &multMatrix, currentMatrix);
    }
}

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
    mat4 rotationMatrix;
    mat4_rotateX(&rotationMatrix, x * angle);
    
    mat4 yRot;
    mat4_rotateY(&yRot, y * angle);
    mat4_mul(&rotationMatrix, &rotationMatrix, &yRot);
    
    mat4 zRot;
    mat4_rotateZ(&zRot, z * angle);
    mat4_mul(&rotationMatrix, &rotationMatrix, &zRot);

    mat4_mul(vampContext->currentMatrix, vampContext->currentMatrix, &rotationMatrix);
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
    mat4 translationMatrix;
    mat4_translate(&translationMatrix, x, y, z);
    
    mat4_mul(vampContext->currentMatrix, vampContext->currentMatrix, &translationMatrix);
}



