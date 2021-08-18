// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILS_VULKAN_HPP
#define NAZARA_UTILS_VULKAN_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <optional>
#include <string>

namespace Nz
{
	inline std::optional<PixelFormat> FromVulkan(VkFormat format);

	inline VkAttachmentLoadOp ToVulkan(AttachmentLoadOp loadOp);
	inline VkAttachmentStoreOp ToVulkan(AttachmentStoreOp storeOp);
	inline VkBufferUsageFlags ToVulkan(BufferType bufferType);
	inline VkFormat ToVulkan(ComponentType componentType);
	inline VkCullModeFlagBits ToVulkan(FaceSide faceSide);
	inline VkPolygonMode ToVulkan(FaceFilling faceFilling);
	inline VkFrontFace ToVulkan(FrontFace frontFace);
	inline VkAccessFlagBits ToVulkan(MemoryAccess memoryAccess);
	inline VkAccessFlags ToVulkan(MemoryAccessFlags memoryAccessFlags);
	inline VkPipelineStageFlagBits ToVulkan(PipelineStage pipelineStage);
	inline VkPipelineStageFlags ToVulkan(PipelineStageFlags pipelineStages);
	inline VkFormat ToVulkan(PixelFormat pixelFormat);
	inline VkPrimitiveTopology ToVulkan(PrimitiveMode primitiveMode);
	inline VkCompareOp ToVulkan(RendererComparison comparison);
	inline VkFilter ToVulkan(SamplerFilter samplerFilter);
	inline VkSamplerMipmapMode ToVulkan(SamplerMipmapMode samplerMipmap);
	inline VkSamplerAddressMode ToVulkan(SamplerWrap samplerWrap);
	inline VkDescriptorType ToVulkan(ShaderBindingType bindingType);
	inline VkShaderStageFlagBits ToVulkan(ShaderStageType stageType);
	inline VkShaderStageFlags ToVulkan(ShaderStageTypeFlags stageType);
	inline VkStencilOp ToVulkan(StencilOperation stencilOp);
	inline VkImageLayout ToVulkan(TextureLayout textureLayout);
	inline VkImageUsageFlagBits ToVulkan(TextureUsage textureLayout);
	inline VkImageUsageFlags ToVulkan(TextureUsageFlags textureLayout);
	inline VkVertexInputRate ToVulkan(VertexInputRate inputRate);

	NAZARA_VULKANRENDERER_API std::string TranslateVulkanError(VkResult code);
}

#include <Nazara/VulkanRenderer/Utils.inl>

#endif // NAZARA_UTILS_VULKAN_HPP
