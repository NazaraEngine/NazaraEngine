// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/World.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <algorithm>

namespace Ndk
{
	inline GraphicsComponent::GraphicsComponent() :
	m_reflectiveMaterialCount(0),
	m_scissorRect(-1, -1)
	{
	}

	/*!
	* \brief Constructs a GraphicsComponent object by copy semantic
	*
	* \param graphicsComponent GraphicsComponent to copy
	*/
	inline GraphicsComponent::GraphicsComponent(const GraphicsComponent& graphicsComponent) :
	Component(graphicsComponent),
	HandledObject(graphicsComponent),
	m_reflectiveMaterialCount(0),
	m_boundingVolume(graphicsComponent.m_boundingVolume),
	m_transformMatrix(graphicsComponent.m_transformMatrix),
	m_boundingVolumeUpdated(graphicsComponent.m_boundingVolumeUpdated),
	m_transformMatrixUpdated(graphicsComponent.m_transformMatrixUpdated)
	{
		m_renderables.reserve(graphicsComponent.m_renderables.size());
		for (const Renderable& r : graphicsComponent.m_renderables)
			Attach(r.renderable, r.data.localMatrix, r.data.renderOrder);
	}

	inline void GraphicsComponent::AddToCullingList(GraphicsComponentCullingList* cullingList) const
	{
		m_volumeCullingEntries.emplace_back(VolumeCullingEntry{});
		VolumeCullingEntry& entry = m_volumeCullingEntries.back();
		entry.cullingListReleaseSlot.Connect(cullingList->OnCullingListRelease, this, &GraphicsComponent::RemoveFromCullingList);
		entry.listEntry = cullingList->RegisterVolumeTest(this);

		InvalidateBoundingVolume();
	}

	/*!
	* \brief Attaches a renderable to the entity
	*
	* \param renderable Reference to a renderable element
	* \param renderOrder Render order of the element
	*/
	inline void GraphicsComponent::Attach(Nz::InstancedRenderableRef renderable, int renderOrder)
	{
		return Attach(std::move(renderable), Nz::Matrix4f::Identity(), renderOrder);
	}

	/*!
	* \brief Clears every renderable elements
	*/

	inline void GraphicsComponent::Clear()
	{
		m_materialEntries.clear();
		m_renderables.clear();

		if (m_reflectiveMaterialCount > 0)
		{
			m_reflectiveMaterialCount = 0;
			InvalidateReflectionMap();
		}

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

				std::size_t materialCount = renderable->GetMaterialCount();
				for (std::size_t i = 0; i < materialCount; ++i)
					UnregisterMaterial(renderable->GetMaterial(i));

				m_renderables.erase(it);
				break;
			}
		}
	}

	/*!
	* \brief Checks if this graphics component requires real-time reflections to be generated
	*
	* If any of the materials attached to a GraphicsComponent (via the attached instanced renderable) needs real-time reflections, this function will return true.
	*
	* \return True if real-time reflections needs to be generated or false
	*/
	inline bool GraphicsComponent::DoesRequireRealTimeReflections() const
	{
		return m_reflectiveMaterialCount != 0 && m_reflectionMap;
	}

	/*!
	* \brief Calls a function for every renderable attached to this component
	*
	* \param func Callback function which will be called with renderable data
	*/
	template<typename Func>
	void GraphicsComponent::ForEachRenderable(const Func& func) const
	{
		for (const auto& renderableData : m_renderables)
			func(renderableData.renderable, renderableData.data.localMatrix, renderableData.data.renderOrder);
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

	inline void GraphicsComponent::RemoveFromCullingList(GraphicsComponentCullingList* cullingList) const
	{
		for (auto it = m_volumeCullingEntries.begin(); it != m_volumeCullingEntries.end(); ++it)
		{
			if (it->listEntry.GetParent() == cullingList)
			{
				if (m_volumeCullingEntries.size() > 1)
					*it = std::move(m_volumeCullingEntries.back());

				m_volumeCullingEntries.pop_back();
				break;
			}
		}
	}

	inline void GraphicsComponent::SetScissorRect(const Nz::Recti& scissorRect)
	{
		m_scissorRect = scissorRect;

		for (VolumeCullingEntry& entry : m_volumeCullingEntries)
			entry.listEntry.ForceInvalidation(); //< Invalidate render queues
	}

	inline void GraphicsComponent::UpdateLocalMatrix(const Nz::InstancedRenderable* instancedRenderable, const Nz::Matrix4f& localMatrix)
	{
		for (auto& renderable : m_renderables)
		{
			if (renderable.renderable == instancedRenderable)
			{
				renderable.data.localMatrix = localMatrix;

				InvalidateBoundingVolume();
				break;
			}
		}
	}

	inline void GraphicsComponent::UpdateRenderOrder(const Nz::InstancedRenderable* instancedRenderable, int renderOrder)
	{
		for (auto& renderable : m_renderables)
		{
			if (renderable.renderable == instancedRenderable)
			{
				renderable.data.renderOrder = renderOrder;
				break;
			}
		}
	}

	/*!
	* \brief Invalidates the bounding volume
	*/

	inline void GraphicsComponent::InvalidateBoundingVolume() const
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
		m_transformMatrixUpdated = false;

		InvalidateBoundingVolume();
		InvalidateRenderables();
	}
}
