// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSINGLEFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANSINGLEFRAMEBUFFER_HPP

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>
#include <memory>

namespace Nz
{
	class RenderPass;
	class Texture;

	class NAZARA_VULKANRENDERER_API VulkanSingleFramebuffer final : public VulkanFramebuffer
	{
		public:
			VulkanSingleFramebuffer(Vk::Device& device, unsigned int width, unsigned int height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments);
			VulkanSingleFramebuffer(const VulkanSingleFramebuffer&) = delete;
			VulkanSingleFramebuffer(VulkanSingleFramebuffer&&) = delete;
			~VulkanSingleFramebuffer() = default;

			inline Vk::Framebuffer& GetFramebuffer();
			inline const Vk::Framebuffer& GetFramebuffer() const;

			VulkanSingleFramebuffer& operator=(const VulkanSingleFramebuffer&) = delete;
			VulkanSingleFramebuffer& operator=(VulkanSingleFramebuffer&&) = delete;

		private:
			Vk::Framebuffer m_framebuffer;
	};
}

#include <Nazara/VulkanRenderer/VulkanSingleFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSINGLEFRAMEBUFFER_HPP
