// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanDescriptorSetLayoutCache.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderPipelineLayout::~VulkanRenderPipelineLayout()
	{
		for (auto& pool : m_descriptorPools)
		{
			if (!pool.freeBindings.TestAll())
				NazaraWarning("Not all ShaderBinding have been released!");
		}
	}

	ShaderBindingPtr VulkanRenderPipelineLayout::AllocateShaderBinding(UInt32 setIndex)
	{
		NazaraAssert(setIndex < m_descriptorSetLayouts.size(), "invalid set index");

		for (std::size_t i = 0; i < m_descriptorPools.size(); ++i)
		{
			ShaderBindingPtr bindingPtr = AllocateFromPool(i, setIndex);
			if (!bindingPtr)
				continue;

			return bindingPtr;
		}

		// No allocation could be made, time to allocate a new pool
		std::size_t newPoolIndex = m_descriptorPools.size();
		AllocatePool();

		ShaderBindingPtr bindingPtr = AllocateFromPool(newPoolIndex, setIndex);
		if (!bindingPtr)
			throw std::runtime_error("Failed to allocate shader binding");

		return bindingPtr;
	}

	bool VulkanRenderPipelineLayout::Create(Vk::Device& device, RenderPipelineLayoutInfo layoutInfo)
	{
		m_device = &device;
		m_layoutInfo = std::move(layoutInfo);

		UInt32 setCount = 0;
		for (const auto& bindingInfo : m_layoutInfo.bindings)
			setCount = std::max(setCount, bindingInfo.setIndex + 1);

		//TODO: Assert set count before stack allocation

		StackArray<VulkanDescriptorSetLayoutInfo> setLayoutInfo = NazaraStackArray(VulkanDescriptorSetLayoutInfo, setCount);
		StackArray<VkDescriptorSetLayout> setLayouts = NazaraStackArrayNoInit(VkDescriptorSetLayout, setCount);

		m_descriptorSetLayouts.resize(setCount);
		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VulkanDescriptorSetLayoutInfo& descriptorSetLayoutInfo = setLayoutInfo[bindingInfo.setIndex];

			VkDescriptorSetLayoutBinding& layoutBinding = descriptorSetLayoutInfo.bindings.emplace_back();
			layoutBinding.binding = bindingInfo.bindingIndex;
			layoutBinding.descriptorCount = 1U;
			layoutBinding.descriptorType = ToVulkan(bindingInfo.type);
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.stageFlags = ToVulkan(bindingInfo.shaderStageFlags);
		}

		for (UInt32 i = 0; i < setCount; ++i)
		{
			m_descriptorSetLayouts[i] = &device.GetDescriptorSetLayoutCache().Get(setLayoutInfo[i]);
			setLayouts[i] = *m_descriptorSetLayouts[i];
		}

		if (!m_pipelineLayout.Create(*m_device, UInt32(setLayouts.size()), setLayouts.data()))
			return false;

		return true;
	}

	auto VulkanRenderPipelineLayout::AllocatePool() -> DescriptorPool&
	{
		StackVector<VkDescriptorPoolSize> poolSizes = NazaraStackVector(VkDescriptorPoolSize, m_layoutInfo.bindings.size());

		constexpr UInt32 MaxSet = 128;

		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VkDescriptorPoolSize& poolSize = poolSizes.emplace_back();
			poolSize.descriptorCount = MaxSet;
			poolSize.type = ToVulkan(bindingInfo.type);
		}

		DescriptorPool pool;
		pool.descriptorPool = std::make_unique<Vk::DescriptorPool>();

		if (!pool.descriptorPool->Create(*m_device, MaxSet, UInt32(poolSizes.size()), poolSizes.data(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
			throw std::runtime_error("failed to allocate new descriptor pool: " + TranslateVulkanError(pool.descriptorPool->GetLastErrorCode()));

		pool.freeBindings.Resize(MaxSet, true);
		pool.storage = std::make_unique<DescriptorPool::BindingStorage[]>(MaxSet);

		return m_descriptorPools.emplace_back(std::move(pool));
	}

	ShaderBindingPtr VulkanRenderPipelineLayout::AllocateFromPool(std::size_t poolIndex, UInt32 setIndex)
	{
		auto& pool = m_descriptorPools[poolIndex];

		std::size_t freeBindingId = pool.freeBindings.FindFirst();
		if (freeBindingId == pool.freeBindings.npos)
			return {}; //< No free binding in this pool

		Vk::DescriptorSet descriptorSet = pool.descriptorPool->AllocateDescriptorSet(*m_descriptorSetLayouts[setIndex]);
		if (!descriptorSet)
		{
			NazaraWarning("Failed to allocate descriptor set: " + TranslateVulkanError(pool.descriptorPool->GetLastErrorCode()));
			return {};
		}

		pool.freeBindings.Reset(freeBindingId);

		VulkanShaderBinding* freeBindingMemory = reinterpret_cast<VulkanShaderBinding*>(&pool.storage[freeBindingId]);
		return ShaderBindingPtr(PlacementNew(freeBindingMemory, *this, poolIndex, freeBindingId, std::move(descriptorSet)));
	}

	void VulkanRenderPipelineLayout::Release(ShaderBinding& binding)
	{
		VulkanShaderBinding& vulkanBinding = static_cast<VulkanShaderBinding&>(binding);

		std::size_t poolIndex = vulkanBinding.GetPoolIndex();
		std::size_t bindingIndex = vulkanBinding.GetBindingIndex();

		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));

		VulkanShaderBinding* bindingMemory = reinterpret_cast<VulkanShaderBinding*>(&pool.storage[bindingIndex]);
		PlacementDestroy(bindingMemory);

		pool.freeBindings.Set(bindingIndex);

		// Try to free pool if it's one of the last one
		if (poolIndex >= m_descriptorPools.size() - 1 && poolIndex <= m_descriptorPools.size())
			TryToShrink();
	}
}
