// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP
#define NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Node.hpp>
#include <memory>
#include <set>
#include <unordered_map>

namespace Nz
{
	class CommandBufferBuilder;
	class FramePipeline;
	class RenderFrame;
	class UploadPool;

	class NAZARA_GRAPHICS_API RenderSystem
	{
		public:
			RenderSystem(entt::registry& registry);
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem(RenderSystem&&) = delete;
			~RenderSystem();

			void Render(entt::registry& registry, RenderFrame& renderFrame);

			RenderSystem& operator=(const RenderSystem&) = delete;
			RenderSystem& operator=(RenderSystem&&) = delete;

		private:
			void OnCameraDestroy(entt::registry& registry, entt::entity entity);
			void OnGraphicsDestroy(entt::registry& registry, entt::entity entity);
			void OnNodeDestroy(entt::registry& registry, entt::entity entity);
			void UpdateInstances(entt::registry& registry);
			void UpdateVisibility(entt::registry& registry);

			struct CameraEntity
			{
				NazaraSlot(Node, OnNodeInvalidation, onNodeInvalidation);
			};

			struct GraphicsEntity
			{
				NazaraSlot(GraphicsComponent, OnRenderableAttached, onRenderableAttached);
				NazaraSlot(GraphicsComponent, OnRenderableDetach, onRenderableDetach);
				NazaraSlot(GraphicsComponent, OnVisibilityUpdate, onVisibilityUpdate);
				NazaraSlot(Node, OnNodeInvalidation, onNodeInvalidation);
			};

			entt::connection m_cameraDestroyConnection;
			entt::connection m_graphicsDestroyConnection;
			entt::connection m_nodeDestroyConnection;
			entt::observer m_cameraConstructObserver;
			entt::observer m_graphicsConstructObserver;
			std::set<entt::entity> m_invalidatedCameraNode;
			std::set<entt::entity> m_invalidatedWorldNode;
			std::unique_ptr<FramePipeline> m_pipeline;
			std::unordered_map<entt::entity, CameraEntity> m_cameraEntities;
			std::unordered_map<entt::entity, GraphicsEntity> m_graphicsEntities;
			std::unordered_set<entt::entity> m_newlyHiddenEntities;
			std::unordered_set<entt::entity> m_newlyVisibleEntities;
	};
}

#include <Nazara/Graphics/Systems/RenderSystem.inl>

#endif // NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP
