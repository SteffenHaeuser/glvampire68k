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

extern "C" void GLBindTexture(struct GLVampContext *vampContext, int i, int j)
{
	if (i==GL_TEXTURE_2D)
	{
		std::map<int,int> *vampTextureMap = (std::map<int,int> *)vampContext->vampTextureMap;
		auto num = vampTextureMap->find(j);
		if (num!=vampTextureMap->end())
		{
			vampContext->currentTexture = num->second;
			magSetTexture(0,vampContext->currentTexture);
		}
		else 
		{
			char error[1024];
			
			sprintf(error,"No Texture %d was found on glBindTexture\n",i);
			vampContext->glError = GL_INVALID_OPERATION;
			GenerateGLError(vampContext->glError,error);			
		}
	}
}

extern "C" void GLTexImage2D(struct GLVampContext *vampContext, int i, int j, int k, int l, int m, int n, int o, int p, void *pixels)
{
	int texHandle = -1;
	int format=-1;
	
	if (p!=GL_UNSIGNED_BYTE)
	{
		char error[1024];
		
		sprintf(error,"glTexImage2D currently only supports Textures in GL_UNSIGNED_BYTE\n");
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,error);
		
		return;			
	}
	
	switch(o)
	{
		case GL_RGBA:
			format = MAG_TEXFMT_RGBA;
			break;
		case GL_RGB:
			format = MAG_TEXFMT_RGB;
			break;
		case GL_DXT1:
			format = MAG_TEXFMT_DXT1;
			break;
		case GL_INTENSITY8:
			break;
		case GL_LUMINANCE8:
			break;
		default:
			break;
	}
	if (format==-1)
	{
		char error[1024];
		
		sprintf(error,"Invalid Texture Format for glTexImage2D: %d\n",o);
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,error);
		
		return;		
	}
	if (i==GL_TEXTURE_2D)
	{
		std::map<int,int> *vampTextureMap;
		vampTextureMap = (std::map<int,int>*)vampContext->vampTextureMap;
		if (l*m==64*64) texHandle = magAllocateTexture(6);
		else if (l*m==128*128) texHandle = magAllocateTexture(7);
		else if (l*m==256*256) texHandle = magAllocateTexture(8);
		if (!texHandle) 
		{
			char error[1024];
			
			sprintf(error,"Could not allocate Texture in call to glexImage2D(%d,%d,%d,%d,%d,%d,%d,%d,ptr)",i,j,k,l,m,n,o,p);
			vampContext->glError = GL_OUT_OF_MEMORY;
			GenerateGLError(vampContext->glError,error);	
	
			return;
		}
		vampTextureMap->insert(std::make_pair(vampContext->maxVampTex, texHandle));
		vampContext->maxVampTex++;
		magUploadTexture(texHandle, j, pixels, format);
	}
}

extern "C" void GLDeleteTextures(struct GLVampContext *vampContext, int num, void *v)
{
	if (num==1)
	{
		int texnum = *((int*)(v));
		
		if (texnum!=0)
		{
			std::map<int,int> *vampTextureMap;
			vampTextureMap = (std::map<int,int>*)vampContext->vampTextureMap;
			auto it = vampTextureMap->find(texnum);
			if (it != vampTextureMap->end()) 
			{
				int number = it->second;
				vampTextureMap->erase(it);
				magFreeTexture(number);
			}
			else 
			{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"Could not find texture %d\n",texnum);
				GenerateGLError(vampContext->glError,error);					
			}
		}
	}
	else 
	{
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError,"glDeleteTextures currently only supports if first parameter is 1\n");
	}
}

extern "C" void GLTexGeni(struct GLVampContext *vampContext, __attribute__((unused)) int i, int j, int k)
{
	if (j!=GL_TEXTURE_GEN_MODE)
	{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"glTexGeni currently only supports GL_TEXTURE_GEN_MODE, %d is not supported\n",j);
				GenerateGLError(vampContext->glError,error);		
				
				return;
	}
	if (k==GL_SPHERE_MAP)
	{
		vampContext->vampDrawModes|= MAG_DRAWMODE_TEXGEN_REFLECT;
	}
	else if (k==GL_NORMAL_MAP)
	{
		vampContext->vampDrawModes &= ~MAG_DRAWMODE_TEXGEN_REFLECT;
	}	
	else
	{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"Invalid TexGen mode %d\n",k);
				GenerateGLError(vampContext->glError,error);		
	}
}
