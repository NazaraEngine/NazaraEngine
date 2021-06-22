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
			inline VulkanWindowFramebuffer(Vk::Framebuffer* framebuffers, std::size_t count);
			VulkanWindowFramebuffer(const VulkanWindowFramebuffer&) = delete;
			VulkanWindowFramebuffer(VulkanWindowFramebuffer&&) noexcept = default;
			~VulkanWindowFramebuffer() = default;

			inline const Vk::Framebuffer& GetFramebuffer(std::size_t index) const;
			inline std::size_t GetFramebufferCount() const;

			VulkanWindowFramebuffer& operator=(const VulkanWindowFramebuffer&) = delete;
			VulkanWindowFramebuffer& operator=(VulkanWindowFramebuffer&&) noexcept = default;

		private:
			std::vector<Vk::Framebuffer> m_framebuffers;
	};
}

#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANWINDOWFRAMEBUFFER_HPP
