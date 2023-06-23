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

extern "C" void GLDepthFunc(struct GLVampContext *vampContext, int i)
{
	if (i!=GL_LEQUAL)
	{
		vampContext->glError = GL_INVALID_ENUM;
		GenerateGLError(vampContext->glError,"Maggie Chip only support GL_LEQUAL for glDepthFunc\n");
	}
}

extern "C" void GLDepthMask(struct GLVampContext *vampContext, int i)
{
	// Not really correct implementation, DepthMask should only affect the writing, which currently is not possible on the Maggie Chipset
	
	if (i==GL_TRUE)
	{
		vampContext->vampDrawModes|= MAG_DRAWMODE_DEPTHBUFFER;
	}
	else
	{
		vampContext->vampDrawModes &= ~MAG_DRAWMODE_DEPTHBUFFER;
	}
}