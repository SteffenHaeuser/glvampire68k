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

extern "C" void GLAlphaFunc(struct GLVampContext* vampContext, int i, float j)
{
	vampContext->alphaFunc = i;
	vampContext->alphaRef = j;
}

void ApplyAlphaFunc(struct GLVampContext* vampContext, std::vector<MaggieVertex>& vertices)
{
	int alphaFunc = vampContext->alphaFunc;
	float alphaRef = vampContext->alphaRef;
    switch (alphaFunc) {
        case GL_NEVER:
            // Alpha test logic for GL_NEVER
            // Reject all fragments by setting the alpha value to 0
            for (auto& vertex : vertices) {
                vertex.colour &= 0x00FFFFFF; // Clear the alpha component
            }
            break;
        case GL_LESS:
            // Alpha test logic for GL_LESS
            // Pass fragments with alpha value less than the reference value
            for (auto& vertex : vertices) {
                float alpha = static_cast<float>(vertex.colour >> 24) / 255.0f; // Get the alpha value
                if (alpha < alphaRef) {
                    vertex.colour &= 0x00FFFFFF; // Clear the alpha component
                }
            }
            break;
        case GL_EQUAL:
            // Alpha test logic for GL_EQUAL
            // Pass fragments with alpha value equal to the reference value
            for (auto& vertex : vertices) {
                float alpha = static_cast<float>(vertex.colour >> 24) / 255.0f; // Get the alpha value
                if (alpha != alphaRef) {
                    vertex.colour &= 0x00FFFFFF; // Clear the alpha component
                }
            }
            break;
        case GL_LEQUAL:
            // Alpha test logic for GL_LEQUAL
            // Pass fragments with alpha value less than or equal to the reference value
            for (auto& vertex : vertices) {
                float alpha = static_cast<float>(vertex.colour >> 24) / 255.0f; // Get the alpha value
                if (alpha > alphaRef) {
                    vertex.colour &= 0x00FFFFFF; // Clear the alpha component
                }
            }
            break;
        case GL_GREATER:
            // Alpha test logic for GL_GREATER
            // Pass fragments with alpha value greater than the reference value
            for (auto& vertex : vertices) {
                float alpha = static_cast<float>(vertex.colour >> 24) / 255.0f; // Get the alpha value
                if (alpha <= alphaRef) {
                    vertex.colour &= 0x00FFFFFF; // Clear the alpha component
                }
            }
            break;
        case GL_NOTEQUAL:
            // Alpha test logic for GL_NOTEQUAL
            // Pass fragments with alpha value not equal to the reference value
            for (auto& vertex : vertices) {
                float alpha = static_cast<float>(vertex.colour >> 24) / 255.0f; // Get the alpha value
                if (alpha == alphaRef) {
                    vertex.colour &= 0x00FFFFFF; // Clear the alpha component
                }
            }
            break;
        case GL_GEQUAL:
            // Alpha test logic for GL_GEQUAL
            // Pass fragments with alpha value greater than or equal to the reference value
            for (auto& vertex : vertices) {
                float alpha = static_cast<float>(vertex.colour >> 24) / 255.0f; // Get the alpha value
                if (alpha < alphaRef) {
                    vertex.colour &= 0x00FFFFFF; // Clear the alpha component
                }
            }
            break;
        case GL_ALWAYS:
            // Alpha test logic for GL_ALWAYS
            // Pass all fragments (no modification needed)
            break;
        default:
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid alphaFunc value");
            break;
    }
}


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

extern "C" void GLTexParameteri(struct GLVampContext *vampContext, int i, int j, int k)
{
	if (i!=GL_TEXTURE_2D)
	{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"glTexParameteri currently only supports GL_TEXTURE_2D, %d is not supported\n",i);
				GenerateGLError(vampContext->glError,error);		
				
				return;
	}	
	if (j == GL_TEXTURE_MIN_FILTER)
	{
		if (k == GL_LINEAR)
		{
			vampContext->vampDrawModes|= MAG_DRAWMODE_BILINEAR;		
		}
		else if (k == GL_NEAREST)
		{
			vampContext->vampDrawModes &= ~MAG_DRAWMODE_BILINEAR;
		}
		else
		{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"glTexParameteri currently only supports GL_LINEAR and GL_NEAREST, %d is not supported\n",k);
				GenerateGLError(vampContext->glError,error);		
				
				return;			
		}
	}
	else if (j == GL_TEXTURE_MAG_FILTER)
	{
		if (k == GL_LINEAR)
		{
			vampContext->vampDrawModes|= MAG_DRAWMODE_BILINEAR;		
		}
		else if (k == GL_NEAREST)
		{
			vampContext->vampDrawModes &= ~MAG_DRAWMODE_BILINEAR;
		}	
		else
		{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"glTexParameteri currently only supports GL_LINEAR and GL_NEAREST, %d is not supported\n",k);
				GenerateGLError(vampContext->glError,error);		
				
				return;			
		}		
	}
	else if (j == GL_TEXTURE_WRAP_S)
	{
	}
	else if (j == GL_TEXTURE_WRAP_T)
	{
	}
	else
	{
				char error[1024];
				
				vampContext->glError = GL_INVALID_OPERATION;
				sprintf(error,"glTexParameteri currently does not support %d\n",j);
				GenerateGLError(vampContext->glError,error);		
				
				return;		
	}
}

extern "C" void GLTexEnvi(GLVampContext* vampContext, int target, int pname, int param)
{
    if (target != GL_TEXTURE_ENV) {
        GenerateGLError(GL_INVALID_ENUM, "Invalid target for glTexEnvi");
        return;
    }

    switch (pname) {
        case GL_TEXTURE_ENV_MODE:
            switch (param) {
                case GL_REPLACE:
					vampContext->texenv = 0;
					break;
				case GL_MODULATE:
					vampContext->texenv = 1;
					break;
				case GL_DECAL:
					vampContext->texenv = 2;
					break;
                case GL_BLEND:
					vampContext->texenv = 3;
					break;
                case GL_ADD:
                    vampContext->texenv = 4;
                    break;
                default:
					vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(GL_INVALID_ENUM, "Invalid parameter value for glTexEnvi");
                    return;
            }
            break;
        default:
			vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid parameter name for glTexEnvi");
            return;
    }
}

extern "C" void GLTexEnvf(GLVampContext* vampContext, int target, int pname, float param)
{
    if (target != GL_TEXTURE_ENV) {
        GenerateGLError(GL_INVALID_ENUM, "Invalid target for glTexEnvi");
        return;
    }

    switch (pname) {
        case GL_TEXTURE_ENV_MODE:
            switch ((int)param) {
                case GL_REPLACE:
					vampContext->texenv = 0;
					break;
				case GL_MODULATE:
					vampContext->texenv = 1;
					break;
				case GL_DECAL:
					vampContext->texenv = 2;
					break;
                case GL_BLEND:
					vampContext->texenv = 3;
					break;
                case GL_ADD:
                    vampContext->texenv = 4;
                    break;
                default:
					vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(GL_INVALID_ENUM, "Invalid parameter value for glTexEnvi");
                    return;
            }
            break;
        default:
			vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(GL_INVALID_ENUM, "Invalid parameter name for glTexEnvi");
            return;
    }
}