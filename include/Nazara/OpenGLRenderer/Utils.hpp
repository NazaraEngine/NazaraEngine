// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_UTILS_HPP
#define NAZARA_OPENGLRENDERER_UTILS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <optional>
#include <string>

namespace Nz
{
	struct GLTextureFormat
	{
		GLint internalFormat;
		GLenum format;
		GLenum type;
		GLenum swizzleR;
		GLenum swizzleG;
		GLenum swizzleB;
		GLenum swizzleA;
	};

	inline std::optional<GLTextureFormat> DescribeTextureFormat(PixelFormat pixelFormat);

	inline GLenum ToOpenGL(BlendEquation blendEquation);
	inline GLenum ToOpenGL(BlendFunc blendFunc);
	inline GLenum ToOpenGL(FaceFilling filling);
	inline GLenum ToOpenGL(FaceSide side);
	inline GLenum ToOpenGL(FrontFace face);
	inline GLenum ToOpenGL(IndexType indexType);
	inline GLenum ToOpenGL(PrimitiveMode primitiveMode);
	inline GLenum ToOpenGL(SamplerFilter filter);
	inline GLenum ToOpenGL(SamplerFilter minFilter, SamplerMipmapMode mipmapFilter);
	inline GLenum ToOpenGL(SamplerWrap wrapMode);
	inline GLenum ToOpenGL(ShaderStageType stageType);
	inline GLenum ToOpenGL(StencilOperation stencilOp);
	inline GLenum ToOpenGL(GL::BufferTarget bufferTarget);
	inline GLenum ToOpenGL(GL::TextureTarget bufferTarget);

	NAZARA_OPENGLRENDERER_API std::string TranslateOpenGLError(GLenum code);
}

#include <Nazara/OpenGLRenderer/Utils.inl>

#endif // NAZARA_OPENGLRENDERER_UTILS_HPP
