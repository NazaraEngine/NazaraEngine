// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline const Vk::DescriptorSetLayout& VulkanRenderPipelineLayout::GetDescriptorSetLayout() const
	{
		return m_descriptorSetLayout;
	}

	inline const Vk::PipelineLayout& VulkanRenderPipelineLayout::GetPipelineLayout() const
	{
		return m_pipelineLayout;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
