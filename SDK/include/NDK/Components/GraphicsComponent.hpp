// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
#define NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP

#include <Nazara/Graphics/CullingList.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/Component.hpp>
#include <unordered_map>

namespace Ndk
{
	class GraphicsComponent;

	using GraphicsComponentCullingList = Nz::CullingList<GraphicsComponent>;
	using GraphicsComponentHandle = Nz::ObjectHandle<GraphicsComponent>;

	class NDK_API GraphicsComponent : public Component<GraphicsComponent>, public Nz::HandledObject<GraphicsComponent>
	{
		friend class RenderSystem;

		public:
			using RenderableList = std::vector<Nz::InstancedRenderableRef>;

			GraphicsComponent() = default;
			inline GraphicsComponent(const GraphicsComponent& graphicsComponent);
			~GraphicsComponent() = default;

			inline void AddToCullingList(GraphicsComponentCullingList* cullingList) const;
			void AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue) const;

			inline void Attach(Nz::InstancedRenderableRef renderable, int renderOrder = 0);
			void Attach(Nz::InstancedRenderableRef renderable, const Nz::Matrix4f& localMatrix, int renderOrder = 0);

			inline void Clear();

			inline void Detach(const Nz::InstancedRenderable* renderable);

			inline bool DoesRequireRealTimeReflections() const;

			inline void EnsureBoundingVolumeUpdate() const;
			inline void EnsureTransformMatrixUpdate() const;

			inline void GetAttachedRenderables(RenderableList* renderables) const;
			inline std::size_t GetAttachedRenderableCount() const;

			inline const Nz::BoundingVolumef& GetBoundingVolume() const;

			inline void RemoveFromCullingList(GraphicsComponentCullingList* cullingList) const;

			static ComponentIndex componentIndex;

		private:
			struct Renderable;

			void ConnectInstancedRenderableSignals(Renderable& renderable);

			inline void InvalidateBoundingVolume() const;
			void InvalidateRenderableData(const Nz::InstancedRenderable* renderable, Nz::UInt32 flags, std::size_t index);
			void InvalidateRenderableMaterial(const Nz::InstancedRenderable* renderable, std::size_t skinIndex, std::size_t matIndex, const Nz::MaterialRef& newMat);
			inline void InvalidateRenderables();
			void InvalidateReflectionMap();
			inline void InvalidateTransformMatrix();

			void RegisterMaterial(Nz::Material* material, std::size_t count = 1);

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			void OnInstancedRenderableResetMaterials(const Nz::InstancedRenderable* renderable, std::size_t newMaterialCount);
			void OnInstancedRenderableSkinChange(const Nz::InstancedRenderable* renderable, std::size_t newSkinIndex);
			void OnMaterialReflectionChange(const Nz::Material* material, Nz::ReflectionMode reflectionMode);
			void OnNodeInvalidated(const Nz::Node* node);

			void UnregisterMaterial(Nz::Material* material);

			void UpdateBoundingVolume() const;
			void UpdateTransformMatrix() const;

			NazaraSlot(Nz::Node, OnNodeInvalidation, m_nodeInvalidationSlot);

			struct MaterialEntry
			{
				NazaraSlot(Nz::Material, OnMaterialReflectionChange, reflectionModelChangeSlot);

				std::size_t renderableCounter;
			};

			struct Renderable
			{
				Renderable(const Nz::Matrix4f& transformMatrix) :
				data(transformMatrix),
				dataUpdated(false)
				{
				}

				Renderable(const Renderable&) = delete;
				Renderable(Renderable&& rhs) noexcept = default;

				~Renderable()
				{
					// Disconnect release slot before releasing instanced renderable reference
					renderableReleaseSlot.Disconnect();
				}

				Renderable& operator=(const Renderable&) = delete;
				Renderable& operator=(Renderable&& r) noexcept = default;

				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableInvalidateBoundingVolume, renderableBoundingVolumeInvalidationSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableInvalidateData, renderableDataInvalidationSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableInvalidateMaterial, renderableMaterialInvalidationSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableRelease, renderableReleaseSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableResetMaterials, renderableResetMaterialsSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableSkinChange, renderableSkinChangeSlot);

				mutable Nz::InstancedRenderable::InstanceData data;
				Nz::InstancedRenderableRef renderable;
				mutable bool dataUpdated;
			};

			using VolumeCullingListEntry = GraphicsComponentCullingList::VolumeEntry;

			struct VolumeCullingEntry
			{
				VolumeCullingListEntry listEntry;

				NazaraSlot(GraphicsComponentCullingList, OnCullingListRelease, cullingListReleaseSlot);
			};

			std::size_t m_reflectiveMaterialCount;
			mutable std::vector<VolumeCullingEntry> m_volumeCullingEntries;
			std::vector<Renderable> m_renderables;
			std::unordered_map<const Nz::Material*, MaterialEntry> m_materialEntries;
			mutable Nz::BoundingVolumef m_boundingVolume;
			mutable Nz::Matrix4f m_transformMatrix;
			Nz::TextureRef m_reflectionMap;
			mutable bool m_boundingVolumeUpdated;
			mutable bool m_transformMatrixUpdated;
			unsigned int m_reflectionMapSize;
	};
}

#include <NDK/Components/GraphicsComponent.inl>

#endif // NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
#endif // NDK_SERVER
