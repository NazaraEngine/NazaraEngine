// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderPipelineLayout::~OpenGLRenderPipelineLayout()
	{
		for (auto& pool : m_descriptorPools)
		{
			if (!pool.freeBindings.TestAll())
				NazaraWarning("Not all ShaderBinding have been released!");
		}
	}

	ShaderBindingPtr OpenGLRenderPipelineLayout::AllocateShaderBinding()
	{
		for (std::size_t i = 0; i < m_descriptorPools.size(); ++i)
		{
			ShaderBindingPtr bindingPtr = AllocateFromPool(i);
			if (!bindingPtr)
				continue;

			return bindingPtr;
		}

		// No allocation could be made, time to allocate a new pool
		std::size_t newPoolIndex = m_descriptorPools.size();
		AllocatePool();

		ShaderBindingPtr bindingPtr = AllocateFromPool(newPoolIndex);
		if (!bindingPtr)
			throw std::runtime_error("Failed to allocate shader binding");

		return bindingPtr;
	}

	bool OpenGLRenderPipelineLayout::Create(Vk::Device& device, RenderPipelineLayoutInfo layoutInfo)
	{
		m_device = &device;
		m_layoutInfo = std::move(layoutInfo);

		StackVector<VkDescriptorSetLayoutBinding> layoutBindings = NazaraStackVector(VkDescriptorSetLayoutBinding, m_layoutInfo.bindings.size());

		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.binding = bindingInfo.index;
			layoutBinding.descriptorCount = 1U;
			layoutBinding.descriptorType = ToOpenGL(bindingInfo.type);
			layoutBinding.stageFlags = ToOpenGL(bindingInfo.shaderStageFlags);
		}

		if (!m_descriptorSetLayout.Create(*m_device, UInt32(layoutBindings.size()), layoutBindings.data()))
			return false;

		if (!m_pipelineLayout.Create(*m_device, m_descriptorSetLayout))
			return false;

		return true;
	}

	auto OpenGLRenderPipelineLayout::AllocatePool() -> DescriptorPool&
	{
		StackVector<VkDescriptorPoolSize> poolSizes = NazaraStackVector(VkDescriptorPoolSize, m_layoutInfo.bindings.size());

		constexpr UInt32 MaxSet = 128;

		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VkDescriptorPoolSize& poolSize = poolSizes.emplace_back();
			poolSize.descriptorCount = MaxSet;
			poolSize.type = ToOpenGL(bindingInfo.type);
		}

		DescriptorPool pool;
		if (!pool.descriptorPool.Create(*m_device, MaxSet, UInt32(poolSizes.size()), poolSizes.data(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
			throw std::runtime_error("Failed to allocate new descriptor pool: " + TranslateOpenGLError(pool.descriptorPool.GetLastErrorCode()));

		pool.freeBindings.Resize(MaxSet, true);
		pool.storage = std::make_unique<DescriptorPool::BindingStorage[]>(MaxSet);

		return m_descriptorPools.emplace_back(std::move(pool));
	}

	ShaderBindingPtr OpenGLRenderPipelineLayout::AllocateFromPool(std::size_t poolIndex)
	{
		auto& pool = m_descriptorPools[poolIndex];

		std::size_t freeBindingId = pool.freeBindings.FindFirst();
		if (freeBindingId == pool.freeBindings.npos)
			return {}; //< No free binding in this pool

		Vk::DescriptorSet descriptorSet = pool.descriptorPool.AllocateDescriptorSet(m_descriptorSetLayout);
		if (!descriptorSet)
		{
			NazaraWarning("Failed to allocate descriptor set: " + TranslateOpenGLError(pool.descriptorPool.GetLastErrorCode()));
			return {};
		}

		pool.freeBindings.Reset(freeBindingId);

		OpenGLShaderBinding* freeBindingMemory = reinterpret_cast<OpenGLShaderBinding*>(&pool.storage[freeBindingId]);
		return ShaderBindingPtr(PlacementNew(freeBindingMemory, *this, poolIndex, freeBindingId, std::move(descriptorSet)));
	}

	void OpenGLRenderPipelineLayout::Release(ShaderBinding& binding)
	{
		OpenGLShaderBinding& vulkanBinding = static_cast<OpenGLShaderBinding&>(binding);

		std::size_t poolIndex = vulkanBinding.GetPoolIndex();
		std::size_t bindingIndex = vulkanBinding.GetBindingIndex();

		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));

		OpenGLShaderBinding* bindingMemory = reinterpret_cast<OpenGLShaderBinding*>(&pool.storage[bindingIndex]);
		PlacementDestroy(bindingMemory);

		pool.freeBindings.Set(bindingIndex);

		// Try to free pool if it's one of the last one
		if (poolIndex >= m_descriptorPools.size() - 1 && poolIndex <= m_descriptorPools.size())
			TryToShrink();
	}
}

#endif
