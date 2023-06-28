#include "glvampire.h"
#include "glvampiredefs.h"

#include <map>
#include <stack>
#include <vector>

#include <proto/exec.h>
#include <proto/Maggie.h>
#include <maggie_vec.h>
#include <maggie_flags.h>
#include <maggie_vertex.h>

extern struct Library *MaggieBase;

extern "C" void GLColor4f(struct GLVampContext *vampContext, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    GLubyte rdec = (GLubyte)(r * 255.0f);
    GLubyte gdec = (GLubyte)(g * 255.0f);
    GLubyte bdec = (GLubyte)(b * 255.0f);
    GLubyte adec = (GLubyte)(a * 255.0f);

    GLuint color = (rdec << 16) | (gdec << 8) | (bdec) | (adec << 24);

    if (vampContext->manualDraw)
    {
        std::vector<MaggieVertex> *vertices = (std::vector<MaggieVertex> *)vampContext->vertices;

        if (!vertices->empty())
        {
            MaggieVertex &currentVertex = vertices->back();
            currentVertex.colour = color;
        }
        else
        {
            vampContext->glError = GL_INVALID_OPERATION;
            GenerateGLError(vampContext->glError, "glColor4f was called before glVertex was called\n");
        }
    }
    else
    {
        magColour(color);
    }
}

extern "C" void GLColor3f(struct GLVampContext *vampContext, GLfloat r, GLfloat g, GLfloat b)
{
    GLubyte rdec = (GLubyte)(r * 255.0f);
    GLubyte gdec = (GLubyte)(g * 255.0f);
    GLubyte bdec = (GLubyte)(b * 255.0f);
    GLubyte adec = 1;

    GLuint color = (rdec << 16) | (gdec << 8) | (bdec) | (adec << 24);

    if (vampContext->manualDraw)
    {
        std::vector<MaggieVertex> *vertices = (std::vector<MaggieVertex> *)vampContext->vertices;

        if (!vertices->empty())
        {
            MaggieVertex &currentVertex = vertices->back();
            currentVertex.colour = color;
        }
        else
        {
            vampContext->glError = GL_INVALID_OPERATION;
            GenerateGLError(vampContext->glError, "glColor3f was called before glVertex was called\n");
        }
    }
    else
    {
        magColour(color);
    }
}

extern "C" void GLColor4ub(struct GLVampContext *vampContext, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    GLuint color = (r << 16) | (g << 8) | (b) | (a << 24);

    if (vampContext->manualDraw)
    {
        std::vector<MaggieVertex> *vertices = (std::vector<MaggieVertex> *)vampContext->vertices;

        if (!vertices->empty())
        {
            MaggieVertex &currentVertex = vertices->back();
            currentVertex.colour = color;
        }
        else
        {
            vampContext->glError = GL_INVALID_OPERATION;
            GenerateGLError(vampContext->glError, "glColor4ub was called before glVertex was called\n");
        }
    }
    else
    {
        magColour(color);
    }
}

extern "C" void GLColor4ubv(struct GLVampContext *vampContext, GLubyte *col)
{
    if (col != nullptr)
    {
        GLColor4ub(vampContext, col[0], col[1], col[2], col[3]);
    }
    else
    {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(vampContext->glError, "Vector for glColor4ubv is null\n");
    }
}

extern "C" void GLColor4fv(struct GLVampContext *vampContext, GLfloat *v)
{
    if (v != nullptr)
    {
        GLColor4f(vampContext, v[0], v[1], v[2], v[3]);
    }
    else
    {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(vampContext->glError, "Vector for glColor4fv is null\n");
    }
}

extern "C" void GLColor3fv(struct GLVampContext *vampContext, GLfloat *v)
{
    if (v != nullptr)
    {
        GLColor3f(vampContext, v[0], v[1], v[2]);
    }
    else
    {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(vampContext->glError, "Vector for glColor3fv is null\n");
    }
}

extern "C" void GLClearColor(__attribute__((unused)) struct GLVampContext *vampContext, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    GLuint rgb;

    GLubyte aDec = (GLubyte)(255 * a);
    GLubyte rDec = (GLubyte)(255 * r);
    GLubyte gDec = (GLubyte)(255 * g);
    GLubyte bDec = (GLubyte)(255 * b);

    rgb = (aDec << 24) | (rDec << 16) | (gDec << 8) | (bDec);
    magClearColour(rgb);
}

extern "C" void GLClear(struct GLVampContext *vampContext, GLuint mask)
{
    GLushort clearMode = 0;
    if (mask & GL_COLOR_BUFFER_BIT)
        clearMode |= MAG_CLEAR_COLOUR;
    if (mask & GL_DEPTH_BUFFER_BIT)
        clearMode |= MAG_CLEAR_DEPTH;
    if (clearMode > 0)
        magClear(clearMode);
    else
    {
        vampContext->glError = GL_INVALID_ENUM;
        GenerateGLError(vampContext->glError, "Maggie Chip only supports GL_LEQUAL for glDepthFunc\n");
    }
}
