// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline Vk::Device* VulkanRenderPipelineLayout::GetDevice() const
	{
		return m_device.Get();
	}

	inline const Vk::DescriptorSetLayout& VulkanRenderPipelineLayout::GetDescriptorSetLayout() const
	{
		return m_descriptorSetLayout;
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
			for (std::size_t i = poolCount - 1; i > 0; ++i)
			{
				if (m_descriptorPools[i].freeBindings.TestAll())
					poolCount--;
			}

			m_descriptorPools.resize(poolCount);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
