// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERQUEUE_HPP
#define NAZARA_GRAPHICS_RENDERQUEUE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class ElementRendererRegistry;
	class InstancedRenderable;
	class GpuResources;
	class SkeletonInstance;

	class RenderQueue
	{
		public:
			inline explicit RenderQueue(std::size_t passIndex, RenderQueueFlags flags = {});
			RenderQueue(const RenderQueue&) = delete;
			RenderQueue(RenderQueue&&) = delete;
			~RenderQueue() = default;

			inline void BeginRegisterRenderable();

			void ClearRenderables();

			void FinalizeRegisterRenderable(std::size_t renderableIndex);

			inline std::size_t GetContentHash() const;
			inline RenderQueueFlags GetFlags() const;
			inline std::size_t GetPassIndex() const;

			void Update(GpuResources& renderResources);

			template<typename F> void Process(UInt32 renderMask, F&& callback) const;

			template<typename F> void RegisterRenderable(F&& callback);

			template<typename F> void SortRenderQueue(F&& callback);

			void UnregisterRenderable(std::size_t renderableIndex);

			void UpdateRenderQueue();

			RenderQueue& operator=(const RenderQueue&) = delete;
			RenderQueue& operator=(RenderQueue&&) = delete;

		private:
			void RecomputeContentHash();

			struct RenderElementIndices
			{
				std::size_t first;
				std::size_t count;
			};

			std::size_t m_contentHash;
			std::size_t m_firstAddedElementIndex;
			std::size_t m_passIndex;
			std::unordered_map<std::size_t /*renderableIndex*/, RenderElementIndices> m_renderElementsIndices;
			std::vector<RenderElementOwner> m_deletedRenderElements;
			std::vector<RenderElementOwner> m_renderElements;
			std::vector<const RenderElement*> m_orderedRenderElements;
			RenderQueueFlags m_flags;
			RenderQueueRegistry m_renderQueueRegistry;
			bool m_shouldRebuildRenderQueue;
			bool m_shouldSortRenderQueue;
	};
}

#include <Nazara/Graphics/RenderQueue.inl>

#endif // NAZARA_GRAPHICS_RENDERQUEUE_HPP
