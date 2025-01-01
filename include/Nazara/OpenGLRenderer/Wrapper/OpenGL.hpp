// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_OPENGL_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_OPENGL_HPP

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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_OPENGL_HPP
