// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline TransientResources::TransientResources(RenderDevice& renderDevice) :
	m_renderDevice(renderDevice)
	{
	}

	inline void TransientResources::FlushReleaseQueue()
	{
		for (Releasable* releasable : m_releaseQueue)
		{
			releasable->Release();
			PlacementDestroy(releasable);
		}
		m_releaseQueue.clear();

		for (auto& memoryblock : m_releaseMemoryPool)
			memoryblock.clear();
	}

	inline RenderDevice& TransientResources::GetRenderDevice()
	{
		return m_renderDevice;
	}

	template<typename T>
	void TransientResources::PushForRelease(T&& value)
	{
		static_assert(std::is_rvalue_reference_v<decltype(value)>);

		return PushReleaseCallback([v = std::move(value)] {});
	}

	template<typename F>
	void TransientResources::PushReleaseCallback(F&& callback)
	{
		using ReleaseFunctor = ReleasableLambda<std::remove_cv_t<std::remove_reference_t<F>>>;

		constexpr std::size_t functorSize = sizeof(ReleaseFunctor);
		constexpr std::size_t functorAlignment = alignof(ReleaseFunctor);

		// Try to minimize lost space
		struct
		{
			Block* block = nullptr;
			UInt64 alignedOffset = 0;
			UInt64 lostSpace = 0;
		} bestBlock;

		for (Block& block : m_releaseMemoryPool)
		{
			std::size_t freeOffset = block.size();

			UInt64 alignedOffset = Align(freeOffset, functorAlignment);
			if (alignedOffset + functorSize > block.capacity())
				continue; //< Not enough space

			UInt64 lostSpace = alignedOffset - freeOffset;

			if (!bestBlock.block || lostSpace < bestBlock.lostSpace)
			{
				bestBlock.block = &block;
				bestBlock.alignedOffset = alignedOffset;
				bestBlock.lostSpace = lostSpace;
			}
		}

		// No block found, allocate a new one
		if (!bestBlock.block)
		{
			Block newBlock;
			newBlock.reserve(BlockSize);

			bestBlock.block = &m_releaseMemoryPool.emplace_back(std::move(newBlock));
			bestBlock.alignedOffset = 0;
			bestBlock.lostSpace = 0;
		}

		Block& targetBlock = *bestBlock.block;
		targetBlock.resize(bestBlock.alignedOffset + functorSize);

		ReleaseFunctor* releasable = reinterpret_cast<ReleaseFunctor*>(&targetBlock[bestBlock.alignedOffset]);
		PlacementNew(releasable, std::forward<F>(callback));

		m_releaseQueue.push_back(releasable);
	}

	template<typename T>
	template<typename U>
	TransientResources::ReleasableLambda<T>::ReleasableLambda(U&& lambda) :
	m_lambda(std::forward<U>(lambda))
	{
	}

	template<typename T>
	void TransientResources::ReleasableLambda<T>::Release()
	{
		m_lambda();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
