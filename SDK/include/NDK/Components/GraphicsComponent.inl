// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <algorithm>

namespace Ndk
{
	inline GraphicsComponent::GraphicsComponent(const GraphicsComponent& graphicsComponent) :
	Component(graphicsComponent),
	m_transformMatrix(graphicsComponent.m_transformMatrix),
	m_transformMatrixUpdated(graphicsComponent.m_transformMatrixUpdated)
	{
		m_renderables.reserve(graphicsComponent.m_renderables.size());
		for (const Renderable& r : graphicsComponent.m_renderables)
			Attach(r.renderable);
	}

	inline void GraphicsComponent::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
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

	inline void GraphicsComponent::Attach(NzRenderableRef renderable)
	{
		m_renderables.emplace_back(m_transformMatrix);
		Renderable& r = m_renderables.back();
		r.renderable = std::move(renderable);
		r.renderableInvalidationSlot.Connect(r.renderable->OnRenderableInvalidateInstanceData, std::bind(&GraphicsComponent::InvalidateRenderableData, this, std::placeholders::_1, std::placeholders::_2, m_renderables.size()-1));
	}

	inline void GraphicsComponent::EnsureTransformMatrixUpdate() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();
	}

	inline void GraphicsComponent::InvalidateTransformMatrix()
	{
		for (Renderable& r : m_renderables)
			r.dataUpdated = false;

		m_transformMatrixUpdated = false;
	}
}
