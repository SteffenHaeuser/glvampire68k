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

extern "C" void GLDebugMessageCallback(struct GLVampContext *vampContext, DebugMessageCallbackFunc callback, const void* userParam)
{
    vampContext->glDebugMessage = callback;
}

extern "C" void GLGenerateError(GLVampContext *vampContext, int type, const char* message)
{
    if (vampContext->glDebugMessage != 0)
    {
        vampContext->glDebugMessage(GL_DEBUG_SOURCE_API, type, 0, GL_DEBUG_SEVERITY_HIGH, strlen(message), message, nullptr);
    }
}

extern "C" int GLGetError(struct GLVampContext *vampContext)
{
	return vampContext->glError;
}