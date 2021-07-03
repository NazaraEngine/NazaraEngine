// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanWindowFramebuffer::VulkanWindowFramebuffer(Vk::Framebuffer framebuffer) :
	VulkanFramebuffer(FramebufferType::Window),
	m_framebuffer(std::move(framebuffer))
	{
	}

	inline const Vk::Framebuffer& Nz::VulkanWindowFramebuffer::GetFramebuffer() const
	{
		return m_framebuffer;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
