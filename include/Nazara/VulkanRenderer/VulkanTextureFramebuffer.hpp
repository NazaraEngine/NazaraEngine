// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANTEXTUREFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANTEXTUREFRAMEBUFFER_HPP

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>
#include <memory>

namespace Nz
{
	class RenderPass;
	class Texture;

	class NAZARA_VULKANRENDERER_API VulkanTextureFramebuffer final : public VulkanFramebuffer
	{
		public:
			VulkanTextureFramebuffer(Vk::Device& device, unsigned int width, unsigned int height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments);
			VulkanTextureFramebuffer(const VulkanTextureFramebuffer&) = delete;
			VulkanTextureFramebuffer(VulkanTextureFramebuffer&&) = delete;
			~VulkanTextureFramebuffer() = default;

			inline const Vk::Framebuffer& GetFramebuffer() const override;

			VulkanTextureFramebuffer& operator=(const VulkanTextureFramebuffer&) = delete;
			VulkanTextureFramebuffer& operator=(VulkanTextureFramebuffer&&) = delete;

		private:
			Vk::Framebuffer m_framebuffer;
	};
}

#include <Nazara/VulkanRenderer/VulkanTextureFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANTEXTUREFRAMEBUFFER_HPP
