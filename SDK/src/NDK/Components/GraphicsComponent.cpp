// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/World.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	void GraphicsComponent::InvalidateRenderableData(const Nz::InstancedRenderable* renderable, Nz::UInt32 flags, std::size_t index)
	{
		NazaraAssert(index < m_renderables.size(), "Invalid renderable index");
		NazaraUnused(renderable);

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

	void GraphicsComponent::OnNodeInvalidated(const Nz::Node* node)
	{
		NazaraUnused(node);

		// Our view matrix depends on NodeComponent position/rotation
		InvalidateTransformMatrix();
	}

	void GraphicsComponent::UpdateBoundingVolume() const
	{
		EnsureTransformMatrixUpdate();

		m_boundingVolume.MakeNull();
		for (const Renderable& r : m_renderables)
			m_boundingVolume.ExtendTo(r.renderable->GetBoundingVolume());

		m_boundingVolume.Update(m_transformMatrix);
		m_boundingVolumeUpdated = true;
	}

	void GraphicsComponent::UpdateTransformMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "GraphicsComponent requires NodeComponent");

		Ndk::RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<Ndk::RenderSystem>();

		m_transformMatrix = Nz::Matrix4f::ConcatenateAffine(renderSystem.GetCoordinateSystemMatrix(), m_entity->GetComponent<NodeComponent>().GetTransformMatrix());
		m_transformMatrixUpdated = true;
	}

	ComponentIndex GraphicsComponent::componentIndex;
}
