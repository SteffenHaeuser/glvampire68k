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

extern "C" void GLEnable(struct GLVampContext* vampContext, int i)
{
    switch (i) {
        case GL_FOG:
            vampContext->useFogging = true;
            break;
        case GL_FOG_COORD_SRC:
            vampContext->useFogCoordSrc = true;
            break;
		case GL_BLEND:
			vampContext->useBlending = true;
			break;
        // Add more cases for other GL_ENABLE flags if needed
        default:
            break;
    }
}

extern "C" void GLDisable(struct GLVampContext* vampContext, int i)
{
    switch (i) {
        case GL_FOG:
            vampContext->useFogging = false;
            break;
        case GL_FOG_COORD_SRC:
            vampContext->useFogCoordSrc = false;
            break;
		case GL_BLEND:
			vampContext->useBlending = false;
			break;
        // Add more cases for other GL_DISABLE flags if needed
        default:
            break;
    }
}