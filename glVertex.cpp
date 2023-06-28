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

extern "C" void GLVertex3f(struct GLVampContext *vampContext, GLfloat x, GLfloat y, GLfloat z)
{
    if (vampContext->manualDraw)
    {
        MaggieVertex vertex;
        std::vector<MaggieVertex> *vertices;

        vertices = reinterpret_cast<std::vector<MaggieVertex>*>(vampContext->vertices);

        vertex.pos = {x, y, z};
        vertices->push_back(vertex);
    }
    else
    {
        magVertex(x, y, z);
    }
}

extern "C" void GLVertex2f(struct GLVampContext *vampContext, GLfloat x, GLfloat y)
{
    if (vampContext->manualDraw)
    {
        MaggieVertex vertex;
        std::vector<MaggieVertex> *vertices;

        vertices = reinterpret_cast<std::vector<MaggieVertex>*>(vampContext->vertices);

        vertex.pos = {x, y, 0.0f};
        vertices->push_back(vertex);
    }
    else
    {
        magVertex(x, y, 0.0f);
    }
}

extern "C" void GLNormal3f(struct GLVampContext *vampContext, GLfloat x, GLfloat y, GLfloat z)
{
    if (vampContext->manualDraw)
    {
        std::vector<MaggieVertex> *vertices;

        vertices = reinterpret_cast<std::vector<MaggieVertex>*>(vampContext->vertices);

        if (!vertices->empty()) {
            MaggieVertex& currentVertex = vertices->back();
            currentVertex.normal = {x, y, z};
        }
        else
        {
            vampContext->glError = GL_INVALID_OPERATION;
            GenerateGLError(vampContext->glError, "glNormal3f was called before glVertex was called\n");
        }
    }
    else
    {
        magNormal(x, y, z);
    }
}

extern "C" void GLVertex3fv(struct GLVampContext *vampContext, const GLfloat *vec)
{
    if (vec == nullptr)
    {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(vampContext->glError, "Vector for glVertex3fv was empty\n");
        return;
    }
    if (vampContext->manualDraw)
    {
        std::vector<MaggieVertex> *vertices;
        MaggieVertex vertex;

        vertices = reinterpret_cast<std::vector<MaggieVertex>*>(vampContext->vertices);

        vertex.pos = {vec[0], vec[1], vec[2]};
        vertices->push_back(vertex);
    }
    else
    {
        magVertex(vec[0], vec[1], vec[2]);
    }
}

extern "C" void GLTexCoord2f(struct GLVampContext *vampContext, GLfloat s, GLfloat t)
{
    if (vampContext->currentTexture != -1)
    {
        if (vampContext->manualDraw == 0)
        {
            magTexCoord(vampContext->currentTexture, s, t);
        }
        else
        {
            std::vector<MaggieVertex> *vertices;

            vertices = reinterpret_cast<std::vector<MaggieVertex>*>(vampContext->vertices);

            if (!vertices->empty())
            {
                MaggieVertex& currentVertex = vertices->back();
                currentVertex.tex[0].u = s;
                currentVertex.tex[0].v = t;
                currentVertex.tex[0].w = 0;
            }
            else
            {
                vampContext->glError = GL_INVALID_OPERATION;
                GenerateGLError(vampContext->glError, "glTexCoord2f was called before glVertex was called\n");
            }
        }
    }
    else
    {
        vampContext->glError = GL_INVALID_OPERATION;
        GenerateGLError(vampContext->glError, "No Texture was bound on call of glTexCoord2f\n");
    }
}

extern "C" void GLTexCoord3f(struct GLVampContext *vampContext, GLfloat s, GLfloat t, GLfloat r)
{
    if (vampContext->currentTexture != -1)
    {
        if (vampContext->manualDraw == 0)
        {
            magTexCoord3(vampContext->currentTexture, s, t, r);
        }
        else
        {
            std::vector<MaggieVertex> *vertices;

            vertices = reinterpret_cast<std::vector<MaggieVertex>*>(vampContext->vertices);

            if (!vertices->empty())
            {
                MaggieVertex& currentVertex = vertices->back();
                currentVertex.tex[0].u = s;
                currentVertex.tex[0].v = t;
                currentVertex.tex[0].w = r;
            }
            else
            {
                vampContext->glError = GL_INVALID_OPERATION;
                GenerateGLError(vampContext->glError, "glTexCoord3f was called before glVertex was called\n");
            }
        }
    }
    else
    {
        vampContext->glError = GL_INVALID_OPERATION;
        GenerateGLError(vampContext->glError, "No Texture was bound on call of glTexCoord3f\n");
    }
}
