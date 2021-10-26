// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline void RenderImage::FlushReleaseQueue()
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

	template<typename F>
	void RenderImage::PushReleaseCallback(F&& callback)
	{
		using Functor = ReleasableLambda<std::remove_cv_t<std::remove_reference_t<F>>>;

		constexpr std::size_t functorSize = sizeof(Functor);
		constexpr std::size_t functorAlignment = alignof(Functor);

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

		Functor* releasable = reinterpret_cast<Functor*>(&targetBlock[bestBlock.alignedOffset]);
		PlacementNew(releasable, std::forward<F>(callback));

		m_releaseQueue.push_back(releasable);
	}

	template<typename T>
	template<typename U>
	RenderImage::ReleasableLambda<T>::ReleasableLambda(U&& lambda) :
	m_lambda(std::forward<U>(lambda))
	{
	}

	template<typename T>
	void RenderImage::ReleasableLambda<T>::Release()
	{
		m_lambda();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
