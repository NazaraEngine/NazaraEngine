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
#include <Nazara/Math/Rect.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class ElementRendererRegistry;
	class InstancedRenderable;
	class RenderResources;
	class SkeletonInstance;

	class RenderQueue
	{
		public:
			inline RenderQueue(ElementRendererRegistry& elementRegistry, std::size_t passIndex);

			void ClearRenderables();

			inline std::size_t GetContentHash() const;

			void Prepare(RenderResources& renderResources);

			template<typename F> void Process(UInt32 renderMask, F&& callback) const;

			void RegisterRenderable(std::size_t renderableIndex, UInt32 instanceIndex, const InstancedRenderable& instancedRenderable, const SkeletonInstance* skeletonInstance, UInt32 renderMask, const Recti& scissorBox);

			void UnregisterRenderable(std::size_t renderableIndex);

			void UpdateRenderQueue();

		private:
			struct RenderElementIndices
			{
				std::size_t first;
				std::size_t count;
			};

			std::size_t m_contentHash;
			std::size_t m_passIndex;
			std::unordered_map<std::size_t /*renderableIndex*/, RenderElementIndices> m_renderElementsIndices;
			std::vector<RenderElementOwner> m_deletedRenderElements;
			std::vector<RenderElementOwner> m_renderElements;
			std::vector<const RenderElement*> m_orderedRenderElements;
			ElementRendererRegistry& m_elementRegistry;
			RenderQueueRegistry m_renderQueueRegistry;
			bool m_shouldRebuildRenderQueue;
			bool m_shouldSortRenderQueue;
	};
}

#include <Nazara/Graphics/RenderQueue.inl>

#endif // NAZARA_GRAPHICS_RENDERQUEUE_HPP
