// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANMULTIPLEFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANMULTIPLEFRAMEBUFFER_HPP

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanMultipleFramebuffer final : public VulkanFramebuffer
	{
		public:
			inline VulkanMultipleFramebuffer(Vk::Framebuffer* framebuffers, std::size_t count);
			VulkanMultipleFramebuffer(const VulkanMultipleFramebuffer&) = delete;
			VulkanMultipleFramebuffer(VulkanMultipleFramebuffer&&) noexcept = default;
			~VulkanMultipleFramebuffer() = default;

			inline const Vk::Framebuffer& GetFramebuffer(std::size_t index) const;
			inline std::size_t GetFramebufferCount() const;

			VulkanMultipleFramebuffer& operator=(const VulkanMultipleFramebuffer&) = delete;
			VulkanMultipleFramebuffer& operator=(VulkanMultipleFramebuffer&&) noexcept = default;

		private:
			std::vector<Vk::Framebuffer> m_framebuffers;
	};
}

#include <Nazara/VulkanRenderer/VulkanMultipleFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANMULTIPLEFRAMEBUFFER_HPP
