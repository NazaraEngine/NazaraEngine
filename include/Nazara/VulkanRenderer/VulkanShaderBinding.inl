// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanShaderBinding::VulkanShaderBinding(VulkanRenderPipelineLayout& owner, Vk::DescriptorSet descriptorSet) :
	m_descriptorSet(std::move(descriptorSet)),
	m_owner(owner)
	{
	}

	inline Vk::DescriptorSet& VulkanShaderBinding::GetDescriptorSet()
	{
		return m_descriptorSet;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
