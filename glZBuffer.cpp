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

extern "C" void GLDepthFunc(struct GLVampContext* vampContext, GLenum func)
{
    if (func != GL_LEQUAL)
    {
        vampContext->glError = GL_INVALID_ENUM;
        GenerateGLError(vampContext->glError, "Maggie Chip only supports GL_LEQUAL for glDepthFunc\n");
    }
}

extern "C" void GLDepthMask(struct GLVampContext* vampContext, GLboolean flag)
{
    // Not a correct implementation, provided as a placeholder
    if (flag == GL_TRUE)
    {
        vampContext->vampDrawModes |= MAG_DRAWMODE_DEPTHBUFFER;
    }
    else
    {
        vampContext->vampDrawModes &= ~MAG_DRAWMODE_DEPTHBUFFER;
    }
}

extern "C" void GLDepthRange(__attribute__((unused)) struct GLVampContext* vampContext, __attribute__((unused)) GLdouble nearVal, __attribute__((unused)) GLdouble farVal)
{
}
