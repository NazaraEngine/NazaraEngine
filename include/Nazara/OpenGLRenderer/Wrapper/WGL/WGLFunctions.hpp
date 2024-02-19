// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLFUNCTIONS_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLFUNCTIONS_HPP

#undef WIN32_LEAN_AND_MEAN //< Redefined by wgl.h header (ty Khronos)

// no include reordering

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
	\
	extBegin(WGL_EXT_swap_control) \
		extFunc(wglSwapIntervalEXT, PFNWGLSWAPINTERVALEXTPROC) \
	extEnd()

#define NAZARA_OPENGLRENDERER_FOREACH_GDI32_FUNC(func) \
	func(ChoosePixelFormat, PFNCHOOSEPIXELFORMATPROC) \
	func(DescribePixelFormat, PFNDESCRIBEPIXELFORMATPROC) \
	func(SetPixelFormat, PFNSETPIXELFORMATPROC) \
	func(SwapBuffers, PFNSWAPBUFFERSPROC) \

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLFUNCTIONS_HPP
