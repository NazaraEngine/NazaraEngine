// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>

namespace Nz
{
	void VulkanCommandBuffer::UpdateDebugName(std::string_view name)
	{
		return m_commandBuffer->UpdateDebugName(name);
	}

	void VulkanCommandBuffer::Release()
	{
		m_owner.Release(*this);
	}
}
