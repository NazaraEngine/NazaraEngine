// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGL_HPP
#define NAZARA_OPENGL_HPP

#ifdef NAZARA_RENDERER_OPENGL

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Utility/Enums.hpp>

// Inclusion des headers OpenGL
#include <GL3/glcorearb.h>
#include <GL3/glext.h>
#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <GL3/wglext.h>
#elif defined(NAZARA_PLATFORM_LINUX)
namespace GLX
{
	#include <GL3/glx.h>
}
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

class NAZARA_RENDERER_API NzOpenGL
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
			GLint swizzle[4];
		};

		NzOpenGL() = delete;
		~NzOpenGL() = delete;

		static void ApplyStates(const NzRenderStates& states);

		static void BindBuffer(nzBufferType type, GLuint id);
		static void BindProgram(GLuint id);
		static void BindSampler(GLuint unit, GLuint id);
		static void BindScissorBox(const NzRecti& scissorBox);
		static void BindTexture(nzImageType type, GLuint id);
		static void BindTexture(unsigned int textureUnit, nzImageType type, GLuint id);
		static void BindTextureUnit(unsigned int textureUnit);
		static void BindViewport(const NzRecti& viewport);

		static void DeleteBuffer(nzBufferType type, GLuint id);
		static void DeleteFrameBuffer(const NzContext* context, GLuint id);
		static void DeleteProgram(GLuint id);
		static void DeleteSampler(GLuint id);
		static void DeleteTexture(GLuint id);
		static void DeleteVertexArray(const NzContext* context, GLuint id);

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
		static GLenum BlendFunc[nzBlendFunc_Max+1];
		static GLenum BufferLock[nzBufferAccess_Max+1];
		static GLenum BufferLockRange[nzBufferAccess_Max+1];
		static GLenum BufferTarget[nzBufferType_Max+1];
		static GLenum BufferTargetBinding[nzBufferType_Max+1];
		static GLenum BufferUsage[nzBufferUsage_Max+1];
		static GLenum ComponentType[nzComponentType_Max+1];
		static GLenum CubemapFace[6]; // Un cube possède six faces et ça n'est pas près de changer
		static GLenum FaceFilling[nzFaceFilling_Max+1];
		static GLenum FaceSide[nzFaceSide_Max+1];
		static GLenum PrimitiveMode[nzPrimitiveMode_Max+1];
		static GLenum QueryCondition[nzGpuQueryCondition_Max+1];
		static GLenum QueryMode[nzGpuQueryMode_Max+1];
		static GLenum RendererComparison[nzRendererComparison_Max+1];
		static GLenum RendererParameter[nzRendererParameter_Max+1];
		static GLenum SamplerWrapMode[nzSamplerWrap_Max+1];
		static GLenum ShaderStage[nzShaderStage_Max+1];
		static GLenum StencilOperation[nzStencilOperation_Max+1];
		static GLenum TextureTarget[nzImageType_Max+1];
		static GLenum TextureTargetBinding[nzImageType_Max+1];
		static GLenum TextureTargetProxy[nzImageType_Max+1];
		static nzUInt8 VertexComponentIndex[nzVertexComponent_Max+1];

	private:
		static void OnContextChanged(const NzContext* newContext);
		static void OnContextDestruction(const NzContext* context);
};

NAZARA_RENDERER_API extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
NAZARA_RENDERER_API extern PFNGLATTACHSHADERPROC             glAttachShader;
NAZARA_RENDERER_API extern PFNGLBEGINCONDITIONALRENDERPROC   glBeginConditionalRender;
NAZARA_RENDERER_API extern PFNGLBEGINQUERYPROC               glBeginQuery;
NAZARA_RENDERER_API extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
NAZARA_RENDERER_API extern PFNGLBINDBUFFERPROC               glBindBuffer;
NAZARA_RENDERER_API extern PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer;
NAZARA_RENDERER_API extern PFNGLBINDFRAGDATALOCATIONPROC     glBindFragDataLocation;
NAZARA_RENDERER_API extern PFNGLBINDRENDERBUFFERPROC         glBindRenderbuffer;
NAZARA_RENDERER_API extern PFNGLBINDSAMPLERPROC              glBindSampler;
NAZARA_RENDERER_API extern PFNGLBINDTEXTUREPROC              glBindTexture;
NAZARA_RENDERER_API extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
NAZARA_RENDERER_API extern PFNGLBLENDFUNCPROC                glBlendFunc;
NAZARA_RENDERER_API extern PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate;
NAZARA_RENDERER_API extern PFNGLBLITFRAMEBUFFERPROC          glBlitFramebuffer;
NAZARA_RENDERER_API extern PFNGLBUFFERDATAPROC               glBufferData;
NAZARA_RENDERER_API extern PFNGLBUFFERSUBDATAPROC            glBufferSubData;
NAZARA_RENDERER_API extern PFNGLCLEARPROC                    glClear;
NAZARA_RENDERER_API extern PFNGLCLEARCOLORPROC               glClearColor;
NAZARA_RENDERER_API extern PFNGLCLEARDEPTHPROC               glClearDepth;
NAZARA_RENDERER_API extern PFNGLCLEARSTENCILPROC             glClearStencil;
NAZARA_RENDERER_API extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
NAZARA_RENDERER_API extern PFNGLCREATESHADERPROC             glCreateShader;
NAZARA_RENDERER_API extern PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus;
NAZARA_RENDERER_API extern PFNGLCOLORMASKPROC                glColorMask;
NAZARA_RENDERER_API extern PFNGLCULLFACEPROC                 glCullFace;
NAZARA_RENDERER_API extern PFNGLCOMPILESHADERPROC            glCompileShader;
NAZARA_RENDERER_API extern PFNGLCOPYTEXSUBIMAGE2DPROC        glCopyTexSubImage2D;
NAZARA_RENDERER_API extern PFNGLDEBUGMESSAGECALLBACKPROC     glDebugMessageCallback;
NAZARA_RENDERER_API extern PFNGLDEBUGMESSAGECONTROLPROC      glDebugMessageControl;
NAZARA_RENDERER_API extern PFNGLDEBUGMESSAGEINSERTPROC       glDebugMessageInsert;
NAZARA_RENDERER_API extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
NAZARA_RENDERER_API extern PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers;
NAZARA_RENDERER_API extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
NAZARA_RENDERER_API extern PFNGLDELETEQUERIESPROC            glDeleteQueries;
NAZARA_RENDERER_API extern PFNGLDELETERENDERBUFFERSPROC      glDeleteRenderbuffers;
NAZARA_RENDERER_API extern PFNGLDELETESAMPLERSPROC           glDeleteSamplers;
NAZARA_RENDERER_API extern PFNGLDELETESHADERPROC             glDeleteShader;
NAZARA_RENDERER_API extern PFNGLDELETETEXTURESPROC           glDeleteTextures;
NAZARA_RENDERER_API extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
NAZARA_RENDERER_API extern PFNGLDEPTHFUNCPROC                glDepthFunc;
NAZARA_RENDERER_API extern PFNGLDEPTHMASKPROC                glDepthMask;
NAZARA_RENDERER_API extern PFNGLDISABLEPROC                  glDisable;
NAZARA_RENDERER_API extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
NAZARA_RENDERER_API extern PFNGLDRAWARRAYSPROC               glDrawArrays;
NAZARA_RENDERER_API extern PFNGLDRAWARRAYSINSTANCEDPROC      glDrawArraysInstanced;
NAZARA_RENDERER_API extern PFNGLDRAWBUFFERPROC               glDrawBuffer;
NAZARA_RENDERER_API extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
NAZARA_RENDERER_API extern PFNGLDRAWELEMENTSPROC             glDrawElements;
NAZARA_RENDERER_API extern PFNGLDRAWELEMENTSINSTANCEDPROC    glDrawElementsInstanced;
NAZARA_RENDERER_API extern PFNGLDRAWTEXTURENVPROC            glDrawTexture;
NAZARA_RENDERER_API extern PFNGLENABLEPROC                   glEnable;
NAZARA_RENDERER_API extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
NAZARA_RENDERER_API extern PFNGLENDCONDITIONALRENDERPROC     glEndConditionalRender;
NAZARA_RENDERER_API extern PFNGLENDQUERYPROC                 glEndQuery;
NAZARA_RENDERER_API extern PFNGLFLUSHPROC                    glFlush;
NAZARA_RENDERER_API extern PFNGLFRAMEBUFFERRENDERBUFFERPROC  glFramebufferRenderbuffer;
NAZARA_RENDERER_API extern PFNGLFRAMEBUFFERTEXTUREPROC       glFramebufferTexture;
NAZARA_RENDERER_API extern PFNGLFRAMEBUFFERTEXTURE1DPROC     glFramebufferTexture1D;
NAZARA_RENDERER_API extern PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D;
NAZARA_RENDERER_API extern PFNGLFRAMEBUFFERTEXTURE3DPROC     glFramebufferTexture3D;
NAZARA_RENDERER_API extern PFNGLFRAMEBUFFERTEXTURELAYERPROC  glFramebufferTextureLayer;
NAZARA_RENDERER_API extern PFNGLGENERATEMIPMAPPROC           glGenerateMipmap;
NAZARA_RENDERER_API extern PFNGLGENBUFFERSPROC               glGenBuffers;
NAZARA_RENDERER_API extern PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers;
NAZARA_RENDERER_API extern PFNGLGENQUERIESPROC               glGenQueries;
NAZARA_RENDERER_API extern PFNGLGENRENDERBUFFERSPROC         glGenRenderbuffers;
NAZARA_RENDERER_API extern PFNGLGENSAMPLERSPROC              glGenSamplers;
NAZARA_RENDERER_API extern PFNGLGENTEXTURESPROC              glGenTextures;
NAZARA_RENDERER_API extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
NAZARA_RENDERER_API extern PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
NAZARA_RENDERER_API extern PFNGLGETBOOLEANVPROC              glGetBooleanv;
NAZARA_RENDERER_API extern PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv;
NAZARA_RENDERER_API extern PFNGLGETDEBUGMESSAGELOGPROC       glGetDebugMessageLog;
NAZARA_RENDERER_API extern PFNGLGETERRORPROC                 glGetError;
NAZARA_RENDERER_API extern PFNGLGETFLOATVPROC                glGetFloatv;
NAZARA_RENDERER_API extern PFNGLGETINTEGERVPROC              glGetIntegerv;
NAZARA_RENDERER_API extern PFNGLGETPROGRAMBINARYPROC         glGetProgramBinary;
NAZARA_RENDERER_API extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
NAZARA_RENDERER_API extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
NAZARA_RENDERER_API extern PFNGLGETQUERYIVPROC               glGetQueryiv;
NAZARA_RENDERER_API extern PFNGLGETQUERYOBJECTIVPROC         glGetQueryObjectiv;
NAZARA_RENDERER_API extern PFNGLGETQUERYOBJECTUIVPROC        glGetQueryObjectuiv;
NAZARA_RENDERER_API extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
NAZARA_RENDERER_API extern PFNGLGETSHADERIVPROC              glGetShaderiv;
NAZARA_RENDERER_API extern PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
NAZARA_RENDERER_API extern PFNGLGETSTRINGPROC                glGetString;
NAZARA_RENDERER_API extern PFNGLGETSTRINGIPROC               glGetStringi;
NAZARA_RENDERER_API extern PFNGLGETTEXIMAGEPROC              glGetTexImage;
NAZARA_RENDERER_API extern PFNGLGETTEXLEVELPARAMETERFVPROC   glGetTexLevelParameterfv;
NAZARA_RENDERER_API extern PFNGLGETTEXLEVELPARAMETERIVPROC   glGetTexLevelParameteriv;
NAZARA_RENDERER_API extern PFNGLGETTEXPARAMETERFVPROC        glGetTexParameterfv;
NAZARA_RENDERER_API extern PFNGLGETTEXPARAMETERIVPROC        glGetTexParameteriv;
NAZARA_RENDERER_API extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
NAZARA_RENDERER_API extern PFNGLINVALIDATEBUFFERDATAPROC     glInvalidateBufferData;
NAZARA_RENDERER_API extern PFNGLISENABLEDPROC                glIsEnabled;
NAZARA_RENDERER_API extern PFNGLLINEWIDTHPROC                glLineWidth;
NAZARA_RENDERER_API extern PFNGLLINKPROGRAMPROC              glLinkProgram;
NAZARA_RENDERER_API extern PFNGLMAPBUFFERPROC                glMapBuffer;
NAZARA_RENDERER_API extern PFNGLMAPBUFFERRANGEPROC           glMapBufferRange;
NAZARA_RENDERER_API extern PFNGLPIXELSTOREIPROC              glPixelStorei;
NAZARA_RENDERER_API extern PFNGLPOINTSIZEPROC                glPointSize;
NAZARA_RENDERER_API extern PFNGLPOLYGONMODEPROC              glPolygonMode;
NAZARA_RENDERER_API extern PFNGLPROGRAMBINARYPROC            glProgramBinary;
NAZARA_RENDERER_API extern PFNGLPROGRAMPARAMETERIPROC        glProgramParameteri;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM1DPROC         glProgramUniform1d;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM1FPROC         glProgramUniform1f;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM1IPROC         glProgramUniform1i;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM1DVPROC        glProgramUniform1dv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM1FVPROC        glProgramUniform1fv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM1IVPROC        glProgramUniform1iv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM2DVPROC        glProgramUniform2dv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM2FVPROC        glProgramUniform2fv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM2IVPROC        glProgramUniform2iv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM3DVPROC        glProgramUniform3dv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM3FVPROC        glProgramUniform3fv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM3IVPROC        glProgramUniform3iv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM4DVPROC        glProgramUniform4dv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM4FVPROC        glProgramUniform4fv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORM4IVPROC        glProgramUniform4iv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORMMATRIX4DVPROC  glProgramUniformMatrix4dv;
NAZARA_RENDERER_API extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC  glProgramUniformMatrix4fv;
NAZARA_RENDERER_API extern PFNGLREADPIXELSPROC               glReadPixels;
NAZARA_RENDERER_API extern PFNGLRENDERBUFFERSTORAGEPROC      glRenderbufferStorage;
NAZARA_RENDERER_API extern PFNGLSAMPLERPARAMETERFPROC        glSamplerParameterf;
NAZARA_RENDERER_API extern PFNGLSAMPLERPARAMETERIPROC        glSamplerParameteri;
NAZARA_RENDERER_API extern PFNGLSCISSORPROC                  glScissor;
NAZARA_RENDERER_API extern PFNGLSHADERSOURCEPROC             glShaderSource;
NAZARA_RENDERER_API extern PFNGLSTENCILFUNCPROC              glStencilFunc;
NAZARA_RENDERER_API extern PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
NAZARA_RENDERER_API extern PFNGLSTENCILOPPROC                glStencilOp;
NAZARA_RENDERER_API extern PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
NAZARA_RENDERER_API extern PFNGLTEXIMAGE1DPROC               glTexImage1D;
NAZARA_RENDERER_API extern PFNGLTEXIMAGE2DPROC               glTexImage2D;
NAZARA_RENDERER_API extern PFNGLTEXIMAGE3DPROC               glTexImage3D;
NAZARA_RENDERER_API extern PFNGLTEXPARAMETERFPROC            glTexParameterf;
NAZARA_RENDERER_API extern PFNGLTEXPARAMETERIPROC            glTexParameteri;
NAZARA_RENDERER_API extern PFNGLTEXSTORAGE1DPROC             glTexStorage1D;
NAZARA_RENDERER_API extern PFNGLTEXSTORAGE2DPROC             glTexStorage2D;
NAZARA_RENDERER_API extern PFNGLTEXSTORAGE3DPROC             glTexStorage3D;
NAZARA_RENDERER_API extern PFNGLTEXSUBIMAGE1DPROC            glTexSubImage1D;
NAZARA_RENDERER_API extern PFNGLTEXSUBIMAGE2DPROC            glTexSubImage2D;
NAZARA_RENDERER_API extern PFNGLTEXSUBIMAGE3DPROC            glTexSubImage3D;
NAZARA_RENDERER_API extern PFNGLUNIFORM1DPROC                glUniform1d;
NAZARA_RENDERER_API extern PFNGLUNIFORM1FPROC                glUniform1f;
NAZARA_RENDERER_API extern PFNGLUNIFORM1IPROC                glUniform1i;
NAZARA_RENDERER_API extern PFNGLUNIFORM1DVPROC               glUniform1dv;
NAZARA_RENDERER_API extern PFNGLUNIFORM1FVPROC               glUniform1fv;
NAZARA_RENDERER_API extern PFNGLUNIFORM1IVPROC               glUniform1iv;
NAZARA_RENDERER_API extern PFNGLUNIFORM2DVPROC               glUniform2dv;
NAZARA_RENDERER_API extern PFNGLUNIFORM2FVPROC               glUniform2fv;
NAZARA_RENDERER_API extern PFNGLUNIFORM2IVPROC               glUniform2iv;
NAZARA_RENDERER_API extern PFNGLUNIFORM3DVPROC               glUniform3dv;
NAZARA_RENDERER_API extern PFNGLUNIFORM3FVPROC               glUniform3fv;
NAZARA_RENDERER_API extern PFNGLUNIFORM3IVPROC               glUniform3iv;
NAZARA_RENDERER_API extern PFNGLUNIFORM4DVPROC               glUniform4dv;
NAZARA_RENDERER_API extern PFNGLUNIFORM4FVPROC               glUniform4fv;
NAZARA_RENDERER_API extern PFNGLUNIFORM4IVPROC               glUniform4iv;
NAZARA_RENDERER_API extern PFNGLUNIFORMMATRIX4DVPROC         glUniformMatrix4dv;
NAZARA_RENDERER_API extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
NAZARA_RENDERER_API extern PFNGLUNMAPBUFFERPROC              glUnmapBuffer;
NAZARA_RENDERER_API extern PFNGLUSEPROGRAMPROC               glUseProgram;
NAZARA_RENDERER_API extern PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
NAZARA_RENDERER_API extern PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
NAZARA_RENDERER_API extern PFNGLVERTEXATTRIBDIVISORPROC      glVertexAttribDivisor;
NAZARA_RENDERER_API extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
NAZARA_RENDERER_API extern PFNGLVERTEXATTRIBIPOINTERPROC     glVertexAttribIPointer;
NAZARA_RENDERER_API extern PFNGLVERTEXATTRIBLPOINTERPROC     glVertexAttribLPointer;
NAZARA_RENDERER_API extern PFNGLVIEWPORTPROC                 glViewport;
#if defined(NAZARA_PLATFORM_WINDOWS)
NAZARA_RENDERER_API extern PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormat;
NAZARA_RENDERER_API extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;
NAZARA_RENDERER_API extern PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
NAZARA_RENDERER_API extern PFNWGLGETEXTENSIONSSTRINGEXTPROC  wglGetExtensionsStringEXT;
NAZARA_RENDERER_API extern PFNWGLSWAPINTERVALEXTPROC         wglSwapInterval;
#elif defined(NAZARA_PLATFORM_LINUX)
NAZARA_RENDERER_API extern GLX::PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs;
NAZARA_RENDERER_API extern GLX::PFNGLXSWAPINTERVALEXTPROC         glXSwapIntervalEXT;
NAZARA_RENDERER_API extern GLX::PFNGLXSWAPINTERVALMESAPROC        NzglXSwapIntervalMESA;
NAZARA_RENDERER_API extern GLX::PFNGLXSWAPINTERVALSGIPROC         glXSwapIntervalSGI;
#endif

#endif // NAZARA_RENDERER_OPENGL

#endif // NAZARA_OPENGL_HPP
