// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderResources::RenderResources(RenderDevice& renderDevice) :
	m_renderDevice(renderDevice)
	{
	}

	inline void RenderResources::FlushReleaseQueue()
	{
		for (ReleasableCallback* callback : m_callbackQueue)
			callback->Release();

		m_callbackQueue.clear();

		for (Releasable* releasable : m_releaseQueue)
			PlacementDestroy(releasable);

		m_releaseQueue.clear();

		for (auto& memoryblock : m_releaseMemoryPool)
			memoryblock.clear();
	}

	inline RenderDevice& RenderResources::GetRenderDevice()
	{
		return m_renderDevice;
	}

	template<typename T>
	void RenderResources::PushForRelease(T&& value)
	{
		static_assert(std::is_rvalue_reference_v<decltype(value)>);

		using ReleaseData = ReleasableData<std::remove_cvref_t<T>>;

		ReleaseData* releasable = Allocate<ReleaseData>();
		PlacementNew(releasable, std::forward<T>(value));

		m_releaseQueue.push_back(releasable);
	}

	template<typename F>
	void RenderResources::PushReleaseCallback(F&& callback)
	{
		using ReleaseFunctor = ReleasableFunctor<std::remove_cvref_t<F>>;

		ReleaseFunctor* releasable = Allocate<ReleaseFunctor>();
		PlacementNew(releasable, std::forward<F>(callback));

		m_releaseQueue.push_back(releasable);
		m_callbackQueue.push_back(releasable);
	}

	template<typename T>
	T* RenderResources::Allocate()
	{
		return static_cast<T*>(Allocate(sizeof(T), alignof(T)));
	}

	void* RenderResources::Allocate(std::size_t size, std::size_t alignment)
	{
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

			UInt64 alignedOffset = Align(freeOffset, alignment);
			if (alignedOffset + size > block.capacity())
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
		targetBlock.resize(bestBlock.alignedOffset + size);

		return &targetBlock[bestBlock.alignedOffset];
	}


	template<typename T>
	RenderResources::ReleasableData<T>::ReleasableData(T&& data) :
	m_data(std::move(data))
	{
	}

	template<std::invocable T>
	template<typename U>
	requires(std::constructible_from<T, U&&>)
	RenderResources::ReleasableFunctor<T>::ReleasableFunctor(U&& lambda) :
	m_lambda(std::forward<U>(lambda))
	{
	}

	template<std::invocable T>
	void RenderResources::ReleasableFunctor<T>::Release()
	{
		m_lambda();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
