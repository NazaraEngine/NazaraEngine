// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANWINDOWFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANWINDOWFRAMEBUFFER_HPP

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanWindowFramebuffer final : public VulkanFramebuffer
	{
		public:
			inline VulkanWindowFramebuffer(Vk::Framebuffer framebuffer);
			VulkanWindowFramebuffer(const VulkanWindowFramebuffer&) = delete;
			VulkanWindowFramebuffer(VulkanWindowFramebuffer&&) noexcept = default;
			~VulkanWindowFramebuffer() = default;

			inline const Vk::Framebuffer& GetFramebuffer() const override;

			VulkanWindowFramebuffer& operator=(const VulkanWindowFramebuffer&) = delete;
			VulkanWindowFramebuffer& operator=(VulkanWindowFramebuffer&&) noexcept = default;

		private:
			Vk::Framebuffer m_framebuffer;
	};
}

#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANWINDOWFRAMEBUFFER_HPP
