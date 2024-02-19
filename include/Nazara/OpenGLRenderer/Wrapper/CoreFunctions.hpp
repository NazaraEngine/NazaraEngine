// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_COREFUNCTIONS_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_COREFUNCTIONS_HPP

// no include reordering

#define GL_GLES_PROTOTYPES 0
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

// Define some OpenGL (not ES) defines/function types
#define GL_POINT                           0x1B00
#define GL_LINE                            0x1B01
#define GL_FILL                            0x1B02
#define GL_SHADER_BINARY_FORMAT_SPIR_V_ARB 0x9551
#define GL_SPIR_V_BINARY_ARB               0x9552
#define GL_TEXTURE_CUBE_MAP_SEAMLESS       0x884F
typedef void (GL_APIENTRYP PFNGLDRAWBUFFERPROC) (GLenum buf);
typedef void (GL_APIENTRYP PFNGLPOLYGONMODEPROC) (GLenum face, GLenum mode);

// Depth clamp (OpenGL 3.2)
#define GL_DEPTH_CLAMP                     0x864F

// Texture views (OpenGL 4.3)
typedef void (GL_APIENTRYP PFNGLTEXTUREVIEWPROC) (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);

// Clip control (OpenGL 4.5)
#define GL_LOWER_LEFT                      0x8CA1
#define GL_UPPER_LEFT                      0x8CA2
#define GL_NEGATIVE_ONE_TO_ONE             0x935E
#define GL_ZERO_TO_ONE                     0x935F
#define GL_CLIP_ORIGIN                     0x935C
#define GL_CLIP_DEPTH_MODE                 0x935D
typedef void (GL_APIENTRYP PFNGLCLIPCONTROLPROC) (GLenum origin, GLenum depth);

// SPIR-V shaders (OpenGL 4.6)
typedef void (GL_APIENTRYP PFNGLSPECIALIZESHADERPROC) (GLuint shader, const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue);

/*** Functions ***/

// OpenGL core
#define NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(cb, extCb) \
	cb(glActiveTexture, PFNGLACTIVETEXTUREPROC) \
	cb(glAttachShader, PFNGLATTACHSHADERPROC) \
	cb(glBeginQuery, PFNGLBEGINQUERYPROC) \
	cb(glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC) \
	cb(glBindBuffer, PFNGLBINDBUFFERPROC) \
	cb(glBindBufferRange, PFNGLBINDBUFFERRANGEPROC) \
	cb(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC) \
	cb(glBindRenderbuffer, PFNGLBINDRENDERBUFFERPROC) \
	cb(glBindSampler, PFNGLBINDSAMPLERPROC) \
	cb(glBindTexture, PFNGLBINDTEXTUREPROC) \
	cb(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC) \
	cb(glBlendEquationSeparate, PFNGLBLENDEQUATIONSEPARATEPROC) \
	cb(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC) \
	cb(glBlitFramebuffer, PFNGLBLITFRAMEBUFFERPROC) \
	cb(glBufferData, PFNGLBUFFERDATAPROC) \
	cb(glBufferSubData, PFNGLBUFFERSUBDATAPROC) \
	cb(glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC) \
	cb(glClear, PFNGLCLEARPROC) \
	cb(glClearBufferfi, PFNGLCLEARBUFFERFIPROC) \
	cb(glClearBufferfv, PFNGLCLEARBUFFERFVPROC) \
	cb(glClearBufferuiv, PFNGLCLEARBUFFERUIVPROC) \
	cb(glClearColor, PFNGLCLEARCOLORPROC) \
	cb(glClearDepthf, PFNGLCLEARDEPTHFPROC) \
	cb(glClearStencil, PFNGLCLEARSTENCILPROC) \
	cb(glColorMask, PFNGLCOLORMASKPROC) \
	cb(glCompileShader, PFNGLCOMPILESHADERPROC) \
	cb(glCompressedTexSubImage2D, PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) \
	cb(glCompressedTexSubImage3D, PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) \
	cb(glCopyBufferSubData, PFNGLCOPYBUFFERSUBDATAPROC) \
	cb(glCopyTexSubImage2D, PFNGLCOPYTEXSUBIMAGE2DPROC) \
	cb(glCreateProgram, PFNGLCREATEPROGRAMPROC) \
	cb(glCreateShader, PFNGLCREATESHADERPROC) \
	cb(glCullFace, PFNGLCULLFACEPROC) \
	cb(glDebugMessageInsert, PFNGLDEBUGMESSAGEINSERTPROC) \
	cb(glDeleteBuffers, PFNGLDELETEBUFFERSPROC) \
	cb(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC) \
	cb(glDeleteProgram, PFNGLDELETEPROGRAMPROC) \
	cb(glDeleteQueries, PFNGLDELETEQUERIESPROC) \
	cb(glDeleteRenderbuffers, PFNGLDELETERENDERBUFFERSPROC) \
	cb(glDeleteSamplers, PFNGLDELETESAMPLERSPROC) \
	cb(glDeleteShader, PFNGLDELETESHADERPROC) \
	cb(glDeleteTextures, PFNGLDELETETEXTURESPROC) \
	cb(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC) \
	cb(glDepthFunc, PFNGLDEPTHFUNCPROC) \
	cb(glDepthMask, PFNGLDEPTHMASKPROC) \
	cb(glDisable, PFNGLDISABLEPROC) \
	cb(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC) \
	cb(glDrawArrays, PFNGLDRAWARRAYSPROC) \
	cb(glDrawArraysInstanced, PFNGLDRAWARRAYSINSTANCEDPROC) \
	cb(glDrawBuffers, PFNGLDRAWBUFFERSPROC) \
	cb(glDrawElements, PFNGLDRAWELEMENTSPROC) \
	cb(glDrawElementsInstanced, PFNGLDRAWELEMENTSINSTANCEDPROC) \
	cb(glEnable, PFNGLENABLEPROC) \
	cb(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC) \
	cb(glEndQuery, PFNGLENDQUERYPROC) \
	cb(glFinish, PFNGLFINISHPROC) \
	cb(glFlush, PFNGLFLUSHPROC) \
	cb(glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC) \
	cb(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC) \
	cb(glFramebufferTextureLayer, PFNGLFRAMEBUFFERTEXTURELAYERPROC) \
	cb(glFrontFace, PFNGLFRONTFACEPROC) \
	cb(glGenBuffers, PFNGLGENBUFFERSPROC) \
	cb(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC) \
	cb(glGenQueries, PFNGLGENQUERIESPROC) \
	cb(glGenRenderbuffers, PFNGLGENRENDERBUFFERSPROC) \
	cb(glGenSamplers, PFNGLGENSAMPLERSPROC) \
	cb(glGenTextures, PFNGLGENTEXTURESPROC) \
	cb(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC) \
	cb(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC) \
	cb(glGetActiveUniform, PFNGLGETACTIVEUNIFORMPROC) \
	cb(glGetActiveUniformsiv, PFNGLGETACTIVEUNIFORMSIVPROC) \
	cb(glGetActiveUniformBlockiv, PFNGLGETACTIVEUNIFORMBLOCKIVPROC) \
	cb(glGetActiveUniformBlockName, PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) \
	cb(glGetBooleanv, PFNGLGETBOOLEANVPROC) \
	cb(glGetBufferParameteriv, PFNGLGETBUFFERPARAMETERIVPROC) \
	cb(glGetError, PFNGLGETERRORPROC) \
	cb(glGetFloatv, PFNGLGETFLOATVPROC) \
	cb(glGetInteger64i_v, PFNGLGETINTEGER64I_VPROC) \
	cb(glGetInteger64v, PFNGLGETINTEGER64VPROC) \
	cb(glGetIntegeri_v, PFNGLGETINTEGERI_VPROC) \
	cb(glGetIntegerv, PFNGLGETINTEGERVPROC) \
	cb(glGetProgramBinary, PFNGLGETPROGRAMBINARYPROC) \
	cb(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC) \
	cb(glGetProgramiv, PFNGLGETPROGRAMIVPROC) \
	cb(glGetQueryObjectuiv, PFNGLGETQUERYOBJECTUIVPROC) \
	cb(glGetQueryiv, PFNGLGETQUERYIVPROC) \
	cb(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC) \
	cb(glGetShaderSource, PFNGLGETSHADERSOURCEPROC) \
	cb(glGetShaderiv, PFNGLGETSHADERIVPROC) \
	cb(glGetString, PFNGLGETSTRINGPROC) \
	cb(glGetStringi, PFNGLGETSTRINGIPROC) \
	cb(glGetTexParameterfv, PFNGLGETTEXPARAMETERFVPROC) \
	cb(glGetTexParameteriv, PFNGLGETTEXPARAMETERIVPROC) \
	cb(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC) \
	cb(glGetUniformfv, PFNGLGETUNIFORMFVPROC) \
	cb(glGetUniformiv, PFNGLGETUNIFORMIVPROC) \
	cb(glGetUniformBlockIndex, PFNGLGETUNIFORMBLOCKINDEXPROC) \
	cb(glIsEnabled, PFNGLISENABLEDPROC) \
	cb(glInvalidateFramebuffer, PFNGLINVALIDATEFRAMEBUFFERPROC) \
	cb(glLineWidth, PFNGLLINEWIDTHPROC) \
	cb(glLinkProgram, PFNGLLINKPROGRAMPROC) \
	cb(glMapBufferRange, PFNGLMAPBUFFERRANGEPROC) \
	cb(glPixelStorei, PFNGLPIXELSTOREIPROC) \
	cb(glPolygonOffset, PFNGLPOLYGONOFFSETPROC) \
	cb(glProgramBinary, PFNGLPROGRAMBINARYPROC) \
	cb(glProgramParameteri, PFNGLPROGRAMPARAMETERIPROC) \
	cb(glReadPixels, PFNGLREADPIXELSPROC) \
	cb(glRenderbufferStorage, PFNGLRENDERBUFFERSTORAGEPROC) \
	cb(glSamplerParameterf, PFNGLSAMPLERPARAMETERFPROC) \
	cb(glSamplerParameterfv, PFNGLSAMPLERPARAMETERFVPROC) \
	cb(glSamplerParameteri, PFNGLSAMPLERPARAMETERIPROC) \
	cb(glSamplerParameteriv, PFNGLSAMPLERPARAMETERIVPROC) \
	cb(glScissor, PFNGLSCISSORPROC) \
	cb(glShaderBinary, PFNGLSHADERBINARYPROC) \
	cb(glShaderSource, PFNGLSHADERSOURCEPROC) \
	cb(glStencilFunc, PFNGLSTENCILFUNCPROC) \
	cb(glStencilFuncSeparate, PFNGLSTENCILFUNCSEPARATEPROC) \
	cb(glStencilOp, PFNGLSTENCILOPPROC) \
	cb(glStencilOpSeparate, PFNGLSTENCILOPSEPARATEPROC) \
	cb(glStencilMaskSeparate, PFNGLSTENCILMASKSEPARATEPROC) \
	cb(glTexImage2D, PFNGLTEXIMAGE2DPROC) \
	cb(glTexImage3D, PFNGLTEXIMAGE3DPROC) \
	cb(glTexParameterf, PFNGLTEXPARAMETERFPROC) \
	cb(glTexParameterfv, PFNGLTEXPARAMETERFVPROC) \
	cb(glTexParameteri, PFNGLTEXPARAMETERIPROC) \
	cb(glTexParameteriv, PFNGLTEXPARAMETERIVPROC) \
	cb(glTexStorage2D, PFNGLTEXSTORAGE2DPROC) \
	cb(glTexStorage3D, PFNGLTEXSTORAGE3DPROC) \
	cb(glTexSubImage2D, PFNGLTEXSUBIMAGE2DPROC) \
	cb(glTexSubImage3D, PFNGLTEXSUBIMAGE3DPROC) \
	cb(glUniform1f, PFNGLUNIFORM1FPROC) \
	cb(glUniform1fv, PFNGLUNIFORM1FVPROC) \
	cb(glUniform1i, PFNGLUNIFORM1IPROC) \
	cb(glUniform1iv, PFNGLUNIFORM1IVPROC) \
	cb(glUniform2fv, PFNGLUNIFORM2FVPROC) \
	cb(glUniform2iv, PFNGLUNIFORM2IVPROC) \
	cb(glUniform3fv, PFNGLUNIFORM3FVPROC) \
	cb(glUniform3iv, PFNGLUNIFORM3IVPROC) \
	cb(glUniform4fv, PFNGLUNIFORM4FVPROC) \
	cb(glUniform4iv, PFNGLUNIFORM4IVPROC) \
	cb(glUniformBlockBinding, PFNGLUNIFORMBLOCKBINDINGPROC) \
	cb(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC) \
	cb(glUnmapBuffer, PFNGLUNMAPBUFFERPROC) \
	cb(glUseProgram, PFNGLUSEPROGRAMPROC) \
	cb(glValidateProgram, PFNGLVALIDATEPROGRAMPROC) \
	cb(glVertexAttrib4f, PFNGLVERTEXATTRIB4FPROC) \
	cb(glVertexAttribDivisor, PFNGLVERTEXATTRIBDIVISORPROC) \
	cb(glVertexAttribIPointer, PFNGLVERTEXATTRIBIPOINTERPROC) \
	cb(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC) \
	cb(glViewport, PFNGLVIEWPORTPROC) \
	/* Core OpenGL (extension in OpenGL ES) */ \
	extCb(glDebugMessageControl, PFNGLDEBUGMESSAGECONTROLPROC) \
	extCb(glDrawBuffer, PFNGLDRAWBUFFERPROC) \
	extCb(glPolygonMode, PFNGLPOLYGONMODEPROC) \
	/* OpenGL 4.2 - OpenGL ES 3.1 */\
	extCb(glBindImageTexture, PFNGLBINDIMAGETEXTUREPROC) \
	extCb(glGetBooleani_v, PFNGLGETBOOLEANI_VPROC) \
	extCb(glMemoryBarrier, PFNGLMEMORYBARRIERPROC) \
	extCb(glMemoryBarrierByRegion, PFNGLMEMORYBARRIERBYREGIONPROC) \
	/* OpenGL 4.3 - OpenGL ES 3.1 */\
	extCb(glDispatchCompute, PFNGLDISPATCHCOMPUTEPROC) \
	/* OpenGL 4.3 - OpenGL ES 3.2 */\
	extCb(glCopyImageSubData, PFNGLCOPYIMAGESUBDATAPROC) \
	extCb(glDebugMessageCallback, PFNGLDEBUGMESSAGECALLBACKPROC) \
	extCb(glObjectLabel, PFNGLOBJECTLABELPROC) \
	extCb(glPopDebugGroup, PFNGLPOPDEBUGGROUPPROC) \
	extCb(glPushDebugGroup, PFNGLPUSHDEBUGGROUPPROC) \
	/* OpenGL 4.3 - GL_ARB_texture_view */ \
	extCb(glTextureView, PFNGLTEXTUREVIEWPROC) \
	/* OpenGL 4.5 - GL_ARB_clip_control/GL_EXT_clip_control */ \
	extCb(glClipControl, PFNGLCLIPCONTROLPROC) \
	/* OpenGL 4.6 - GL_ARB_spirv_extensions */\
	extCb(glSpecializeShader, PFNGLSPECIALIZESHADERPROC) \

#endif // NAZARA_OPENGLRENDERER_WRAPPER_COREFUNCTIONS_HPP
