// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/BaseElementRenderPipelinePass.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>

namespace Nz
{
	BaseElementRenderPipelinePass::BaseElementRenderPipelinePass(PassData& passData) :
	FramePipelinePass(FramePipelineNotification::ElementInvalidation | FramePipelineNotification::MaterialInstanceRegistration),
	m_elementRegistry(passData.elementRegistry),
	m_renderMask(MaxValue())
	{
	}

	void BaseElementRenderPipelinePass::ClearRenderables()
	{
		if (!m_renderElements.empty())
		{
			if (m_deletedRenderElements.empty())
				m_deletedRenderElements = std::move(m_deletedRenderElements);
			else
				std::move(m_renderElements.begin(), m_renderElements.end(), std::back_inserter(m_deletedRenderElements));

			m_renderElements.clear();
			m_renderElementsIndices.clear();
			m_rebuildRenderQueue = true;
		}
	}

	void BaseElementRenderPipelinePass::Prepare(FrameData& frameData)
	{
		if (!m_deletedRenderElements.empty())
		{
			frameData.renderResources.PushForRelease(std::move(m_deletedRenderElements));
			m_deletedRenderElements.clear();
		}

		if (m_rebuildRenderQueue)
		{
			m_renderQueueRegistry.Clear();
			m_renderQueue.Clear();

			for (const auto& renderElement : m_renderElements)
			{
				renderElement->Register(m_renderQueueRegistry);
				m_renderQueue.Insert(renderElement.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_rebuildRenderQueue = false;
		}

		// TODO: Don't sort every frame if no material pass requires distance sorting
		m_renderQueue.Sort([&](const RenderElement* element)
		{
			return element->ComputeSortingScore(frameData.frustum, m_renderQueueRegistry);
		});
	}

	void BaseElementRenderPipelinePass::RegisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		if (!materialInstance.HasPass(m_passIndex))
			return;

		auto it = m_materialInstances.find(&materialInstance);
		if (it == m_materialInstances.end())
		{
			auto& matPassEntry = m_materialInstances[&materialInstance];
			matPassEntry.onMaterialInstancePipelineInvalidated.Connect(materialInstance.OnMaterialInstancePipelineInvalidated, [this](const MaterialInstance*, std::size_t passIndex)
			{
				if (passIndex != m_passIndex)
					return;

				//m_rebuildElements = true;
			});
		}
		else
			it->second.usedCount++;
	}

	void BaseElementRenderPipelinePass::RegisterRenderable(std::size_t renderableIndex, UInt32 instanceIndex, const InstancedRenderable& instancedRenderable, const SkeletonInstance* skeletonInstance, UInt32 renderMask, const Recti& scissorBox)
	{
		if ((m_renderMask & renderMask) == 0)
			return;

		InstancedRenderable::ElementData elementData{
			&scissorBox,
			skeletonInstance,
			instanceIndex
		};

		auto it = m_renderElementsIndices.find(renderableIndex);
		if (it == m_renderElementsIndices.end())
		{
			std::size_t firstElementIndex = m_renderElements.size();
			instancedRenderable.BuildElement(m_elementRegistry, elementData, m_passIndex, m_renderElements);
			std::size_t elementCount = m_renderElements.size() - firstElementIndex;

			if (elementCount == 0)
				return;

			for (std::size_t i = 0; i < elementCount; ++i)
			{
				const auto& renderElement = m_renderElements[firstElementIndex + i];
				renderElement->Register(m_renderQueueRegistry);
				m_renderQueue.Insert(renderElement.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_renderElementsIndices[renderableIndex] = RenderElementIndices{ firstElementIndex, elementCount };
		}
		else
		{
			const RenderElementIndices& indices = it->second;

			std::size_t firstElementIndex = m_renderElements.size();
			instancedRenderable.BuildElement(m_elementRegistry, elementData, m_passIndex, m_renderElements);
			std::size_t elementCount = m_renderElements.size() - firstElementIndex;

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

			m_rebuildRenderQueue = true;
		}
	}

	void BaseElementRenderPipelinePass::UnregisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		auto it = m_materialInstances.find(&materialInstance);
		if (it != m_materialInstances.end())
		{
			if (--it->second.usedCount == 0)
				m_materialInstances.erase(it);
		}
	}

	void BaseElementRenderPipelinePass::UnregisterRenderable(std::size_t renderableIndex)
	{
		auto it = m_renderElementsIndices.find(renderableIndex);
		if (it != m_renderElementsIndices.end())
		{
			const RenderElementIndices& indices = it->second;

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
			m_rebuildRenderQueue = true;
		}
	}
}
