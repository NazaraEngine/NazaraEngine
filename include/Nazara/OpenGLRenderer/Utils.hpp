// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILS_OPENGL_HPP
#define NAZARA_UTILS_OPENGL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <string>

namespace Nz
{
	inline VkBufferUsageFlags ToOpenGL(BufferType bufferType);
	inline VkFormat ToOpenGL(ComponentType componentType);
	inline VkCullModeFlagBits ToOpenGL(FaceSide faceSide);
	inline VkPolygonMode ToOpenGL(FaceFilling faceFilling);
	inline VkPrimitiveTopology ToOpenGL(PrimitiveMode primitiveMode);
	inline VkCompareOp ToOpenGL(RendererComparison comparison);
	inline VkFilter ToOpenGL(SamplerFilter samplerFilter);
	inline VkSamplerMipmapMode ToOpenGL(SamplerMipmapMode samplerMipmap);
	inline VkSamplerAddressMode ToOpenGL(SamplerWrap samplerWrap);
	inline VkDescriptorType ToOpenGL(ShaderBindingType bindingType);
	inline VkShaderStageFlagBits ToOpenGL(ShaderStageType stageType);
	inline VkShaderStageFlags ToOpenGL(ShaderStageTypeFlags stageType);
	inline VkStencilOp ToOpenGL(StencilOperation stencilOp);
	inline VkVertexInputRate ToOpenGL(VertexInputRate inputRate);

	NAZARA_OPENGLRENDERER_API std::string TranslateOpenGLError(VkResult code);
}

#include <Nazara/OpenGLRenderer/Utils.inl>

#endif // NAZARA_UTILS_OPENGL_HPP
