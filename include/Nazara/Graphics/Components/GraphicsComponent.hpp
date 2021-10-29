// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_COMPONENTS_GRAPHICSCOMPONENT_HPP
#define NAZARA_GRAPHICS_COMPONENTS_GRAPHICSCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API GraphicsComponent
	{
		public:
			struct Renderable;

			GraphicsComponent();
			GraphicsComponent(const GraphicsComponent&) = default;
			GraphicsComponent(GraphicsComponent&&) = default;
			~GraphicsComponent() = default;

			inline void AttachRenderable(std::shared_ptr<InstancedRenderable> renderable, UInt32 renderMask = 0xFFFFFFFF);
			inline void DetachRenderable(const std::shared_ptr<InstancedRenderable>& renderable);

			inline const std::vector<Renderable>& GetRenderables() const;
			inline const WorldInstancePtr& GetWorldInstance() const;

			GraphicsComponent& operator=(const GraphicsComponent&) = default;
			GraphicsComponent& operator=(GraphicsComponent&&) = default;

			NazaraSignal(OnRenderableAttached, GraphicsComponent* /*graphicsComponent*/, const Renderable& /*renderable*/);
			NazaraSignal(OnRenderableDetach, GraphicsComponent* /*graphicsComponent*/, const Renderable& /*renderable*/);

			struct Renderable
			{
				std::shared_ptr<InstancedRenderable> renderable;
				UInt32 renderMask;
			};

		private:
			std::vector<Renderable> m_renderables;
			WorldInstancePtr m_worldInstance;
	};
}

#include <Nazara/Graphics/Components/GraphicsComponent.inl>

#endif // NAZARA_GRAPHICS_COMPONENTS_GRAPHICSCOMPONENT_HPP
