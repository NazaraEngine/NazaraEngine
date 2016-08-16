// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <algorithm>
#include "GraphicsComponent.hpp"

namespace Ndk
{
	inline GraphicsComponent::GraphicsComponent(const GraphicsComponent& graphicsComponent) :
	Component(graphicsComponent),
	HandledObject(graphicsComponent),
	m_boundingVolume(graphicsComponent.m_boundingVolume),
	m_transformMatrix(graphicsComponent.m_transformMatrix),
	m_boundingVolumeUpdated(graphicsComponent.m_boundingVolumeUpdated),
	m_transformMatrixUpdated(graphicsComponent.m_transformMatrixUpdated)
	{
		m_renderables.reserve(graphicsComponent.m_renderables.size());
		for (const Renderable& r : graphicsComponent.m_renderables)
			Attach(r.renderable, r.data.renderOrder);
	}

	inline void GraphicsComponent::AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue) const
	{
		EnsureTransformMatrixUpdate();

		for (const Renderable& object : m_renderables)
		{
			if (!object.dataUpdated)
			{
				object.renderable->UpdateData(&object.data);
				object.dataUpdated = true;
			}

			object.renderable->AddToRenderQueue(renderQueue, object.data);
		}
	}

	inline void GraphicsComponent::Attach(Nz::InstancedRenderableRef renderable, int renderOrder)
	{
		m_renderables.emplace_back(m_transformMatrix);
		Renderable& r = m_renderables.back();
		r.data.renderOrder = renderOrder;
		r.renderable = std::move(renderable);
		r.renderableInvalidationSlot.Connect(r.renderable->OnInstancedRenderableInvalidateData, std::bind(&GraphicsComponent::InvalidateRenderableData, this, std::placeholders::_1, std::placeholders::_2, m_renderables.size() - 1));
		r.renderableReleaseSlot.Connect(r.renderable->OnInstancedRenderableRelease, this, &GraphicsComponent::Detach);

		InvalidateBoundingVolume();
	}

	inline void GraphicsComponent::Clear()
	{
		m_renderables.clear();

		InvalidateBoundingVolume();
	}

	inline void GraphicsComponent::Detach(const Nz::InstancedRenderable* renderable)
	{
		for (auto it = m_renderables.begin(); it != m_renderables.end(); ++it)
		{
			if (it->renderable == renderable)
			{
				InvalidateBoundingVolume();
				m_renderables.erase(it);
				break;
			}
		}
	}

	inline void GraphicsComponent::EnsureBoundingVolumeUpdate() const
	{
		if (!m_boundingVolumeUpdated)
			UpdateBoundingVolume();
	}

	inline void GraphicsComponent::EnsureTransformMatrixUpdate() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();
	}

	inline void GraphicsComponent::GetAttachedRenderables(RenderableList* renderables) const
	{
		NazaraAssert(renderables, "Invalid renderable list");

		renderables->reserve(renderables->size() + m_renderables.size());
		for (const Renderable& r : m_renderables)
			renderables->push_back(r.renderable);
	}

	inline std::size_t GraphicsComponent::GetAttachedRenderableCount() const
	{
		return m_renderables.size();
	}

	inline const Nz::BoundingVolumef& GraphicsComponent::GetBoundingVolume() const
	{
		EnsureBoundingVolumeUpdate();

		return m_boundingVolume;
	}

	inline void GraphicsComponent::InvalidateBoundingVolume()
	{
		m_boundingVolumeUpdated = false;
	}

	inline void GraphicsComponent::InvalidateRenderables()
	{
		for (Renderable& r : m_renderables)
			r.dataUpdated = false;
	}

	inline void GraphicsComponent::InvalidateTransformMatrix()
	{
		m_boundingVolumeUpdated = false;
		m_transformMatrixUpdated = false;

		InvalidateRenderables();
	}
}
