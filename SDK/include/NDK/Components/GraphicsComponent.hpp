// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
#define NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class GraphicsComponent;

	using GraphicsComponentHandle = Nz::ObjectHandle<GraphicsComponent>;

	class NDK_API GraphicsComponent : public Component<GraphicsComponent>, public Nz::HandledObject<GraphicsComponent>
	{
		friend class RenderSystem;

		public:
			GraphicsComponent() = default;
			inline GraphicsComponent(const GraphicsComponent& graphicsComponent);
			~GraphicsComponent() = default;

			inline void AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue) const;

			inline void Attach(Nz::InstancedRenderableRef renderable, int renderOrder = 0);

			inline void EnsureBoundingVolumeUpdate() const;
			inline void EnsureTransformMatrixUpdate() const;

			inline const Nz::BoundingVolumef& GetBoundingVolume() const;

			static ComponentIndex componentIndex;

		private:
			inline void InvalidateBoundingVolume();
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
				Renderable(Nz::Matrix4f& transformMatrix) :
				data(transformMatrix),
				dataUpdated(false)
				{
				}

				NazaraSlot(Nz::InstancedRenderable, OnInstancedRenderableInvalidateData, renderableInvalidationSlot);

				mutable Nz::InstancedRenderable::InstanceData data;
				Nz::InstancedRenderableRef renderable;
				mutable bool dataUpdated;
			};

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