// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanSingleFramebuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanSingleFramebuffer::VulkanSingleFramebuffer(Vk::Framebuffer framebuffer) :
	VulkanFramebuffer(Type::Single),
	m_framebuffer(std::move(framebuffer))
	{
	}

	inline Vk::Framebuffer& VulkanSingleFramebuffer::GetFramebuffer()
	{
		return m_framebuffer;
	}

	inline const Vk::Framebuffer& VulkanSingleFramebuffer::GetFramebuffer() const
	{
		return m_framebuffer;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
