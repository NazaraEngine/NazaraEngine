// Copyright (C) 2012 Jérôme Leclercq
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
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Renderer/Enums.hpp>

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
	nzOpenGLExtension_DebugOutput,
	nzOpenGLExtension_FP64,
	nzOpenGLExtension_FrameBufferObject,
	nzOpenGLExtension_PixelBufferObject,
	nzOpenGLExtension_SeparateShaderObjects,
	nzOpenGLExtension_TextureArray,
	nzOpenGLExtension_TextureCompression_s3tc,
	nzOpenGLExtension_TextureStorage,
	nzOpenGLExtension_VertexArrayObject,

	nzOpenGLExtension_Max = nzOpenGLExtension_VertexArrayObject
};

using NzOpenGLFunc = void (*)();

namespace NzOpenGL
{
	enum FormatType
	{
		FormatType_RenderBuffer,
//		FormatType_MultisampleTexture,
		FormatType_Texture
	};

	struct Format
	{
		GLenum dataFormat;
		GLenum dataType;
		GLint internalFormat;
	};

	NzOpenGLFunc GetEntry(const NzString& entryPoint);
	unsigned int GetVersion();
	bool Initialize();
	bool IsSupported(nzOpenGLExtension extension);
	bool IsSupported(const NzString& string);
	bool TranslateFormat(nzPixelFormat pixelFormat, Format* format, FormatType target);
	void Uninitialize();

	extern GLenum Attachment[nzAttachmentPoint_Max+1];
	extern nzUInt8 AttributeIndex[nzElementUsage_Max+1];
	extern GLenum BlendFunc[nzBlendFunc_Max+1];
	extern GLenum BufferLock[nzBufferAccess_Max+1];
	extern GLenum BufferLockRange[nzBufferAccess_Max+1];
	extern GLenum BufferTarget[nzBufferType_Max+1];
	extern GLenum BufferTargetBinding[nzBufferType_Max+1];
	extern GLenum BufferUsage[nzBufferUsage_Max+1];
	extern GLenum CubemapFace[6]; // Un cube possède six faces et ça n'est pas prêt de changer
	extern GLenum ElementType[nzElementType_Max+1];
	extern GLenum FaceCulling[nzFaceCulling_Max+1];
	extern GLenum FaceFilling[nzFaceFilling_Max+1];
	extern GLenum PrimitiveType[nzPrimitiveType_Max+1];
	extern GLenum RendererComparison[nzRendererComparison_Max+1];
	extern GLenum RendererParameter[nzRendererParameter_Max+1];
	extern GLenum ShaderType[nzShaderType_Max+1];
	extern GLenum StencilOperation[nzStencilOperation_Max+1];
	extern GLenum TextureTarget[nzImageType_Max+1];
	extern GLenum TextureTargetBinding[nzImageType_Max+1];
	extern GLenum TextureTargetProxy[nzImageType_Max+1];
}

NAZARA_API extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
NAZARA_API extern PFNGLATTACHSHADERPROC             glAttachShader;
NAZARA_API extern PFNGLBEGINQUERYPROC               glBeginQuery;
NAZARA_API extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
NAZARA_API extern PFNGLBINDBUFFERPROC               glBindBuffer;
NAZARA_API extern PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer;
NAZARA_API extern PFNGLBINDFRAGDATALOCATIONPROC     glBindFragDataLocation;
NAZARA_API extern PFNGLBINDRENDERBUFFERPROC         glBindRenderbuffer;
NAZARA_API extern PFNGLBINDTEXTUREPROC              glBindTexture;
NAZARA_API extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
NAZARA_API extern PFNGLBLENDFUNCPROC                glBlendFunc;
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
NAZARA_API extern PFNGLDELETESHADERPROC             glDeleteShader;
NAZARA_API extern PFNGLDELETETEXTURESPROC           glDeleteTextures;
NAZARA_API extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
NAZARA_API extern PFNGLDEPTHFUNCPROC                glDepthFunc;
NAZARA_API extern PFNGLDEPTHMASKPROC                glDepthMask;
NAZARA_API extern PFNGLDISABLEPROC                  glDisable;
NAZARA_API extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
NAZARA_API extern PFNGLDRAWARRAYSPROC               glDrawArrays;
NAZARA_API extern PFNGLDRAWBUFFERPROC               glDrawBuffer;
NAZARA_API extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
NAZARA_API extern PFNGLDRAWELEMENTSPROC             glDrawElements;
NAZARA_API extern PFNGLENABLEPROC                   glEnable;
NAZARA_API extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
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
NAZARA_API extern PFNGLGENTEXTURESPROC              glGenTextures;
NAZARA_API extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
NAZARA_API extern PFNGLGETBOOLEANVPROC              glGetBooleanv;
NAZARA_API extern PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv;
NAZARA_API extern PFNGLGETDEBUGMESSAGELOGPROC       glGetDebugMessageLog;
NAZARA_API extern PFNGLGETERRORPROC                 glGetError;
NAZARA_API extern PFNGLGETFLOATVPROC                glGetFloatv;
NAZARA_API extern PFNGLGETINTEGERVPROC              glGetIntegerv;
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
NAZARA_API extern PFNGLISENABLEDPROC                glIsEnabled;
NAZARA_API extern PFNGLLINEWIDTHPROC                glLineWidth;
NAZARA_API extern PFNGLLINKPROGRAMPROC              glLinkProgram;
NAZARA_API extern PFNGLMAPBUFFERPROC                glMapBuffer;
NAZARA_API extern PFNGLMAPBUFFERRANGEPROC           glMapBufferRange;
NAZARA_API extern PFNGLPIXELSTOREIPROC              glPixelStorei;
NAZARA_API extern PFNGLPOINTSIZEPROC                glPointSize;
NAZARA_API extern PFNGLPOLYGONMODEPROC              glPolygonMode;
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
NAZARA_API extern PFNGLSCISSORPROC                  glScissor;
NAZARA_API extern PFNGLSHADERSOURCEPROC             glShaderSource;
NAZARA_API extern PFNGLSTENCILFUNCPROC              glStencilFunc;
NAZARA_API extern PFNGLSTENCILOPPROC                glStencilOp;
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
