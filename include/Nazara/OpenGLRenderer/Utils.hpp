// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_UTILS_HPP
#define NAZARA_OPENGLRENDERER_UTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <NZSL/Enums.hpp>
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

		inline bool HasSwizzle() const;
	};

	inline std::optional<GLTextureFormat> DescribeTextureFormat(PixelFormat pixelFormat);

	inline GLenum ToOpenGL(BlendEquation blendEquation);
	inline GLenum ToOpenGL(BlendFunc blendFunc);
	inline GLenum ToOpenGL(FaceCulling side);
	inline GLenum ToOpenGL(FaceFilling filling);
	inline GLenum ToOpenGL(FrontFace face);
	inline GLenum ToOpenGL(IndexType indexType);
	inline GLenum ToOpenGL(PrimitiveMode primitiveMode);
	inline GLenum ToOpenGL(SamplerFilter filter);
	inline GLenum ToOpenGL(SamplerFilter minFilter, SamplerMipmapMode mipmapFilter);
	inline GLenum ToOpenGL(SamplerWrap wrapMode);
	inline GLenum ToOpenGL(nzsl::ShaderStageType stageType);
	inline GLenum ToOpenGL(StencilOperation stencilOp);
	inline GLenum ToOpenGL(TextureAccess textureAccess);
	inline GLenum ToOpenGL(GL::BufferTarget bufferTarget);
	inline GLenum ToOpenGL(GL::TextureTarget bufferTarget);

	NAZARA_OPENGLRENDERER_API std::string TranslateOpenGLError(GLenum code);
}

#include <Nazara/OpenGLRenderer/Utils.inl>

#endif // NAZARA_OPENGLRENDERER_UTILS_HPP
