// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Renderer/RenderResources.hpp>

namespace Nz
{
	void RenderQueue::ClearRenderables()
	{
		if (!m_renderElements.empty())
		{
			if (m_deletedRenderElements.empty())
				m_deletedRenderElements = std::move(m_deletedRenderElements);
			else
				std::move(m_renderElements.begin(), m_renderElements.end(), std::back_inserter(m_deletedRenderElements));

			m_renderElements.clear();
			m_renderElementsIndices.clear();
			m_shouldRebuildRenderQueue = true;
		}
	}

	void RenderQueue::Prepare(RenderResources& renderResources)
	{
		if (!m_deletedRenderElements.empty())
		{
			renderResources.PushForRelease(std::move(m_deletedRenderElements));
			m_deletedRenderElements.clear();
		}

		if (m_shouldRebuildRenderQueue)
		{
			m_renderQueueRegistry.Clear();
			m_orderedRenderElements.clear();

			for (const auto& renderElementOwner : m_renderElements)
			{
				renderElementOwner->Register(m_renderQueueRegistry);
				m_orderedRenderElements.push_back(renderElementOwner.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_shouldRebuildRenderQueue = false;
			m_shouldSortRenderQueue = true;
		}

		if (m_shouldSortRenderQueue)
		{
			std::sort(m_orderedRenderElements.begin(), m_orderedRenderElements.end(), [&](const RenderElement* lhs, const RenderElement* rhs)
			{
				return lhs->ComputeSortKey(m_renderQueueRegistry) < rhs->ComputeSortKey(m_renderQueueRegistry);
			});
		}
	}

	void RenderQueue::RegisterRenderable(std::size_t renderableIndex, UInt32 instanceIndex, const InstancedRenderable& instancedRenderable, const SkeletonInstance* skeletonInstance, UInt32 renderMask, const Recti& scissorBox)
	{
		InstancedRenderable::ElementData elementData{
			&scissorBox,
			skeletonInstance,
			instanceIndex
		};

		std::size_t firstElementIndex = m_renderElements.size();
		instancedRenderable.BuildElement(m_elementRegistry, elementData, m_passIndex, renderMask, m_renderElements);
		std::size_t elementCount = m_renderElements.size() - firstElementIndex;

		auto it = m_renderElementsIndices.find(renderableIndex);
		if (it == m_renderElementsIndices.end())
		{
			if (elementCount == 0)
				return;

			for (std::size_t i = 0; i < elementCount; ++i)
			{
				const auto& renderElementOwner = m_renderElements[firstElementIndex + i];
				renderElementOwner->Register(m_renderQueueRegistry);
				m_orderedRenderElements.push_back(renderElementOwner.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_renderElementsIndices[renderableIndex] = RenderElementIndices{ firstElementIndex, elementCount };

			m_shouldSortRenderQueue = true;
		}
		else
		{
			RenderElementIndices indices = it->second;

			if (indices.count == elementCount)
			{
				// Fast path: same number of renderables generated, override them
				std::move(m_renderElements.begin() + indices.first, m_renderElements.begin() + indices.first + indices.count, std::back_inserter(m_deletedRenderElements));
				std::move(m_renderElements.begin() + firstElementIndex, m_renderElements.begin() + firstElementIndex + elementCount, m_renderElements.begin() + indices.first);
				m_renderElements.erase(m_renderElements.begin() + firstElementIndex, m_renderElements.end());
			}
			else
			{
				// Slow path: remove previous renderables and correct indices
				auto beginIt = m_renderElements.begin() + indices.first;
				auto endIt = beginIt + indices.count;
				std::move(beginIt, endIt, std::back_inserter(m_deletedRenderElements));
				m_renderElements.erase(beginIt, endIt);

				for (auto&& [renderable, elementIndices] : m_renderElementsIndices)
				{
					if (elementIndices.first >= indices.first)
						elementIndices.first -= indices.count;
				}

				m_renderElementsIndices[renderableIndex] = RenderElementIndices{ firstElementIndex, elementCount };
			}

			m_shouldRebuildRenderQueue = true;
		}
	}

	void RenderQueue::UnregisterRenderable(std::size_t renderableIndex)
	{
		auto it = m_renderElementsIndices.find(renderableIndex);
		if (it != m_renderElementsIndices.end())
		{
			RenderElementIndices indices = it->second;

			auto beginIt = m_renderElements.begin() + indices.first;
			auto endIt = beginIt + indices.count;
			std::move(beginIt, endIt, std::back_inserter(m_deletedRenderElements));
			m_renderElements.erase(beginIt, endIt);

			for (auto&& [renderable, elementIndices] : m_renderElementsIndices)
			{
				if (elementIndices.first > indices.first)
					elementIndices.first -= indices.count;
			}

			m_renderElementsIndices.erase(renderableIndex);
			m_shouldRebuildRenderQueue = true;
		}
	}
}
