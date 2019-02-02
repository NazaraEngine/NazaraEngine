// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

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
		EnsureBoundingVolumesUpdate();
		EnsureTransformMatrixUpdate();

		RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<RenderSystem>();

		for (const Renderable& object : m_renderables)
		{
			if (!object.dataUpdated)
			{
				object.renderable->UpdateData(&object.data);
				object.dataUpdated = true;
			}

			object.renderable->AddToRenderQueue(renderQueue, object.data, m_scissorRect);
		}
	}

	/*!
	* \brief Adds the renderable elements to the render queue if their bounding volume intersects with the frustum
	*
	* \param frustum Queue to be added
	* \param renderQueue Queue to be added
	*/
	void GraphicsComponent::AddToRenderQueueByCulling(const Nz::Frustumf& frustum, Nz::AbstractRenderQueue* renderQueue) const
	{
		EnsureBoundingVolumesUpdate();
		EnsureTransformMatrixUpdate();

		RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<RenderSystem>();

		for (const Renderable& object : m_renderables)
		{
			if (frustum.Contains(object.boundingVolume))
			{
				if (!object.dataUpdated)
				{
					object.renderable->UpdateData(&object.data);
					object.dataUpdated = true;
				}

				object.renderable->AddToRenderQueue(renderQueue, object.data, m_scissorRect);
			}
		}
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
		Renderable& entry = m_renderables.back();
		entry.data.localMatrix = localMatrix;
		entry.data.renderOrder = renderOrder;
		entry.renderable = std::move(renderable);

		ConnectInstancedRenderableSignals(entry);

		std::size_t materialCount = entry.renderable->GetMaterialCount();
		for (std::size_t i = 0; i < materialCount; ++i)
			RegisterMaterial(entry.renderable->GetMaterial(i));

		InvalidateAABB();
		ForceCullingInvalidation();
	}

	void GraphicsComponent::ConnectInstancedRenderableSignals(Renderable& entry)
	{
		entry.renderableBoundingVolumeInvalidationSlot.Connect(entry.renderable->OnInstancedRenderableInvalidateBoundingVolume, [this](const Nz::InstancedRenderable*) { InvalidateAABB(); });
		entry.renderableDataInvalidationSlot.Connect(entry.renderable->OnInstancedRenderableInvalidateData, std::bind(&GraphicsComponent::InvalidateRenderableData, this, std::placeholders::_1, std::placeholders::_2, m_renderables.size() - 1));
		entry.renderableMaterialInvalidationSlot.Connect(entry.renderable->OnInstancedRenderableInvalidateMaterial, this, &GraphicsComponent::InvalidateRenderableMaterial);
		entry.renderableReleaseSlot.Connect(entry.renderable->OnInstancedRenderableRelease, this, &GraphicsComponent::Detach);
		entry.renderableResetMaterialsSlot.Connect(entry.renderable->OnInstancedRenderableResetMaterials, this, &GraphicsComponent::OnInstancedRenderableResetMaterials);
		entry.renderableSkinChangeSlot.Connect(entry.renderable->OnInstancedRenderableSkinChange, this, &GraphicsComponent::OnInstancedRenderableSkinChange);
	}

	void GraphicsComponent::InvalidateRenderableData(const Nz::InstancedRenderable* renderable , Nz::UInt32 flags, std::size_t index)
	{
		NazaraAssert(index < m_renderables.size(), "Invalid renderable index");
		NazaraUnused(renderable);

		Renderable& r = m_renderables[index];
		r.dataUpdated = false;
		r.renderable->InvalidateData(&r.data, flags);

		ForceCullingInvalidation();
	}

	void GraphicsComponent::InvalidateRenderableMaterial(const Nz::InstancedRenderable* renderable, std::size_t skinIndex, std::size_t matIndex, const Nz::MaterialRef& newMat)
	{
		// Don't listen to dormant materials
		if (renderable->GetSkin() != skinIndex)
			return;

		RegisterMaterial(newMat);

		const Nz::MaterialRef& oldMat = renderable->GetMaterial(skinIndex, matIndex);
		UnregisterMaterial(oldMat);
	}

	void Ndk::GraphicsComponent::InvalidateReflectionMap()
	{
		m_entity->Invalidate();

		if (m_reflectiveMaterialCount > 0)
		{
			if (!m_reflectionMap)
			{
				m_reflectionMap = Nz::Texture::New();
				if (!m_reflectionMap->Create(Nz::ImageType_Cubemap, Nz::PixelFormatType_RGB8, m_reflectionMapSize, m_reflectionMapSize))
				{
					NazaraWarning("Failed to create reflection map, reflections will be disabled for this entity");
					return;
				}
			}
		}
		else
			m_reflectionMap.Reset();
	}

	void GraphicsComponent::RegisterMaterial(Nz::Material* material, std::size_t count)
	{
		auto it = m_materialEntries.find(material);
		if (it == m_materialEntries.end())
		{
			MaterialEntry matEntry;
			matEntry.reflectionModelChangeSlot.Connect(material->OnMaterialReflectionModeChange, this, &GraphicsComponent::OnMaterialReflectionChange);
			matEntry.renderableCounter = count;

			if (material->GetReflectionMode() == Nz::ReflectionMode_RealTime)
			{
				if (m_reflectiveMaterialCount++ == 0)
					InvalidateReflectionMap();
			}

			m_materialEntries.emplace(material, std::move(matEntry));
		}
		else
			it->second.renderableCounter += count;
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

	void GraphicsComponent::OnInstancedRenderableResetMaterials(const Nz::InstancedRenderable* renderable, std::size_t newMaterialCount)
	{
		RegisterMaterial(Nz::Material::GetDefault(), newMaterialCount);

		std::size_t materialCount = renderable->GetMaterialCount();
		for (std::size_t i = 0; i < materialCount; ++i)
			UnregisterMaterial(renderable->GetMaterial(i));
	}

	void GraphicsComponent::OnInstancedRenderableSkinChange(const Nz::InstancedRenderable* renderable, std::size_t newSkinIndex)
	{
		std::size_t materialCount = renderable->GetMaterialCount();
		for (std::size_t i = 0; i < materialCount; ++i)
			RegisterMaterial(renderable->GetMaterial(newSkinIndex, i));

		for (std::size_t i = 0; i < materialCount; ++i)
			UnregisterMaterial(renderable->GetMaterial(i));
	}

	void GraphicsComponent::OnMaterialReflectionChange(const Nz::Material* material, Nz::ReflectionMode reflectionMode)
	{
		// Since this signal is only called when the new reflection mode is different from the current one, no need to compare both
		if (material->GetReflectionMode() == Nz::ReflectionMode_RealTime)
		{
			if (--m_reflectiveMaterialCount == 0)
				InvalidateReflectionMap();
		}
		else if (reflectionMode == Nz::ReflectionMode_RealTime)
		{
			if (m_reflectiveMaterialCount++ == 0)
				InvalidateReflectionMap();
		}
	}

	void GraphicsComponent::OnNodeInvalidated(const Nz::Node* node)
	{
		NazaraUnused(node);

		// Our view matrix depends on NodeComponent position/rotation
		InvalidateAABB();
		InvalidateTransformMatrix();

		ForceCullingInvalidation(); //< Force invalidation on movement for now (FIXME)
	}

	void GraphicsComponent::UnregisterMaterial(Nz::Material* material)
	{
		auto it = m_materialEntries.find(material);
		NazaraAssert(it != m_materialEntries.end(), "Material not registered");

		MaterialEntry& matEntry = it->second;
		if (--matEntry.renderableCounter == 0)
		{
			if (material->GetReflectionMode() == Nz::ReflectionMode_RealTime)
			{
				if (--m_reflectiveMaterialCount == 0)
					InvalidateReflectionMap();
			}

			m_materialEntries.erase(it);
		}
	}

	/*!
	* \brief Updates the bounding volume
	*/

	void GraphicsComponent::UpdateBoundingVolumes() const
	{
		EnsureTransformMatrixUpdate();

		RenderSystem& renderSystem = m_entity->GetWorld()->GetSystem<RenderSystem>();

		m_aabb.Set(-1.f, -1.f, -1.f);

		bool isAabbSet = false;

		for (const Renderable& r : m_renderables)
		{
			r.boundingVolume = r.renderable->GetBoundingVolume();
			r.data.transformMatrix = Nz::Matrix4f::ConcatenateAffine(renderSystem.GetCoordinateSystemMatrix(), Nz::Matrix4f::ConcatenateAffine(r.data.localMatrix, m_transformMatrix));
			if (r.boundingVolume.IsFinite())
			{
				r.boundingVolume.Update(r.data.transformMatrix);

				if (isAabbSet)
					m_aabb.ExtendTo(r.boundingVolume.aabb);
				else
				{
					m_aabb.Set(r.boundingVolume.aabb);
					isAabbSet = true;
				}
			}
		}

		m_boundingVolumesUpdated = true;

		for (CullingBoxEntry& entry : m_cullingBoxEntries)
			entry.listEntry.UpdateBox(m_aabb);
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
