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
	* \brief Adds the renderable elements to the render queue
	*
	* \param renderQueue Queue to be added
	*/
	void GraphicsComponent::AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue) const
	{
		EnsureTransformMatrixUpdate();

		RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<RenderSystem>();

		for (const Renderable& object : m_renderables)
		{
			if (!object.dataUpdated)
			{
				object.data.transformMatrix = Nz::Matrix4f::ConcatenateAffine(renderSystem.GetCoordinateSystemMatrix(), Nz::Matrix4f::ConcatenateAffine(object.data.localMatrix, m_transformMatrix));
				object.renderable->UpdateData(&object.data);
				object.dataUpdated = true;
			}

			object.renderable->AddToRenderQueue(renderQueue, object.data);
		}
	}

	/*!
	* \brief Attaches a renderable to the entity
	*
	* \param renderable Reference to a renderable element
	* \param renderOrder Render order of the element
	*/
	void GraphicsComponent::Attach(Nz::InstancedRenderableRef renderable, int renderOrder)
	{
		return Attach(renderable, Nz::Matrix4f::Identity(), renderOrder);
	}

	/*!
	* \brief Attaches a renderable to the entity with a specific matrix
	*
	* \param renderable Reference to a renderable element
	* \param localMatrix Local matrix that will be applied to the instanced renderable
	* \param renderOrder Render order of the element
	*/
	void GraphicsComponent::Attach(Nz::InstancedRenderableRef renderable, const Nz::Matrix4f& localMatrix, int renderOrder)
	{
		m_renderables.emplace_back(m_transformMatrix);
		Renderable& r = m_renderables.back();
		r.data.localMatrix = localMatrix;
		r.data.renderOrder = renderOrder;
		r.renderable = std::move(renderable);
		r.renderableBoundingVolumeInvalidationSlot.Connect(r.renderable->OnInstancedRenderableInvalidateBoundingVolume, [this] (const Nz::InstancedRenderable*) { InvalidateBoundingVolume(); });
		r.renderableDataInvalidationSlot.Connect(r.renderable->OnInstancedRenderableInvalidateData, std::bind(&GraphicsComponent::InvalidateRenderableData, this, std::placeholders::_1, std::placeholders::_2, m_renderables.size() - 1));
		r.renderableReleaseSlot.Connect(r.renderable->OnInstancedRenderableRelease, this, &GraphicsComponent::Detach);

		InvalidateBoundingVolume();
	}

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

		for (VolumeCullingEntry& entry : m_volumeCullingEntries)
			entry.listEntry.ForceInvalidation();
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
		{
			Nz::BoundingVolumef boundingVolume = r.renderable->GetBoundingVolume();

			// Adjust renderable bounding volume by local matrix
			if (boundingVolume.IsFinite())
			{
				Nz::Boxf localBox = boundingVolume.obb.localBox;
				Nz::Vector3f newPos = r.data.localMatrix * localBox.GetPosition();
				Nz::Vector3f newLengths = r.data.localMatrix * localBox.GetLengths();

				boundingVolume.Set(Nz::Boxf(newPos.x, newPos.y, newPos.z, newLengths.x, newLengths.y, newLengths.z));
			}

			m_boundingVolume.ExtendTo(r.renderable->GetBoundingVolume());
		}

		RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<RenderSystem>();

		m_boundingVolume.Update(Nz::Matrix4f::ConcatenateAffine(renderSystem.GetCoordinateSystemMatrix(), m_transformMatrix));
		m_boundingVolumeUpdated = true;

		for (VolumeCullingEntry& entry : m_volumeCullingEntries)
			entry.listEntry.UpdateVolume(m_boundingVolume);
	}

	/*!
	* \brief Updates the transform matrix of the renderable
	*
	* \remark Produces a NazaraAssert if entity is invalid or has no NodeComponent
	*/

	void GraphicsComponent::UpdateTransformMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "GraphicsComponent requires NodeComponent");

		m_transformMatrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		m_transformMatrixUpdated = true;
	}

	ComponentIndex GraphicsComponent::componentIndex;
}
