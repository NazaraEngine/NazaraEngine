// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICSCOMPONENT_HPP
#define NAZARA_GRAPHICSCOMPONENT_HPP

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
			GraphicsComponent() = default;
			GraphicsComponent(const GraphicsComponent&) = default;
			GraphicsComponent(GraphicsComponent&&) = default;
			~GraphicsComponent() = default;

			inline void AttachRenderable(std::shared_ptr<InstancedRenderable> renderable);
			inline void DetachRenderable(const std::shared_ptr<InstancedRenderable>& renderable);

			inline const std::vector<std::shared_ptr<InstancedRenderable>>& GetRenderables() const;
			inline WorldInstance& GetWorldInstance();
			inline const WorldInstance& GetWorldInstance() const;

			GraphicsComponent& operator=(const GraphicsComponent&) = default;
			GraphicsComponent& operator=(GraphicsComponent&&) = default;

		private:
			std::vector<std::shared_ptr<InstancedRenderable>> m_renderables;
			WorldInstance m_worldInstance;
	};
}

#include <Nazara/Graphics/Components/GraphicsComponent.inl>

#endif
