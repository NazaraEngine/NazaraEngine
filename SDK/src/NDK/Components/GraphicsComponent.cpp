// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	void GraphicsComponent::InvalidateRenderableData(const NzRenderable* renderable, nzUInt32 flags, unsigned int index)
	{
		NazaraUnused(renderable);

		NazaraAssert(index < m_renderables.size(), "Invalid renderable index");

		Renderable& r = m_renderables[index];
		r.dataUpdated = false;
		r.renderable->InvalidateData(&r.data, flags);
	}

	void GraphicsComponent::OnAttached()
	{
		if (m_entity->HasComponent<NodeComponent>())
			m_nodeInvalidationSlot.Connect(m_entity->GetComponent<NodeComponent>().OnNodeInvalidation, this, &GraphicsComponent::OnNodeInvalidated);

		InvalidateTransformMatrix();
	}

	void GraphicsComponent::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			NodeComponent& nodeComponent = static_cast<NodeComponent&>(component);
			m_nodeInvalidationSlot.Connect(nodeComponent.OnNodeInvalidation, this, &GraphicsComponent::OnNodeInvalidated);

			InvalidateTransformMatrix();
		}
	}

	void GraphicsComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			m_nodeInvalidationSlot.Disconnect();

			InvalidateTransformMatrix();
		}
	}

	void GraphicsComponent::OnDetached()
	{
		m_nodeInvalidationSlot.Disconnect();

		InvalidateTransformMatrix();
	}

	void GraphicsComponent::OnNodeInvalidated(const NzNode* node)
	{
		NazaraUnused(node);

		// Our view matrix depends on NodeComponent position/rotation
		InvalidateTransformMatrix();
	}

	void GraphicsComponent::UpdateTransformMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "GraphicsComponent requires NodeComponent");

		m_transformMatrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		m_transformMatrixUpdated = true;
	}

	ComponentIndex GraphicsComponent::componentIndex;
}
