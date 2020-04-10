// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanFramebuffer::VulkanFramebuffer(Type type) :
	m_type(type)
	{
	}

	inline auto VulkanFramebuffer::GetType() const -> Type
	{
		return m_type;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
