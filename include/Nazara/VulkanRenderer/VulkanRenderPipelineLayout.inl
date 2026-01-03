// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline Vk::Device* VulkanRenderPipelineLayout::GetDevice() const
	{
		return m_device.Get();
	}

	inline const Vk::PipelineLayout& VulkanRenderPipelineLayout::GetPipelineLayout() const
	{
		return m_pipelineLayout;
	}

	inline void VulkanRenderPipelineLayout::TryToShrink()
	{
		std::size_t poolCount = m_descriptorPools.size();
		if (poolCount >= 2 && m_descriptorPools.back().freeBindings.TestAll())
		{
			for (std::size_t i = poolCount - 1; i > 0; --i)
			{
				if (!m_descriptorPools[i].freeBindings.TestAll())
					break;

				poolCount--;
			}

			m_descriptorPools.resize(poolCount);
		}
	}
}
