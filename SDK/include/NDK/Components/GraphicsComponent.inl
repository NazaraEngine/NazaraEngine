// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <algorithm>
#include "GraphicsComponent.hpp"

namespace Ndk
{
	/*!
	* \brief Constructs a GraphicsComponent object by copy semantic
	*
	* \param graphicsComponent GraphicsComponent to copy
	*/

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

	/*!
	* \brief Adds the renderable elements to the render queue
	*
	* \param renderQueue Queue to be added
	*/

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

	/*!
	* \brief Attaches a renderable to the entity
	*
	* \param renderable Reference to a renderable element
	* \param renderOrder Render order of the element
	*/

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

	/*!
	* \brief Clears every renderable elements
	*/

	inline void GraphicsComponent::Clear()
	{
		m_renderables.clear();

		InvalidateBoundingVolume();
	}

	/*!
	* \brief Detaches a renderable to the entity
	*
	* \param renderable Reference to a renderable element
	*/

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

	/*!
	* \brief Ensures the bounding volume is up to date
	*/

	inline void GraphicsComponent::EnsureBoundingVolumeUpdate() const
	{
		if (!m_boundingVolumeUpdated)
			UpdateBoundingVolume();
	}

	/*!
	* \brief Ensures the transformation matrix is up to date
	*/

	inline void GraphicsComponent::EnsureTransformMatrixUpdate() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();
	}

	/*!
	* \brief Gets the set of renderable elements
	*
	* \param renderables Pointer to the list of renderables
	*
	* \remark Produces a NazaraAssert if renderables is invalid
	*/

	inline void GraphicsComponent::GetAttachedRenderables(RenderableList* renderables) const
	{
		NazaraAssert(renderables, "Invalid renderable list");

		renderables->reserve(renderables->size() + m_renderables.size());
		for (const Renderable& r : m_renderables)
			renderables->push_back(r.renderable);
	}

	/*!
	* \brief Gets the number of renderable elements attached to the entity
	* \return Number of renderable elements
	*/

	inline std::size_t GraphicsComponent::GetAttachedRenderableCount() const
	{
		return m_renderables.size();
	}

	/*!
	* \brief Gets the bouding volume of the entity
	* \return A constant reference to the bounding volume
	*/

	inline const Nz::BoundingVolumef& GraphicsComponent::GetBoundingVolume() const
	{
		EnsureBoundingVolumeUpdate();

		return m_boundingVolume;
	}

	/*!
	* \brief Invalidates the bounding volume
	*/

	inline void GraphicsComponent::InvalidateBoundingVolume()
	{
		m_boundingVolumeUpdated = false;
	}

	/*!
	* \brief Invalidates every renderable elements
	*/

	inline void GraphicsComponent::InvalidateRenderables()
	{
		for (Renderable& r : m_renderables)
			r.dataUpdated = false;
	}

	/*!
	* \brief Invalidates the transformation matrix
	*/

	inline void GraphicsComponent::InvalidateTransformMatrix()
	{
		m_boundingVolumeUpdated = false;
		m_transformMatrixUpdated = false;

		InvalidateRenderables();
	}
}
