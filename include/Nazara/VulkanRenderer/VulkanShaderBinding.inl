// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanShaderBinding::VulkanShaderBinding(VulkanRenderPipelineLayout& owner, std::size_t poolIndex, std::size_t bindingIndex, Vk::DescriptorSet descriptorSet) :
	m_descriptorSet(std::move(descriptorSet)),
	m_owner(owner),
	m_bindingIndex(bindingIndex),
	m_poolIndex(poolIndex)
	{
	}

	inline std::size_t VulkanShaderBinding::GetBindingIndex() const
	{
		return m_bindingIndex;
	}

	inline std::size_t VulkanShaderBinding::GetPoolIndex() const
	{
		return m_poolIndex;
	}

	inline Vk::DescriptorSet& VulkanShaderBinding::GetDescriptorSet()
	{
		return m_descriptorSet;
	}

	inline VulkanRenderPipelineLayout& VulkanShaderBinding::GetOwner()
	{
		return m_owner;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
