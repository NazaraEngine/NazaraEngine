// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef	NAZARA_OPENGLRENDERER_EGLFUNCTIONS_HPP
#define NAZARA_OPENGLRENDERER_EGLFUNCTIONS_HPP

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define NAZARA_OPENGLRENDERER_FOREACH_EGL_FUNC(func, extBegin, extEnd, extFunc) \
	func(eglBindAPI, PFNEGLBINDAPIPROC) \
	func(eglChooseConfig, PFNEGLCHOOSECONFIGPROC) \
	func(eglCreateContext, PFNEGLCREATECONTEXTPROC) \
	func(eglCreatePbufferSurface, PFNEGLCREATEPBUFFERSURFACEPROC) \
	func(eglCreateWindowSurface, PFNEGLCREATEWINDOWSURFACEPROC) \
	func(eglDestroyContext, PFNEGLDESTROYCONTEXTPROC) \
	func(eglDestroySurface, PFNEGLDESTROYSURFACEPROC) \
	func(eglGetDisplay, PFNEGLGETDISPLAYPROC) \
	func(eglGetError, PFNEGLGETERRORPROC) \
	func(eglGetProcAddress, PFNEGLGETPROCADDRESSPROC) \
	func(eglInitialize, PFNEGLINITIALIZEPROC) \
	func(eglMakeCurrent, PFNEGLMAKECURRENTPROC) \
	func(eglQueryString, PFNEGLQUERYSTRINGPROC) \
	func(eglSwapBuffers, PFNEGLSWAPBUFFERSPROC) \
	func(eglTerminate, PFNEGLTERMINATEPROC)

#endif
