// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	Vk::DescriptorSet VulkanRenderPipelineLayout::AllocateDescriptorSet()
	{
		// TODO: Watch descriptor set count for each pool
		for (auto& pool : m_descriptorPools)
		{
			Vk::DescriptorSet descriptorSet = pool.descriptorPool.AllocateDescriptorSet(m_descriptorSetLayout);
			if (descriptorSet)
				return descriptorSet;
		}

		// Allocate a new descriptor pool
		StackVector<VkDescriptorPoolSize> poolSizes = NazaraStackVector(VkDescriptorPoolSize, m_layoutInfo.bindings.size());

		constexpr UInt32 MaxSet = 100;

		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VkDescriptorPoolSize& poolSize = poolSizes.emplace_back();
			poolSize.descriptorCount = MaxSet;
			poolSize.type = ToVulkan(bindingInfo.type);
		}

		DescriptorPool pool;
		if (!pool.descriptorPool.Create(*m_device, MaxSet, UInt32(poolSizes.size()), poolSizes.data(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
			return {};

		return m_descriptorPools.emplace_back(std::move(pool)).descriptorPool.AllocateDescriptorSet(m_descriptorSetLayout);
	}

	bool VulkanRenderPipelineLayout::Create(Vk::Device& device, RenderPipelineLayoutInfo layoutInfo)
	{
		m_device = &device;
		m_layoutInfo = std::move(layoutInfo);

		StackVector<VkDescriptorSetLayoutBinding> layoutBindings = NazaraStackVector(VkDescriptorSetLayoutBinding, m_layoutInfo.bindings.size());

		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.binding = bindingInfo.index;
			layoutBinding.descriptorCount = 1U;
			layoutBinding.descriptorType = ToVulkan(bindingInfo.type);
			layoutBinding.stageFlags = ToVulkan(bindingInfo.shaderStageFlags);
		}

		if (!m_descriptorSetLayout.Create(*m_device, UInt32(layoutBindings.size()), layoutBindings.data()))
			return false;

		if (!m_pipelineLayout.Create(*m_device, m_descriptorSetLayout))
			return false;

		return true;
	}
}
