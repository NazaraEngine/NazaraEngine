// Copyright (C) 2015 Jérôme Leclercq
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

			void Attach(Nz::InstancedRenderableRef renderable, int renderOrder = 0);
			void Attach(Nz::InstancedRenderableRef renderable, const Nz::Matrix4f& localMatrix, int renderOrder = 0);

			inline void Clear();

			inline void Detach(const Nz::InstancedRenderable* renderable);

			inline void EnsureBoundingVolumeUpdate() const;
			inline void EnsureTransformMatrixUpdate() const;

			inline void GetAttachedRenderables(RenderableList* renderables) const;
			inline std::size_t GetAttachedRenderableCount() const;

			inline const Nz::BoundingVolumef& GetBoundingVolume() const;

			inline void RemoveFromCullingList(GraphicsComponentCullingList* cullingList) const;

			static ComponentIndex componentIndex;

		private:
			inline void InvalidateBoundingVolume() const;
			void InvalidateRenderableData(const Nz::InstancedRenderable* renderable, Nz::UInt32 flags, std::size_t index);
			inline void InvalidateRenderables();
			inline void InvalidateTransformMatrix();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;
			void OnNodeInvalidated(const Nz::Node* node);

			void UpdateBoundingVolume() const;
			void UpdateTransformMatrix() const;

			NazaraSlot(Nz::Node, OnNodeInvalidation, m_nodeInvalidationSlot);

			struct Renderable
			{
				Renderable(const Nz::Matrix4f& transformMatrix) :
				data(transformMatrix),
				dataUpdated(false)
				{
				}

				Renderable(Renderable&& rhs) noexcept :
				renderableBoundingVolumeInvalidationSlot(std::move(rhs.renderableBoundingVolumeInvalidationSlot)),
				renderableDataInvalidationSlot(std::move(rhs.renderableDataInvalidationSlot)),
				renderableReleaseSlot(std::move(rhs.renderableReleaseSlot)),
				data(std::move(rhs.data)),
				renderable(std::move(rhs.rhs)),
				dataUpdated(rhs.dataUpdated)
				{
				}

				~Renderable()
				{
					// Disconnect release slot before releasing instanced renderable reference
					renderableReleaseSlot.Disconnect();
				}

				Renderable& operator=(Renderable&& r) noexcept
				{
					data = std::move(r.data);
					dataUpdated = r.dataUpdated;
					renderable = std::move(r.renderable);
					renderableBoundingVolumeInvalidationSlot = std::move(r.renderableBoundingVolumeInvalidationSlot);
					renderableDataInvalidationSlot = std::move(r.renderableDataInvalidationSlot);
					renderableReleaseSlot = std::move(r.renderableReleaseSlot);

					return *this;
				}

				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableInvalidateBoundingVolume, renderableBoundingVolumeInvalidationSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableInvalidateData, renderableDataInvalidationSlot);
				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableRelease, renderableReleaseSlot);

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

			mutable std::vector<VolumeCullingEntry> m_volumeCullingEntries;
			std::vector<Renderable> m_renderables;
			mutable Nz::BoundingVolumef m_boundingVolume;
			mutable Nz::Matrix4f m_transformMatrix;
			mutable bool m_boundingVolumeUpdated;
			mutable bool m_transformMatrixUpdated;
	};
}

#include <NDK/Components/GraphicsComponent.inl>

#endif // NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
#endif // NDK_SERVER
