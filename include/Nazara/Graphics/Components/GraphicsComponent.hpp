// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_COMPONENTS_GRAPHICSCOMPONENT_HPP
#define NAZARA_GRAPHICS_COMPONENTS_GRAPHICSCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Math/Rect.hpp>
#include <flecs.h>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API GraphicsComponent
	{
		public:
			struct Renderable;
			static constexpr std::size_t MaxRenderableCount = 8;

			inline GraphicsComponent(bool initiallyVisible = true);
			inline GraphicsComponent(std::shared_ptr<InstancedRenderable> renderable, UInt32 renderMask = 0xFFFFFFFF, bool initiallyVisible = true);
			GraphicsComponent(const GraphicsComponent&) = default;
			GraphicsComponent(GraphicsComponent&&) = default;
			~GraphicsComponent() = default;

			inline void AttachRenderable(std::shared_ptr<InstancedRenderable> renderable, UInt32 renderMask = 0xFFFFFFFF);

			inline void Clear();

			inline void DetachRenderable(const std::shared_ptr<InstancedRenderable>& renderable);

			inline  Boxf GetAABB() const;
			inline const Renderable& GetRenderableEntry(std::size_t renderableIndex) const;
			inline const std::array<Renderable, MaxRenderableCount>& GetRenderables() const;
			inline const Recti& GetScissorBox() const;
			inline const WorldInstancePtr& GetWorldInstance() const;

			inline void Hide();

			inline bool IsVisible() const;

			inline void Show(bool show = true);

			inline void UpdateScissorBox(const Recti& scissorBox);

			GraphicsComponent& operator=(const GraphicsComponent&) = default;
			GraphicsComponent& operator=(GraphicsComponent&&) = default;

			NazaraSignal(OnRenderableAttached, GraphicsComponent* /*graphicsComponent*/, std::size_t /*renderableIndex*/);
			NazaraSignal(OnRenderableDetach, GraphicsComponent* /*graphicsComponent*/, std::size_t /*renderableIndex*/);
			NazaraSignal(OnScissorBoxUpdate, GraphicsComponent* /*graphicsComponent*/, const Recti& /*newScissorBox*/);
			NazaraSignal(OnVisibilityUpdate, GraphicsComponent* /*graphicsComponent*/, bool /*newVisibilityState*/);

			struct Renderable
			{
				std::shared_ptr<InstancedRenderable> renderable;
				UInt32 renderMask;
			};

		private:
			std::array<Renderable, MaxRenderableCount> m_renderables;
			Recti m_scissorBox;
			WorldInstancePtr m_worldInstance;
			bool m_isVisible;
	};
}

#include <Nazara/Graphics/Components/GraphicsComponent.inl>

#endif // NAZARA_GRAPHICS_COMPONENTS_GRAPHICSCOMPONENT_HPP
