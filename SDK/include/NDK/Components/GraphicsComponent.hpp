// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
#define NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class NDK_API GraphicsComponent : public Component<GraphicsComponent>
	{
		public:
			GraphicsComponent() = default;
			inline GraphicsComponent(const GraphicsComponent& graphicsComponent);
			~GraphicsComponent() = default;

			inline void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const;

			inline void Attach(NzInstancedRenderableRef renderable);

			inline void EnsureTransformMatrixUpdate() const;

			static ComponentIndex componentIndex;

		private:
			void InvalidateRenderableData(const NzInstancedRenderable* renderable, nzUInt32 flags, unsigned int index);
			inline void InvalidateTransformMatrix();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;
			void OnNodeInvalidated(const NzNode* node);

			void UpdateTransformMatrix() const;

			NazaraSlot(NzNode, OnNodeInvalidation, m_nodeInvalidationSlot);

			struct Renderable
			{
				Renderable(NzMatrix4f& transformMatrix) :
				data(transformMatrix),
				dataUpdated(false)
				{
				}

				NazaraSlot(NzInstancedRenderable, OnInstancedRenderableInvalidateData, renderableInvalidationSlot);

				mutable NzInstancedRenderable::InstanceData data;
				NzInstancedRenderableRef renderable;
				mutable bool dataUpdated;
			};

			std::vector<Renderable> m_renderables;
			mutable NzMatrix4f m_transformMatrix;
			mutable bool m_transformMatrixUpdated;
	};
}

#include <NDK/Components/GraphicsComponent.inl>

#endif // NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
