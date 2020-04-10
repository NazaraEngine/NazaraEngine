// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSINGLEFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANSINGLEFRAMEBUFFER_HPP

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanSingleFramebuffer final : public VulkanFramebuffer
	{
		public:
			inline VulkanSingleFramebuffer(Vk::Framebuffer renderPass);
			VulkanSingleFramebuffer(const VulkanSingleFramebuffer&) = delete;
			VulkanSingleFramebuffer(VulkanSingleFramebuffer&&) noexcept = default;
			~VulkanSingleFramebuffer() = default;

			inline Vk::Framebuffer& GetFramebuffer();
			inline const Vk::Framebuffer& GetFramebuffer() const;

			VulkanSingleFramebuffer& operator=(const VulkanSingleFramebuffer&) = delete;
			VulkanSingleFramebuffer& operator=(VulkanSingleFramebuffer&&) noexcept = default;

		private:
			Vk::Framebuffer m_framebuffer;
	};
}

#include <Nazara/VulkanRenderer/VulkanSingleFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSINGLEFRAMEBUFFER_HPP
