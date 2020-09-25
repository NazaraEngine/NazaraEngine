// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
	OpenGLRenderPipelineLayout::OpenGLRenderPipelineLayout(RenderPipelineLayoutInfo layoutInfo) :
	m_textureDescriptorCount(0),
	m_uniformBufferDescriptorCount(0),
	m_layoutInfo(std::move(layoutInfo))
	{
		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			switch (bindingInfo.type)
			{
				case ShaderBindingType::Texture:
					m_textureDescriptorCount++;
					break;

				case ShaderBindingType::UniformBuffer:
					m_uniformBufferDescriptorCount++;
					break;

				default:
					throw std::runtime_error("unknown binding type 0x" + NumberToString(UnderlyingCast(bindingInfo.type), 16));
			}
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

	auto OpenGLRenderPipelineLayout::AllocatePool() -> DescriptorPool&
	{
		constexpr UInt32 MaxSet = 128;

		DescriptorPool pool;
		pool.freeBindings.Resize(MaxSet, true);
		pool.storage = std::make_unique<DescriptorPool::BindingStorage[]>(MaxSet);
		pool.textureDescriptor.resize(m_textureDescriptorCount * MaxSet);
		pool.uniformBufferDescriptor.resize(m_uniformBufferDescriptorCount * MaxSet);

		return m_descriptorPools.emplace_back(std::move(pool));
	}

	ShaderBindingPtr OpenGLRenderPipelineLayout::AllocateFromPool(std::size_t poolIndex)
	{
		auto& pool = m_descriptorPools[poolIndex];

		std::size_t freeBindingId = pool.freeBindings.FindFirst();
		if (freeBindingId == pool.freeBindings.npos)
			return {}; //< No free binding in this pool

		pool.freeBindings.Reset(freeBindingId);

		OpenGLShaderBinding* freeBindingMemory = reinterpret_cast<OpenGLShaderBinding*>(&pool.storage[freeBindingId]);
		return ShaderBindingPtr(PlacementNew(freeBindingMemory, *this, poolIndex, freeBindingId));
	}

	auto OpenGLRenderPipelineLayout::GetTextureDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t textureIndex) -> TextureDescriptor&
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));
		assert(textureIndex < m_textureDescriptorCount);

		return pool.textureDescriptor[bindingIndex * m_textureDescriptorCount + textureIndex];
	}

	auto OpenGLRenderPipelineLayout::GetUniformBufferDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t uniformBufferIndex) -> UniformBufferDescriptor&
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));
		assert(uniformBufferIndex < m_uniformBufferDescriptorCount);

		return pool.uniformBufferDescriptor[bindingIndex * m_uniformBufferDescriptorCount + uniformBufferIndex];
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
