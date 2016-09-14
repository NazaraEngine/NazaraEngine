// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/World.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::GraphicsComponent
	* \brief NDK class that represents the component for graphics
	*/

	/*!
	* \brief Invalidates the data for renderable
	*
	* \param renderable Renderable to invalidate
	* \param flags Flags for the instance
	* \param index Index of the renderable to invalidate
	*
	* \remark Produces a NazaraAssert if index is out of bound
	*/

	void GraphicsComponent::InvalidateRenderableData(const Nz::InstancedRenderable* renderable , Nz::UInt32 flags, std::size_t index)
	{
		NazaraAssert(index < m_renderables.size(), "Invalid renderable index");
		NazaraUnused(renderable);

		Renderable& r = m_renderables[index];
		r.dataUpdated = false;
		r.renderable->InvalidateData(&r.data, flags);
	}

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void GraphicsComponent::OnAttached()
	{
		if (m_entity->HasComponent<NodeComponent>())
			m_nodeInvalidationSlot.Connect(m_entity->GetComponent<NodeComponent>().OnNodeInvalidation, this, &GraphicsComponent::OnNodeInvalidated);

		InvalidateTransformMatrix();
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void GraphicsComponent::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			NodeComponent& nodeComponent = static_cast<NodeComponent&>(component);
			m_nodeInvalidationSlot.Connect(nodeComponent.OnNodeInvalidation, this, &GraphicsComponent::OnNodeInvalidated);

			InvalidateTransformMatrix();
		}
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void GraphicsComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			m_nodeInvalidationSlot.Disconnect();

			InvalidateTransformMatrix();
		}
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void GraphicsComponent::OnDetached()
	{
		m_nodeInvalidationSlot.Disconnect();

		InvalidateTransformMatrix();
	}

	/*!
	* \brief Operation to perform when the node is invalidated
	*
	* \param node Pointer to the node
	*/

	void GraphicsComponent::OnNodeInvalidated(const Nz::Node* node)
	{
		NazaraUnused(node);

		// Our view matrix depends on NodeComponent position/rotation
		InvalidateTransformMatrix();
	}

	/*!
	* \brief Updates the bounding volume
	*/

	void GraphicsComponent::UpdateBoundingVolume() const
	{
		EnsureTransformMatrixUpdate();

		m_boundingVolume.MakeNull();
		for (const Renderable& r : m_renderables)
			m_boundingVolume.ExtendTo(r.renderable->GetBoundingVolume());

		m_boundingVolume.Update(m_transformMatrix);
		m_boundingVolumeUpdated = true;
	}

	/*!
	* \brief Updates the transform matrix of the renderable
	*
	* \remark Produces a NazaraAssert if entity is invalid or has no NodeComponent
	*/

	void GraphicsComponent::UpdateTransformMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "GraphicsComponent requires NodeComponent");

		Ndk::RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<Ndk::RenderSystem>();

		m_transformMatrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		m_transformMatrixUpdated = true;
	}

	ComponentIndex GraphicsComponent::componentIndex;
}
