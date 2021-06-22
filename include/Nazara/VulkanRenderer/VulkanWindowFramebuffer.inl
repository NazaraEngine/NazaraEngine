// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanWindowFramebuffer::VulkanWindowFramebuffer(Vk::Framebuffer* framebuffers, std::size_t count) :
	VulkanFramebuffer(FramebufferType::Window)
	{
		m_framebuffers.reserve(count);
		for (std::size_t i = 0; i < count; ++i)
			m_framebuffers.push_back(std::move(framebuffers[i]));
	}

	inline const Vk::Framebuffer& Nz::VulkanWindowFramebuffer::GetFramebuffer(std::size_t index) const
	{
		assert(index < m_framebuffers.size());
		return m_framebuffers[index];
	}

	inline std::size_t VulkanWindowFramebuffer::GetFramebufferCount() const
	{
		return m_framebuffers.size();
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
