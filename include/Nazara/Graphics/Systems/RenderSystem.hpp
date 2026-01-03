// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP
#define NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/EnttObserver.hpp>
#include <Nazara/Core/Node.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Components/LightComponent.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <entt/entt.hpp>
#include <array>
#include <memory>
#include <set>
#include <unordered_map>

namespace Nz
{
	class CommandBufferBuilder;
	class FramePipeline;
	class RenderFrame;
	class SharedSkeletonComponent;
	class SkeletonComponent;
	class UploadPool;

	class NAZARA_GRAPHICS_API RenderSystem
	{
		public:
			static constexpr bool AllowConcurrent = false;
			static constexpr Int64 ExecutionOrder = 1'000;

			RenderSystem(entt::registry& registry);
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

			void BindObservers();
			void RegisterSharedSkeleton(GraphicsEntity* graphicsEntity, SharedSkeletonComponent& sharedSkeletonComponent);
			void RegisterSkeleton(GraphicsEntity* graphicsEntity, SkeletonComponent& skeletonComponent);
			void UpdateGraphicsVisibility(GraphicsEntity* gfxData, GraphicsComponent& gfxComponent, bool isVisible);
			void UpdateLightVisibility(LightEntity* gfxData, LightComponent& lightComponent, bool isVisible);
			void UpdateInstances();

			static constexpr std::size_t NoInstance = std::numeric_limits<std::size_t>::max();

			struct CameraEntity
			{
				entt::entity entity;
				std::size_t poolIndex;
				std::size_t viewerIndex;

				NazaraSlot(Node, OnNodeInvalidation, onNodeInvalidation);
			};

			struct GraphicsEntity
			{
				entt::entity entity;
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
				entt::entity entity;
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

			entt::registry& m_registry;
			entt::scoped_connection m_cameraDestroyConnection;
			entt::scoped_connection m_disabledConstructedConnection;
			entt::scoped_connection m_disabledDestroyConnection;
			entt::scoped_connection m_graphicsDestroyConnection;
			entt::scoped_connection m_lightDestroyConnection;
			entt::scoped_connection m_nodeDestroyConnection;
			entt::scoped_connection m_sharedSkeletonDestroyConnection;
			entt::scoped_connection m_skeletonDestroyConnection;
			std::set<GraphicsEntity*> m_invalidatedGfxWorldNode;
			std::set<LightEntity*> m_invalidatedLightWorldNode;
			std::unique_ptr<FramePipeline> m_pipeline;
			std::unordered_map<Skeleton*, SharedSkeleton> m_sharedSkeletonInstances;
			std::unordered_set<entt::entity> m_invalidatedCameraNode;
			std::vector<std::reference_wrapper<WindowSwapchain>> m_externalSwapchains;
			std::vector<std::unique_ptr<WindowSwapchain>> m_windowSwapchains;
			ElementRendererRegistry m_elementRegistry;
			EnttObserver<TypeList<class CameraComponent, class NodeComponent>, TypeList<class DisabledComponent>, CameraEntity*> m_cameraEntities;
			EnttObserver<TypeList<class GraphicsComponent, class NodeComponent>, TypeList<class DisabledComponent>, GraphicsEntity*> m_graphicsEntities;
			EnttObserver<TypeList<class LightComponent, class NodeComponent>, TypeList<class DisabledComponent>, LightEntity*> m_lightEntities;
			EnttObserver<TypeList<class GraphicsComponent, class NodeComponent, class SharedSkeletonComponent>, TypeList<class DisabledComponent, class SkeletonComponent>> m_sharedSkeletonObserver;
			EnttObserver<TypeList<class GraphicsComponent, class NodeComponent, class SkeletonComponent>, TypeList<class DisabledComponent, class SharedSkeletonComponent>> m_skeletonObserver;
			MemoryPool<CameraEntity> m_cameraEntityPool;
			MemoryPool<GraphicsEntity> m_graphicsEntityPool;
			MemoryPool<LightEntity> m_lightEntityPool;
	};
}

#include <Nazara/Graphics/Systems/RenderSystem.inl>

#endif // NAZARA_GRAPHICS_SYSTEMS_RENDERSYSTEM_HPP
