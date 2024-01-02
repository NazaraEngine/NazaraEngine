// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderPipelineLayout::OpenGLRenderPipelineLayout(RenderPipelineLayoutInfo layoutInfo) :
	m_maxDescriptorCount(0),
	m_layoutInfo(std::move(layoutInfo))
	{
		// Build binding mapping (vulkan-like set | binding => GL binding) and register max descriptor count
		unsigned int bindingIndex = 0;
		for (const auto& binding : m_layoutInfo.bindings)
		{
			for (UInt32 i = 0; i < binding.arraySize; ++i)
			{
				UInt64 bindingKey = UInt64(binding.setIndex) << 32 | UInt64(binding.bindingIndex + i);

				m_bindingMapping[bindingKey] = bindingIndex++;
			}

			m_maxDescriptorCount = std::max<std::size_t>(m_maxDescriptorCount, binding.bindingIndex + binding.arraySize);
		}
	}

	OpenGLRenderPipelineLayout::~OpenGLRenderPipelineLayout()
	{
		for (auto& pool : m_descriptorPools)
		{
			if (!pool.freeBindings.TestAll())
				NazaraWarning("Not all ShaderBinding have been released!");
		}
	}

	ShaderBindingPtr OpenGLRenderPipelineLayout::AllocateShaderBinding(UInt32 setIndex)
	{
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

	void OpenGLRenderPipelineLayout::UpdateDebugName(std::string_view /*name*/)
	{
		// No OpenGL object to name
	}

	auto OpenGLRenderPipelineLayout::AllocatePool() -> DescriptorPool&
	{
		constexpr UInt32 MaxSet = 128;

		DescriptorPool pool;
		pool.freeBindings.Resize(MaxSet, true);
		pool.storage = std::make_unique<DescriptorPool::BindingStorage[]>(MaxSet);
		pool.descriptors.resize(m_maxDescriptorCount * MaxSet);

		return m_descriptorPools.emplace_back(std::move(pool));
	}

	ShaderBindingPtr OpenGLRenderPipelineLayout::AllocateFromPool(std::size_t poolIndex, UInt32 /*setIndex*/)
	{
		//FIXME: Make use of set index to use less memory

		auto& pool = m_descriptorPools[poolIndex];

		std::size_t freeBindingId = pool.freeBindings.FindFirst();
		if (freeBindingId == pool.freeBindings.npos)
			return {}; //< No free binding in this pool

		pool.freeBindings.Reset(freeBindingId);

		OpenGLShaderBinding* freeBindingMemory = reinterpret_cast<OpenGLShaderBinding*>(&pool.storage[freeBindingId]);
		return ShaderBindingPtr(PlacementNew(freeBindingMemory, *this, poolIndex, freeBindingId));
	}

	auto OpenGLRenderPipelineLayout::GetSampledTextureDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex) -> SampledTextureDescriptor&
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));
		assert(descriptorIndex < m_maxDescriptorCount);

		return pool.descriptors[bindingIndex * m_maxDescriptorCount + descriptorIndex].emplace<SampledTextureDescriptor>();
	}

	auto OpenGLRenderPipelineLayout::GetStorageBufferDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex) -> StorageBufferDescriptor&
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));
		assert(descriptorIndex < m_maxDescriptorCount);

		return pool.descriptors[bindingIndex * m_maxDescriptorCount + descriptorIndex].emplace<StorageBufferDescriptor>();
	}

	auto OpenGLRenderPipelineLayout::GetTextureDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex) -> TextureDescriptor&
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));
		assert(descriptorIndex < m_maxDescriptorCount);

		return pool.descriptors[bindingIndex * m_maxDescriptorCount + descriptorIndex].emplace<TextureDescriptor>();
	}

	auto OpenGLRenderPipelineLayout::GetUniformBufferDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex) -> UniformBufferDescriptor&
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));
		assert(descriptorIndex < m_maxDescriptorCount);

		return pool.descriptors[bindingIndex * m_maxDescriptorCount + descriptorIndex].emplace<UniformBufferDescriptor>();
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
