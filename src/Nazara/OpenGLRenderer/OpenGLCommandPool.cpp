// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	CommandBufferPtr OpenGLCommandPool::BuildCommandBuffer(const FunctionRef<void(CommandBufferBuilder& builder)>& callback)
	{
		CommandBufferPtr commandBuffer;
		for (std::size_t i = 0; i < m_commandPools.size(); ++i)
		{
			commandBuffer = AllocateFromPool(i);
			if (commandBuffer)
				break;
		}

		if (!commandBuffer)
		{
			// No allocation could be made, time to allocate a new pool
			std::size_t newPoolIndex = m_commandPools.size();
			AllocatePool();

			commandBuffer = AllocateFromPool(newPoolIndex);
			assert(commandBuffer);
		}

		OpenGLCommandBufferBuilder builder(SafeCast<OpenGLCommandBuffer&>(*commandBuffer.get()));
		callback(builder);

		return commandBuffer;
	}

	void OpenGLCommandPool::UpdateDebugName(std::string_view /*name*/)
	{
		// No OpenGL object to name
	}

	auto OpenGLCommandPool::AllocatePool() -> CommandPool&
	{
		constexpr UInt32 MaxSet = 128;

		CommandPool pool;
		pool.freeCommands.Resize(MaxSet, true);
		pool.storage = std::make_unique<CommandPool::BindingStorage[]>(MaxSet);

		return m_commandPools.emplace_back(std::move(pool));
	}

	CommandBufferPtr OpenGLCommandPool::AllocateFromPool(std::size_t poolIndex)
	{
		auto& pool = m_commandPools[poolIndex];

		std::size_t freeBindingId = pool.freeCommands.FindFirst();
		if (freeBindingId == pool.freeCommands.npos)
			return {}; //< No free binding in this pool

		pool.freeCommands.Reset(freeBindingId);

		OpenGLCommandBuffer* freeBindingMemory = reinterpret_cast<OpenGLCommandBuffer*>(&pool.storage[freeBindingId]);
		return CommandBufferPtr(PlacementNew(freeBindingMemory, *this, poolIndex, freeBindingId));
	}

	void OpenGLCommandPool::Release(CommandBuffer& binding)
	{
		OpenGLCommandBuffer& openglBinding = SafeCast<OpenGLCommandBuffer&>(binding);

		std::size_t poolIndex = openglBinding.GetPoolIndex();
		std::size_t bindingIndex = openglBinding.GetBindingIndex();

		assert(poolIndex < m_commandPools.size());
		auto& pool = m_commandPools[poolIndex];
		assert(!pool.freeCommands.Test(bindingIndex));

		OpenGLCommandBuffer* bindingMemory = reinterpret_cast<OpenGLCommandBuffer*>(&pool.storage[bindingIndex]);
		PlacementDestroy(bindingMemory);

		pool.freeCommands.Set(bindingIndex);

		// Try to free pool if it's one of the last one
		if (poolIndex >= m_commandPools.size() - 1 && poolIndex <= m_commandPools.size())
			TryToShrink();
	}
}
