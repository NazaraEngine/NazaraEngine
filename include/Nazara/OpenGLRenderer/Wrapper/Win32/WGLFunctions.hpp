// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef	NAZARA_OPENGLRENDERER_WGLFUNCTIONS_HPP
#define NAZARA_OPENGLRENDERER_WGLFUNCTIONS_HPP

#include <GLES3/gl3.h>
#include <GL/wgl.h>
#include <GL/wglext.h>

#define NAZARA_OPENGLRENDERER_FOREACH_WGL_FUNC(func, extBegin, extEnd, extFunc) \
	func(wglCreateContext, PFNWGLCREATECONTEXTPROC) \
	func(wglDeleteContext, PFNWGLDELETECONTEXTPROC) \
	func(wglGetCurrentContext, PFNWGLGETCURRENTCONTEXTPROC) \
	func(wglGetProcAddress, PFNWGLGETPROCADDRESSPROC) \
	func(wglMakeCurrent, PFNWGLMAKECURRENTPROC) \
	func(wglShareLists, PFNWGLSHARELISTSPROC) \
	\
	extBegin(WGL_ARB_create_context) \
		extFunc(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC) \
	extEnd() \
	\
	extBegin(WGL_ARB_pixel_format) \
		extFunc(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC) \
	extEnd() \
	\
	extBegin(WGL_EXT_pixel_format) \
		extFunc(wglChoosePixelFormatEXT, PFNWGLCHOOSEPIXELFORMATEXTPROC) \
	extEnd() \
	\
	extBegin(WGL_ARB_extensions_string) \
		extFunc(wglGetExtensionsStringARB, PFNWGLGETEXTENSIONSSTRINGARBPROC) \
	extEnd() \
	\
	extBegin(WGL_EXT_extensions_string) \
		extFunc(wglGetExtensionsStringEXT, PFNWGLGETEXTENSIONSSTRINGEXTPROC) \
	extEnd() \

#define NAZARA_OPENGLRENDERER_FOREACH_GDI32_FUNC(func) \
	func(ChoosePixelFormat, PFNCHOOSEPIXELFORMATPROC) \
	func(DescribePixelFormat, PFNDESCRIBEPIXELFORMATPROC) \
	func(SetPixelFormat, PFNSETPIXELFORMATPROC) \
	func(SwapBuffers, PFNSWAPBUFFERSPROC) \

#endif
