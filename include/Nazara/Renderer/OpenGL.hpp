// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGL_HPP
#define NAZARA_OPENGL_HPP

#ifdef NAZARA_RENDERER_OPENGL

// gl3.h définit WIN32_LEAN_AND_MEAN qui entre en conflit avec la définition de Nazara et doit donc être inclut en premier
#include <GL3/glcorearb.h>
#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Utility/Enums.hpp>

// Inclusion des extensions
#include <GL3/glext.h>
#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <GL3/wglext.h>
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <GL3/glxext.h>
#endif

enum nzOpenGLExtension
{
	nzOpenGLExtension_AnisotropicFilter,
	nzOpenGLExtension_ConditionalRender,
	nzOpenGLExtension_DebugOutput,
	nzOpenGLExtension_DrawInstanced,
	nzOpenGLExtension_FP64,
	nzOpenGLExtension_FrameBufferObject,
	nzOpenGLExtension_GetProgramBinary,
	nzOpenGLExtension_InstancedArray,
	nzOpenGLExtension_PixelBufferObject,
	nzOpenGLExtension_SamplerObjects,
	nzOpenGLExtension_SeparateShaderObjects,
	nzOpenGLExtension_Shader_ImageLoadStore,
	nzOpenGLExtension_TextureArray,
	nzOpenGLExtension_TextureCompression_s3tc,
	nzOpenGLExtension_TextureStorage,
	nzOpenGLExtension_VertexArrayObjects,

	nzOpenGLExtension_Max = nzOpenGLExtension_VertexArrayObjects
};

class NzContext;
class NzRenderTarget;

using NzOpenGLFunc = void (*)();

class NAZARA_API NzOpenGL
{
	friend NzContext;

	public:
		enum FormatType
		{
			FormatType_RenderBuffer,
//			FormatType_MultisampleTexture,
			FormatType_Texture
		};

		struct Format
		{
			GLenum dataFormat;
			GLenum dataType;
			GLint internalFormat;
		};

		NzOpenGL() = delete;
		~NzOpenGL() = delete;

		static void ApplyStates(const NzRenderStates& states);

		static void BindBuffer(nzBufferType type, GLuint id);
		static void BindProgram(GLuint id);
		static void BindScissorBox(const NzRecti& scissorBox);
		static void BindTexture(nzImageType type, GLuint id);
		static void BindTexture(unsigned int textureUnit, nzImageType type, GLuint id);
		static void BindTextureUnit(unsigned int textureUnit);
		static void BindViewport(const NzRecti& viewport);

		static void DeleteBuffer(nzBufferType type, GLuint id);
		static void DeleteProgram(GLuint id);
		static void DeleteTexture(GLuint id);

		static GLuint GetCurrentBuffer(nzBufferType type);
		static GLuint GetCurrentProgram();
		static NzRecti GetCurrentScissorBox();
		static const NzRenderTarget* GetCurrentTarget();
		static GLuint GetCurrentTexture();
		static GLuint GetCurrentTexture(unsigned int textureUnit);
		static unsigned int GetCurrentTextureUnit();
		static NzRecti GetCurrentViewport();

		static NzOpenGLFunc GetEntry(const NzString& entryPoint);
		static unsigned int GetGLSLVersion();
		static NzString GetRendererName();
		static NzString GetVendorName();
		static unsigned int GetVersion();

		static bool Initialize();

		static bool IsInitialized();
		static bool IsSupported(nzOpenGLExtension extension);
		static bool IsSupported(const NzString& string);

		static void SetBuffer(nzBufferType type, GLuint id);
		static void SetProgram(GLuint id);
		static void SetScissorBox(const NzRecti& scissorBox);
		static void SetTarget(const NzRenderTarget* renderTarget);
		static void SetTexture(GLuint id);
		static void SetTexture(unsigned int textureUnit, GLuint id);
		static void SetTextureUnit(unsigned int textureUnit);
		static void SetViewport(const NzRecti& viewport);

		static bool TranslateFormat(nzPixelFormat pixelFormat, Format* format, FormatType target);

		static void Uninitialize();

		static GLenum Attachment[nzAttachmentPoint_Max+1];
		static nzUInt8 AttributeIndex[nzAttributeUsage_Max+1];
		static GLenum AttributeType[nzAttributeType_Max+1];
		static GLenum BlendFunc[nzBlendFunc_Max+1];
		static GLenum BufferLock[nzBufferAccess_Max+1];
		static GLenum BufferLockRange[nzBufferAccess_Max+1];
		static GLenum BufferTarget[nzBufferType_Max+1];
		static GLenum BufferTargetBinding[nzBufferType_Max+1];
		static GLenum BufferUsage[nzBufferUsage_Max+1];
		static GLenum CubemapFace[6]; // Un cube possède six faces et ça n'est pas prêt de changer
		static GLenum FaceFilling[nzFaceFilling_Max+1];
		static GLenum FaceSide[nzFaceSide_Max+1];
		static GLenum PrimitiveMode[nzPrimitiveMode_Max+1];
		static GLenum QueryCondition[nzGpuQueryCondition_Max+1];
		static GLenum QueryMode[nzGpuQueryMode_Max+1];
		static GLenum RendererComparison[nzRendererComparison_Max+1];
		static GLenum RendererParameter[nzRendererParameter_Max+1];
		static GLenum SamplerWrapMode[nzSamplerWrap_Max+1];
		static GLenum ShaderType[nzShaderType_Max+1];
		static GLenum StencilOperation[nzStencilOperation_Max+1];
		static GLenum TextureTarget[nzImageType_Max+1];
		static GLenum TextureTargetBinding[nzImageType_Max+1];
		static GLenum TextureTargetProxy[nzImageType_Max+1];

	private:
		static void OnContextDestruction(const NzContext* context);
		static void OnContextChange(const NzContext* newContext);
};

NAZARA_API extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
NAZARA_API extern PFNGLATTACHSHADERPROC             glAttachShader;
NAZARA_API extern PFNGLBEGINCONDITIONALRENDERPROC   glBeginConditionalRender;
NAZARA_API extern PFNGLBEGINQUERYPROC               glBeginQuery;
NAZARA_API extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
NAZARA_API extern PFNGLBINDBUFFERPROC               glBindBuffer;
NAZARA_API extern PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer;
NAZARA_API extern PFNGLBINDFRAGDATALOCATIONPROC     glBindFragDataLocation;
NAZARA_API extern PFNGLBINDRENDERBUFFERPROC         glBindRenderbuffer;
NAZARA_API extern PFNGLBINDSAMPLERPROC              glBindSampler;
NAZARA_API extern PFNGLBINDTEXTUREPROC              glBindTexture;
NAZARA_API extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
NAZARA_API extern PFNGLBLENDFUNCPROC                glBlendFunc;
NAZARA_API extern PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate;
NAZARA_API extern PFNGLBUFFERDATAPROC               glBufferData;
NAZARA_API extern PFNGLBUFFERSUBDATAPROC            glBufferSubData;
NAZARA_API extern PFNGLCLEARPROC                    glClear;
NAZARA_API extern PFNGLCLEARCOLORPROC               glClearColor;
NAZARA_API extern PFNGLCLEARDEPTHPROC               glClearDepth;
NAZARA_API extern PFNGLCLEARSTENCILPROC             glClearStencil;
NAZARA_API extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
NAZARA_API extern PFNGLCREATESHADERPROC             glCreateShader;
NAZARA_API extern PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus;
NAZARA_API extern PFNGLCOLORMASKPROC                glColorMask;
NAZARA_API extern PFNGLCULLFACEPROC                 glCullFace;
NAZARA_API extern PFNGLCOMPILESHADERPROC            glCompileShader;
NAZARA_API extern PFNGLCOPYTEXSUBIMAGE2DPROC        glCopyTexSubImage2D;
NAZARA_API extern PFNGLDEBUGMESSAGECALLBACKPROC     glDebugMessageCallback;
NAZARA_API extern PFNGLDEBUGMESSAGECONTROLPROC      glDebugMessageControl;
NAZARA_API extern PFNGLDEBUGMESSAGEINSERTPROC       glDebugMessageInsert;
NAZARA_API extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
NAZARA_API extern PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers;
NAZARA_API extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
NAZARA_API extern PFNGLDELETEQUERIESPROC            glDeleteQueries;
NAZARA_API extern PFNGLDELETERENDERBUFFERSPROC      glDeleteRenderbuffers;
NAZARA_API extern PFNGLDELETESAMPLERSPROC           glDeleteSamplers;
NAZARA_API extern PFNGLDELETESHADERPROC             glDeleteShader;
NAZARA_API extern PFNGLDELETETEXTURESPROC           glDeleteTextures;
NAZARA_API extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
NAZARA_API extern PFNGLDEPTHFUNCPROC                glDepthFunc;
NAZARA_API extern PFNGLDEPTHMASKPROC                glDepthMask;
NAZARA_API extern PFNGLDISABLEPROC                  glDisable;
NAZARA_API extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
NAZARA_API extern PFNGLDRAWARRAYSPROC               glDrawArrays;
NAZARA_API extern PFNGLDRAWARRAYSINSTANCEDPROC      glDrawArraysInstanced;
NAZARA_API extern PFNGLDRAWBUFFERPROC               glDrawBuffer;
NAZARA_API extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
NAZARA_API extern PFNGLDRAWELEMENTSPROC             glDrawElements;
NAZARA_API extern PFNGLDRAWELEMENTSINSTANCEDPROC    glDrawElementsInstanced;
NAZARA_API extern PFNGLDRAWTEXTURENVPROC            glDrawTexture;
NAZARA_API extern PFNGLENABLEPROC                   glEnable;
NAZARA_API extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
NAZARA_API extern PFNGLENDCONDITIONALRENDERPROC     glEndConditionalRender;
NAZARA_API extern PFNGLENDQUERYPROC                 glEndQuery;
NAZARA_API extern PFNGLFLUSHPROC                    glFlush;
NAZARA_API extern PFNGLFRAMEBUFFERRENDERBUFFERPROC  glFramebufferRenderbuffer;
NAZARA_API extern PFNGLFRAMEBUFFERTEXTUREPROC       glFramebufferTexture;
NAZARA_API extern PFNGLFRAMEBUFFERTEXTURE1DPROC     glFramebufferTexture1D;
NAZARA_API extern PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D;
NAZARA_API extern PFNGLFRAMEBUFFERTEXTURE3DPROC     glFramebufferTexture3D;
NAZARA_API extern PFNGLFRAMEBUFFERTEXTURELAYERPROC  glFramebufferTextureLayer;
NAZARA_API extern PFNGLGENERATEMIPMAPPROC           glGenerateMipmap;
NAZARA_API extern PFNGLGENBUFFERSPROC               glGenBuffers;
NAZARA_API extern PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers;
NAZARA_API extern PFNGLGENQUERIESPROC               glGenQueries;
NAZARA_API extern PFNGLGENRENDERBUFFERSPROC         glGenRenderbuffers;
NAZARA_API extern PFNGLGENSAMPLERSPROC              glGenSamplers;
NAZARA_API extern PFNGLGENTEXTURESPROC              glGenTextures;
NAZARA_API extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
NAZARA_API extern PFNGLGETBOOLEANVPROC              glGetBooleanv;
NAZARA_API extern PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv;
NAZARA_API extern PFNGLGETDEBUGMESSAGELOGPROC       glGetDebugMessageLog;
NAZARA_API extern PFNGLGETERRORPROC                 glGetError;
NAZARA_API extern PFNGLGETFLOATVPROC                glGetFloatv;
NAZARA_API extern PFNGLGETINTEGERVPROC              glGetIntegerv;
NAZARA_API extern PFNGLGETPROGRAMBINARYPROC         glGetProgramBinary;
NAZARA_API extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
NAZARA_API extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
NAZARA_API extern PFNGLGETQUERYIVPROC               glGetQueryiv;
NAZARA_API extern PFNGLGETQUERYOBJECTIVPROC         glGetQueryObjectiv;
NAZARA_API extern PFNGLGETQUERYOBJECTUIVPROC        glGetQueryObjectuiv;
NAZARA_API extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
NAZARA_API extern PFNGLGETSHADERIVPROC              glGetShaderiv;
NAZARA_API extern PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
NAZARA_API extern PFNGLGETSTRINGPROC                glGetString;
NAZARA_API extern PFNGLGETSTRINGIPROC               glGetStringi;
NAZARA_API extern PFNGLGETTEXIMAGEPROC              glGetTexImage;
NAZARA_API extern PFNGLGETTEXLEVELPARAMETERFVPROC   glGetTexLevelParameterfv;
NAZARA_API extern PFNGLGETTEXLEVELPARAMETERIVPROC   glGetTexLevelParameteriv;
NAZARA_API extern PFNGLGETTEXPARAMETERFVPROC        glGetTexParameterfv;
NAZARA_API extern PFNGLGETTEXPARAMETERIVPROC        glGetTexParameteriv;
NAZARA_API extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
NAZARA_API extern PFNGLINVALIDATEBUFFERDATAPROC     glInvalidateBufferData;
NAZARA_API extern PFNGLISENABLEDPROC                glIsEnabled;
NAZARA_API extern PFNGLLINEWIDTHPROC                glLineWidth;
NAZARA_API extern PFNGLLINKPROGRAMPROC              glLinkProgram;
NAZARA_API extern PFNGLMAPBUFFERPROC                glMapBuffer;
NAZARA_API extern PFNGLMAPBUFFERRANGEPROC           glMapBufferRange;
NAZARA_API extern PFNGLPIXELSTOREIPROC              glPixelStorei;
NAZARA_API extern PFNGLPOINTSIZEPROC                glPointSize;
NAZARA_API extern PFNGLPOLYGONMODEPROC              glPolygonMode;
NAZARA_API extern PFNGLPROGRAMBINARYPROC            glProgramBinary;
NAZARA_API extern PFNGLPROGRAMPARAMETERIPROC        glProgramParameteri;
NAZARA_API extern PFNGLPROGRAMUNIFORM1DPROC         glProgramUniform1d;
NAZARA_API extern PFNGLPROGRAMUNIFORM1FPROC         glProgramUniform1f;
NAZARA_API extern PFNGLPROGRAMUNIFORM1IPROC         glProgramUniform1i;
NAZARA_API extern PFNGLPROGRAMUNIFORM2DVPROC        glProgramUniform2dv;
NAZARA_API extern PFNGLPROGRAMUNIFORM2FVPROC        glProgramUniform2fv;
NAZARA_API extern PFNGLPROGRAMUNIFORM3DVPROC        glProgramUniform3dv;
NAZARA_API extern PFNGLPROGRAMUNIFORM3FVPROC        glProgramUniform3fv;
NAZARA_API extern PFNGLPROGRAMUNIFORM4DVPROC        glProgramUniform4dv;
NAZARA_API extern PFNGLPROGRAMUNIFORM4FVPROC        glProgramUniform4fv;
NAZARA_API extern PFNGLPROGRAMUNIFORMMATRIX4DVPROC  glProgramUniformMatrix4dv;
NAZARA_API extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC  glProgramUniformMatrix4fv;
NAZARA_API extern PFNGLREADPIXELSPROC               glReadPixels;
NAZARA_API extern PFNGLRENDERBUFFERSTORAGEPROC      glRenderbufferStorage;
NAZARA_API extern PFNGLSAMPLERPARAMETERFPROC        glSamplerParameterf;
NAZARA_API extern PFNGLSAMPLERPARAMETERIPROC        glSamplerParameteri;
NAZARA_API extern PFNGLSCISSORPROC                  glScissor;
NAZARA_API extern PFNGLSHADERSOURCEPROC             glShaderSource;
NAZARA_API extern PFNGLSTENCILFUNCPROC              glStencilFunc;
NAZARA_API extern PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
NAZARA_API extern PFNGLSTENCILOPPROC                glStencilOp;
NAZARA_API extern PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
NAZARA_API extern PFNGLTEXIMAGE1DPROC               glTexImage1D;
NAZARA_API extern PFNGLTEXIMAGE2DPROC               glTexImage2D;
NAZARA_API extern PFNGLTEXIMAGE3DPROC               glTexImage3D;
NAZARA_API extern PFNGLTEXPARAMETERFPROC            glTexParameterf;
NAZARA_API extern PFNGLTEXPARAMETERIPROC            glTexParameteri;
NAZARA_API extern PFNGLTEXSTORAGE1DPROC             glTexStorage1D;
NAZARA_API extern PFNGLTEXSTORAGE2DPROC             glTexStorage2D;
NAZARA_API extern PFNGLTEXSTORAGE3DPROC             glTexStorage3D;
NAZARA_API extern PFNGLTEXSUBIMAGE1DPROC            glTexSubImage1D;
NAZARA_API extern PFNGLTEXSUBIMAGE2DPROC            glTexSubImage2D;
NAZARA_API extern PFNGLTEXSUBIMAGE3DPROC            glTexSubImage3D;
NAZARA_API extern PFNGLUNIFORM1DPROC                glUniform1d;
NAZARA_API extern PFNGLUNIFORM1FPROC                glUniform1f;
NAZARA_API extern PFNGLUNIFORM1IPROC                glUniform1i;
NAZARA_API extern PFNGLUNIFORM2DVPROC               glUniform2dv;
NAZARA_API extern PFNGLUNIFORM2FVPROC               glUniform2fv;
NAZARA_API extern PFNGLUNIFORM3DVPROC               glUniform3dv;
NAZARA_API extern PFNGLUNIFORM3FVPROC               glUniform3fv;
NAZARA_API extern PFNGLUNIFORM4DVPROC               glUniform4dv;
NAZARA_API extern PFNGLUNIFORM4FVPROC               glUniform4fv;
NAZARA_API extern PFNGLUNIFORMMATRIX4DVPROC         glUniformMatrix4dv;
NAZARA_API extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
NAZARA_API extern PFNGLUNMAPBUFFERPROC              glUnmapBuffer;
NAZARA_API extern PFNGLUSEPROGRAMPROC               glUseProgram;
NAZARA_API extern PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
NAZARA_API extern PFNGLVERTEXATTRIBDIVISORPROC      glVertexAttribDivisor;
NAZARA_API extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
NAZARA_API extern PFNGLVIEWPORTPROC                 glViewport;
#if defined(NAZARA_PLATFORM_WINDOWS)
NAZARA_API extern PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormat;
NAZARA_API extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;
NAZARA_API extern PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
NAZARA_API extern PFNWGLGETEXTENSIONSSTRINGEXTPROC  wglGetExtensionsStringEXT;
NAZARA_API extern PFNWGLSWAPINTERVALEXTPROC         wglSwapInterval;
#elif defined(NAZARA_PLATFORM_LINUX)
NAZARA_API extern PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs;
NAZARA_API extern PFNGLXSWAPINTERVALSGIPROC         glXSwapInterval;
#endif

#endif // NAZARA_RENDERER_OPENGL

#endif // NAZARA_OPENGL_HPP
