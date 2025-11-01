// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP
#define NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/FlecsUtils.hpp>
#include <Nazara/Core/Node.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Components/LightComponent.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <flecs.h>
#include <array>
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
			static constexpr bool AllowConcurrent = false;
			static constexpr Int64 ExecutionOrder = 1'000;

			RenderSystem(flecs::world& world);
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem(RenderSystem&&) = delete;
			~RenderSystem();

			inline void AttachExternalSwapchain(WindowSwapchain& swapchain);

			WindowSwapchain& CreateSwapchain(Window& window, const SwapchainParameters& parameters = SwapchainParameters{});

			void DestroySwapchain(WindowSwapchain& swapchain);
			inline void DetachExternalSwapchain(WindowSwapchain& swapchain);

			inline FramePipeline& GetFramePipeline();
			inline const FramePipeline& GetFramePipeline() const;

			void Update(Time elapsedTime);

			RenderSystem& operator=(const RenderSystem&) = delete;
			RenderSystem& operator=(RenderSystem&&) = delete;

		private:
			struct GraphicsEntity;
			struct LightEntity;

			void UpdateGraphicsVisibility(GraphicsEntity* gfxData, GraphicsComponent& gfxComponent, bool isVisible);
			void UpdateLightVisibility(LightEntity* gfxData, LightComponent& lightComponent, bool isVisible);
			void UpdateInstances();

			static constexpr std::size_t NoInstance = std::numeric_limits<std::size_t>::max();

			struct CameraEntity
			{
				flecs::entity entity;
				std::size_t poolIndex;
				std::size_t viewerIndex;

				NazaraSlot(Node, OnNodeInvalidation, onNodeInvalidation);
			};

			struct GraphicsEntity
			{
				flecs::entity entity;
				std::array<std::size_t, GraphicsComponent::MaxRenderableCount> renderableIndices;
				std::size_t poolIndex;
				std::size_t skeletonInstanceIndex;
				std::size_t worldInstanceIndex;

				NazaraSlot(GraphicsComponent, OnRenderableAttached, onRenderableAttached);
				NazaraSlot(GraphicsComponent, OnRenderableDetach, onRenderableDetach);
				NazaraSlot(GraphicsComponent, OnScissorBoxUpdate, onScissorBoxUpdate);
				NazaraSlot(GraphicsComponent, OnVisibilityUpdate, onVisibilityUpdate);
				NazaraSlot(Node, OnNodeInvalidation, onNodeInvalidation);
				NazaraSlot(Skeleton, OnSkeletonJointsInvalidated, onSkeletonJointsInvalidated); //< only connected for owned skeleton
			};

			struct LightEntity
			{
				flecs::entity entity;
				std::array<std::size_t, LightComponent::MaxLightCount> lightIndices;
				std::size_t poolIndex;

				NazaraSlot(LightComponent, OnLightAttached, onLightAttached);
				NazaraSlot(LightComponent, OnLightDetach, onLightDetach);
				NazaraSlot(LightComponent, OnVisibilityUpdate, onVisibilityUpdate);
				NazaraSlot(Node, OnNodeInvalidation, onNodeInvalidation);
			};

			struct SharedSkeleton
			{
				std::size_t skeletonInstanceIndex;
				std::size_t useCount;

				NazaraSlot(Skeleton, OnSkeletonJointsInvalidated, onJointsInvalidated);
			};

			flecs::world& m_world;
			flecs::entity m_cameraConstructObserver;
			flecs::entity m_graphicsConstructObserver;
			flecs::entity m_lightConstructObserver;
			flecs::entity m_sharedSkeletonConstructObserver;
			flecs::entity m_skeletonConstructObserver;
			std::set<CameraEntity*> m_invalidatedCameraNode;
			std::set<GraphicsEntity*> m_invalidatedGfxWorldNode;
			std::set<LightEntity*> m_invalidatedLightWorldNode;
			std::unique_ptr<FramePipeline> m_pipeline;
			std::unordered_map<flecs::entity, CameraEntity*, FlecsHasher> m_cameraEntities;
			std::unordered_map<flecs::entity, GraphicsEntity*, FlecsHasher> m_graphicsEntities;
			std::unordered_map<flecs::entity, LightEntity*, FlecsHasher> m_lightEntities;
			std::unordered_map<Skeleton*, SharedSkeleton> m_sharedSkeletonInstances;
			std::vector<std::reference_wrapper<WindowSwapchain>> m_externalSwapchains;
			std::vector<std::unique_ptr<WindowSwapchain>> m_windowSwapchains;
			ElementRendererRegistry m_elementRegistry;
			MemoryPool<CameraEntity> m_cameraEntityPool;
			MemoryPool<GraphicsEntity> m_graphicsEntityPool;
			MemoryPool<LightEntity> m_lightEntityPool;
	};
}

#include <Nazara/Graphics/Systems/RenderSystem.inl>

#endif // NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP
