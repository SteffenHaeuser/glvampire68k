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

bool IsValidAlphaFunction(GLenum func)
{
    return func == GL_NEVER || func == GL_LESS || func == GL_EQUAL || func == GL_LEQUAL ||
           func == GL_GREATER || func == GL_NOTEQUAL || func == GL_GEQUAL || func == GL_ALWAYS;
}

extern "C" void GLAlphaFunc(struct GLVampContext* vampContext, GLenum func, GLfloat ref)
{
    if (!IsValidAlphaFunction(func)) {
        vampContext->glError = GL_INVALID_ENUM;
        GenerateGLError(vampContext->glError, "Invalid alpha function for GLAlphaFunc");
        return;
    }

    vampContext->alphaFunc = func;
    vampContext->alphaRef = ref;
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


extern "C" void GLBindTexture(struct GLVampContext *vampContext, GLenum target, GLuint texture)
{
	if (target == GL_TEXTURE_2D)
	{
		std::map<int, int> *vampTextureMap = (std::map<int, int> *)vampContext->vampTextureMap;
		auto num = vampTextureMap->find(texture);
		if (num != vampTextureMap->end())
		{
			vampContext->currentTexture = num->second;
			printf("TexSet: %d\n",vampContext->currentTexture);
			magSetTexture(0, vampContext->currentTexture);
		}
		else
		{
			char error[1024];

			sprintf(error, "No Texture %d was found on glBindTexture\n", target);
			vampContext->glError = GL_INVALID_OPERATION;
			GenerateGLError(vampContext->glError, error);
		}
	}
}

extern "C" void GLTexImage2D(struct GLVampContext *vampContext, GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, void *pixels)
{
	int texHandle = -1;
	GLint magFormat = -1;

	if (type != GL_UNSIGNED_BYTE)
	{
		char error[1024];

		sprintf(error, "glTexImage2D currently only supports Textures in GL_UNSIGNED_BYTE\n");
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError, error);

		return;
	}

	switch (format)
	{
	case GL_RGBA:
		magFormat = MAG_TEXFMT_RGBA;
		break;
	case GL_RGB:
		magFormat = MAG_TEXFMT_RGB;
		break;
	case GL_DXT1:
		magFormat = MAG_TEXFMT_DXT1;
		break;
	case GL_INTENSITY8:
		break;
	case GL_LUMINANCE8:
		break;
	default:
		break;
	}

	if (magFormat == -1)
	{
		char error[1024];

		sprintf(error, "Invalid Texture Format for glTexImage2D: %d\n", format);
		vampContext->glError = GL_INVALID_OPERATION;
		GenerateGLError(vampContext->glError, error);

		return;
	}

	if (target == GL_TEXTURE_2D)
	{
		std::map<int, int> *vampTextureMap;
		vampTextureMap = (std::map<int, int>*)vampContext->vampTextureMap;

		if (width * height == 64 * 64)
			texHandle = magAllocateTexture(6);
		else if (width * height == 128 * 128)
			texHandle = magAllocateTexture(7);
		else if (width * height == 256 * 256)
			texHandle = magAllocateTexture(8);

		if (!texHandle)
		{
			char error[1024];

			sprintf(error, "Could not allocate Texture in call to glTexImage2D(%d, %d, %d, %d, %d, %d, %d, %d, ptr)", target, level, internalFormat, width, height, border, format, type);
			vampContext->glError = GL_OUT_OF_MEMORY;
			GenerateGLError(vampContext->glError, error);

			return;
		}

		vampTextureMap->insert(std::make_pair(vampContext->maxVampTex, texHandle));
		vampContext->maxVampTex++;
		magUploadTexture(texHandle, level, pixels, magFormat);
		printf("Texture Uploaded: %x %x %d %d\n",texHandle,pixels,level,magFormat);
	}
}

extern "C" void GLDeleteTextures(struct GLVampContext* vampContext, GLsizei num, GLuint* textures)
{
    if (num < 1)
    {
        vampContext->glError = GL_INVALID_VALUE;
        GenerateGLError(vampContext->glError, "Invalid value for num in glDeleteTextures");
        return;
    }

    std::map<int, int>* vampTextureMap = (std::map<int, int>*)vampContext->vampTextureMap;

    for (GLsizei i = 0; i < num; ++i)
    {
        GLuint texnum = textures[i];

        if (texnum != 0)
        {
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
                sprintf(error, "Could not find texture %d\n", texnum);
                GenerateGLError(vampContext->glError, error);
            }
        }
    }
}

extern "C" void GLTexGeni(struct GLVampContext* vampContext, __attribute__((unused)) GLenum coord, GLenum pname, GLint param)
{
	if (pname != GL_TEXTURE_GEN_MODE)
	{
		vampContext->glError = GL_INVALID_OPERATION;
		char error[1024];
		sprintf(error, "glTexGeni currently only supports GL_TEXTURE_GEN_MODE, %d is not supported\n", pname);
		GenerateGLError(vampContext->glError, error);
		return;
	}

	if (param == GL_SPHERE_MAP)
	{
		vampContext->vampDrawModes |= MAG_DRAWMODE_TEXGEN_REFLECT;
	}
	else if (param == GL_NORMAL_MAP)
	{
		vampContext->vampDrawModes &= ~MAG_DRAWMODE_TEXGEN_REFLECT;
	}
	else
	{
		vampContext->glError = GL_INVALID_OPERATION;
		char error[1024];
		sprintf(error, "Invalid TexGen mode %d\n", param);
		GenerateGLError(vampContext->glError, error);
	}
}

extern "C" void GLTexParameteri(struct GLVampContext* vampContext, GLenum target, GLenum pname, GLint param)
{
	if (target != GL_TEXTURE_2D)
	{
		vampContext->glError = GL_INVALID_OPERATION;
		char error[1024];
		sprintf(error, "glTexParameteri currently only supports GL_TEXTURE_2D, %d is not supported\n", target);
		GenerateGLError(vampContext->glError, error);
		return;
	}

	if (pname == GL_TEXTURE_MIN_FILTER)
	{
		if (param == GL_LINEAR)
		{
			vampContext->vampDrawModes |= MAG_DRAWMODE_BILINEAR;
		}
		else if (param == GL_NEAREST)
		{
			vampContext->vampDrawModes &= ~MAG_DRAWMODE_BILINEAR;
		}
		else
		{
			vampContext->glError = GL_INVALID_OPERATION;
			char error[1024];
			sprintf(error, "glTexParameteri currently only supports GL_LINEAR and GL_NEAREST, %d is not supported\n", param);
			GenerateGLError(vampContext->glError, error);
			return;
		}
	}
	else if (pname == GL_TEXTURE_MAG_FILTER)
	{
		if (param == GL_LINEAR)
		{
			vampContext->vampDrawModes |= MAG_DRAWMODE_BILINEAR;
		}
		else if (param == GL_NEAREST)
		{
			vampContext->vampDrawModes &= ~MAG_DRAWMODE_BILINEAR;
		}
		else
		{
			vampContext->glError = GL_INVALID_OPERATION;
			char error[1024];
			sprintf(error, "glTexParameteri currently only supports GL_LINEAR and GL_NEAREST, %d is not supported\n", param);
			GenerateGLError(vampContext->glError, error);
			return;
		}
	}
	else if (pname == GL_TEXTURE_WRAP_S)
	{
		// Handle GL_TEXTURE_WRAP_S parameter
	}
	else if (pname == GL_TEXTURE_WRAP_T)
	{
		// Handle GL_TEXTURE_WRAP_T parameter
	}
	else
	{
		vampContext->glError = GL_INVALID_OPERATION;
		char error[1024];
		sprintf(error, "glTexParameteri currently does not support %d\n", pname);
		GenerateGLError(vampContext->glError, error);
		return;
	}
}

extern "C" void GLTexEnvi(GLVampContext* vampContext, GLenum target, GLenum pname, GLint param)
{
    if (target != GL_TEXTURE_ENV) {
        vampContext->glError = GL_INVALID_ENUM;
        GenerateGLError(vampContext->glError, "Invalid target for glTexEnvi");
        return;
    }

    switch (pname) {
        case GL_TEXTURE_ENV_MODE:
            switch (param) {
                case GL_REPLACE:
                    vampContext->texenv = GL_REPLACE;
                    break;
                case GL_MODULATE:
                    vampContext->texenv = GL_MODULATE;
                    break;
                case GL_DECAL:
                    vampContext->texenv = GL_DECAL;
                    break;
                case GL_BLEND:
                    vampContext->texenv = GL_BLEND;
                    break;
                case GL_ADD:
                    vampContext->texenv = GL_ADD;
                    break;
                default:
                    vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(vampContext->glError, "Invalid parameter value for glTexEnvi");
                    return;
            }
            break;
        case GL_COMBINE_RGB:
            // Handle GL_COMBINE_RGB parameter
            vampContext->combineRGB = param;
            break;
        case GL_COMBINE_ALPHA:
            // Handle GL_COMBINE_ALPHA parameter
            vampContext->combineAlpha = param;
            break;
        default:
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(vampContext->glError, "Invalid parameter name for glTexEnvi");
            return;
    }
}

extern "C" void GLTexEnvf(GLVampContext* vampContext, GLenum target, GLenum pname, GLfloat param)
{
    if (target != GL_TEXTURE_ENV) {
        vampContext->glError = GL_INVALID_ENUM;
        GenerateGLError(vampContext->glError, "Invalid target for glTexEnvf");
        return;
    }

    switch (pname) {
        case GL_TEXTURE_ENV_MODE:
            switch ((int)param) {
                case GL_REPLACE:
                    vampContext->texenv = GL_REPLACE;
                    break;
                case GL_MODULATE:
                    vampContext->texenv = GL_MODULATE;
                    break;
                case GL_DECAL:
                    vampContext->texenv = GL_DECAL;
                    break;
                case GL_BLEND:
                    vampContext->texenv = GL_BLEND;
                    break;
                case GL_ADD:
                    vampContext->texenv = GL_ADD;
                    break;
                default:
                    vampContext->glError = GL_INVALID_ENUM;
                    GenerateGLError(vampContext->glError, "Invalid parameter value for glTexEnvf");
                    return;
            }
            break;
        case GL_COMBINE_RGB:
            // Handle GL_COMBINE_RGB parameter
            vampContext->combineRGB = static_cast<GLint>(param);
            break;
        case GL_COMBINE_ALPHA:
            // Handle GL_COMBINE_ALPHA parameter
            vampContext->combineAlpha = static_cast<GLint>(param);
            break;
        default:
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(vampContext->glError, "Invalid parameter name for glTexEnvf");
            return;
    }
}

ULONG ARGB32ColorFromFloats(float alpha, float red, float green, float blue)
{
    ULONG a = static_cast<ULONG>(alpha * 255.0f + 0.5f);
    ULONG r = static_cast<ULONG>(red * 255.0f + 0.5f);
    ULONG g = static_cast<ULONG>(green * 255.0f + 0.5f);
    ULONG b = static_cast<ULONG>(blue * 255.0f + 0.5f);

    return (a << 24) | (r << 16) | (g << 8) | b;
}

void GLTexEnvfv(GLVampContext* vampContext, GLenum target, GLenum pname, const GLfloat* params)
{
    if (target != GL_TEXTURE_ENV) {
        vampContext->glError = GL_INVALID_ENUM;
        GenerateGLError(vampContext->glError, "Invalid target for glTexEnvfv");
        return;
    }

    switch (pname) {
        case GL_TEXTURE_ENV_COLOR:
            // Handle GL_TEXTURE_ENV_COLOR parameter
            vampContext->texEnvColor = ARGB32ColorFromFloats(params[0], params[1], params[2], params[3]);
            break;
        case GL_COMBINE_RGB:
            // Handle GL_COMBINE_RGB parameter
            vampContext->combineRGB = static_cast<GLint>(params[0]);
            break;
        case GL_COMBINE_ALPHA:
            // Handle GL_COMBINE_ALPHA parameter
            vampContext->combineAlpha = static_cast<GLint>(params[0]);
            break;
        case GL_RGB_SCALE:
            // Handle GL_RGB_SCALE parameter
            vampContext->rgbScale = params[0];
            break;
        case GL_ALPHA_SCALE:
            // Handle GL_ALPHA_SCALE parameter
            vampContext->alphaScale = params[0];
            break;
        case GL_COMBINE_CONSTANT:
            // Handle GL_COMBINE_CONSTANT parameter
            vampContext->combineRGBConstant = ARGB32ColorFromFloats(params[0], params[1], params[2], params[3]);
            break;
        default:
            vampContext->glError = GL_INVALID_ENUM;
            GenerateGLError(vampContext->glError, "Invalid parameter name for glTexEnvfv");
            return;
    }
}
