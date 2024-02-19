// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline VulkanWindowFramebuffer::VulkanWindowFramebuffer(Vk::Framebuffer framebuffer) :
	VulkanFramebuffer(FramebufferType::Window),
	m_framebuffer(std::move(framebuffer))
	{
	}

	inline Vk::Framebuffer& VulkanWindowFramebuffer::GetFramebuffer()
	{
		return m_framebuffer;
	}

	inline const Vk::Framebuffer& VulkanWindowFramebuffer::GetFramebuffer() const
	{
		return m_framebuffer;
	}
}

