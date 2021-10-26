// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	void VulkanCommandBuffer::Release()
	{
		m_owner.Release(*this);
	}
}
