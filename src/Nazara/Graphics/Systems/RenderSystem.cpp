// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Systems/RenderSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Components/SkeletonComponent.hpp>
#include <Nazara/Graphics/DefaultFramePipeline.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Components/CameraComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>

namespace Nz
{
	RenderSystem::RenderSystem(entt::registry& registry) :
	m_registry(registry),
	m_cameraEntities(m_registry),
	m_graphicsEntities(m_registry),
	m_lightEntities(m_registry),
	m_sharedSkeletonObserver(m_registry),
	m_skeletonObserver(m_registry),
	m_cameraEntityPool(8),
	m_graphicsEntityPool(1024),
	m_lightEntityPool(32)
	{
		m_pipeline = std::make_unique<DefaultFramePipeline>(m_elementRegistry);

		BindObservers();

		m_cameraEntities.SignalExisting();
		m_graphicsEntities.SignalExisting();
		m_lightEntities.SignalExisting();
		m_sharedSkeletonObserver.SignalExisting();
		m_skeletonObserver.SignalExisting();
	}

	RenderSystem::~RenderSystem()
	{
		// Destroy swapchains before pipeline (to free release queues which may point to resources owned by the pipeline)
		m_windowSwapchains.clear();
		m_pipeline.reset();
	}

	WindowSwapchain& RenderSystem::CreateSwapchain(Window& window, const SwapchainParameters& parameters)
	{
		return *m_windowSwapchains.emplace_back(std::make_unique<WindowSwapchain>(Graphics::Instance()->GetRenderDevice(), window, parameters));
	}

	void RenderSystem::DestroySwapchain(WindowSwapchain& swapchain)
	{
		auto it = std::find_if(m_windowSwapchains.begin(), m_windowSwapchains.end(), [&](const auto& ptr) { return ptr.get() == &swapchain; });
		NazaraAssertMsg(it != m_windowSwapchains.end(), "invalid swapchain");

		m_windowSwapchains.erase(it);
	}

	void RenderSystem::Update(Time /*elapsedTime*/)
	{
		UpdateInstances();

		auto HandleSwapchain = [&](WindowSwapchain& swapchain)
		{
			RenderFrame frame = swapchain.AcquireFrame();
			if (!frame)
				return;

			if (!m_cameraEntities.IsEmpty())
				m_pipeline->Render(frame);

			frame.Present();
		};

		for (auto& swapchainPtr : m_windowSwapchains)
			HandleSwapchain(*swapchainPtr);

		for (WindowSwapchain& swapchain : m_externalSwapchains)
			HandleSwapchain(swapchain);
	}

	void RenderSystem::BindObservers()
	{
		m_cameraEntities.OnEntityAdded.Connect([&](entt::entity entity)
		{
			CameraComponent& entityCamera = m_registry.get<CameraComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			CameraEntity*& cameraEntity = m_cameraEntities.Get(entity);

			std::size_t poolIndex;
			cameraEntity = m_cameraEntityPool.Allocate(poolIndex);
			cameraEntity->poolIndex = poolIndex;
			cameraEntity->entity = entity;
			cameraEntity->viewerIndex = m_pipeline->RegisterViewer(&entityCamera, entityCamera.GetRenderOrder());
			cameraEntity->onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Node* /*node*/)
			{
				m_invalidatedCameraNode.insert(entity);
			});

			m_invalidatedCameraNode.insert(entity);
		});

		m_cameraEntities.OnEntityRemove.Connect([&](entt::entity entity)
		{
			CameraEntity* cameraEntity = m_cameraEntities.Get(entity);

			m_invalidatedCameraNode.erase(entity);
			m_pipeline->UnregisterViewer(cameraEntity->viewerIndex);

			m_cameraEntityPool.Free(cameraEntity->poolIndex);
		});

		m_graphicsEntities.OnEntityAdded.Connect([&](entt::entity entity)
		{
			GraphicsComponent& entityGfx = m_registry.get<GraphicsComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			GraphicsEntity*& graphicsEntity = m_graphicsEntities.Get(entity);

			std::size_t poolIndex;
			graphicsEntity = m_graphicsEntityPool.Allocate(poolIndex);
			graphicsEntity->entity = entity;
			graphicsEntity->poolIndex = poolIndex;
			graphicsEntity->renderableIndices.fill(NoInstance);
			graphicsEntity->skeletonInstanceIndex = NoInstance;
			graphicsEntity->worldInstanceIndex = m_pipeline->RegisterWorldInstance(entityGfx.GetWorldInstance());
			graphicsEntity->onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, graphicsEntity](const Node* /*node*/)
			{
				m_invalidatedGfxWorldNode.insert(graphicsEntity);
			});

			// Check observer instead of just component presence to apply all conditions
			if (m_sharedSkeletonObserver.Contains(entity))
				RegisterSharedSkeleton(graphicsEntity, m_registry.get<SharedSkeletonComponent>(entity));
			else if (m_skeletonObserver.Contains(entity))
				RegisterSkeleton(graphicsEntity, m_registry.get<SkeletonComponent>(entity));

			graphicsEntity->onRenderableAttached.Connect(entityGfx.OnRenderableAttached, [this, graphicsEntity](GraphicsComponent* gfx, std::size_t renderableIndex)
			{
				if (!gfx->IsVisible())
					return;

				const auto& renderableEntry = gfx->GetRenderableEntry(renderableIndex);
				graphicsEntity->renderableIndices[renderableIndex] = m_pipeline->RegisterRenderable(graphicsEntity->worldInstanceIndex, graphicsEntity->skeletonInstanceIndex, renderableEntry.renderable.get(), renderableEntry.renderMask, gfx->GetScissorBox());
			});

			graphicsEntity->onRenderableDetach.Connect(entityGfx.OnRenderableDetach, [this, graphicsEntity](GraphicsComponent* gfx, std::size_t renderableIndex)
			{
				if (!gfx->IsVisible())
					return;

				m_pipeline->UnregisterRenderable(graphicsEntity->renderableIndices[renderableIndex]);
				graphicsEntity->renderableIndices[renderableIndex] = NoInstance;
			});

			graphicsEntity->onScissorBoxUpdate.Connect(entityGfx.OnScissorBoxUpdate, [this, graphicsEntity](GraphicsComponent* gfx, const Recti& scissorBox)
			{
				if (!gfx->IsVisible())
					return;

				for (std::size_t renderableIndex = 0; renderableIndex < GraphicsComponent::MaxRenderableCount; ++renderableIndex)
				{
					const auto& renderableEntry = gfx->GetRenderableEntry(renderableIndex);
					if (!renderableEntry.renderable)
						continue;

					m_pipeline->UpdateRenderableScissorBox(graphicsEntity->renderableIndices[renderableIndex], scissorBox);
				}
			});

			graphicsEntity->onVisibilityUpdate.Connect(entityGfx.OnVisibilityUpdate, [this, graphicsEntity](GraphicsComponent* gfx, bool isVisible)
			{
				UpdateGraphicsVisibility(graphicsEntity, *gfx, isVisible);
			});
			m_invalidatedGfxWorldNode.insert(graphicsEntity);

			if (entityGfx.IsVisible())
				UpdateGraphicsVisibility(graphicsEntity, m_registry.get<GraphicsComponent>(entity), true);
		});

		m_graphicsEntities.OnEntityRemove.Connect([&](entt::entity entity)
		{
			GraphicsEntity* graphicsEntity = m_graphicsEntities.Get(entity);

			m_invalidatedGfxWorldNode.erase(graphicsEntity);

			GraphicsComponent& entityGfx = m_registry.get<GraphicsComponent>(entity);
			if (entityGfx.IsVisible())
			{
				for (std::size_t renderableIndex = 0; renderableIndex < GraphicsComponent::MaxRenderableCount; ++renderableIndex)
				{
					const auto& renderableEntry = entityGfx.GetRenderableEntry(renderableIndex);
					if (!renderableEntry.renderable)
						continue;

					m_pipeline->UnregisterRenderable(graphicsEntity->renderableIndices[renderableIndex]);
				}
			}

			m_pipeline->UnregisterWorldInstance(graphicsEntity->worldInstanceIndex);

			m_graphicsEntityPool.Free(graphicsEntity->poolIndex);
		});

		m_lightEntities.OnEntityAdded.Connect([&](entt::entity entity)
		{
			LightComponent& entityLight = m_registry.get<LightComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			LightEntity*& lightEntity = m_lightEntities.Get(entity);

			std::size_t poolIndex;
			lightEntity = m_lightEntityPool.Allocate(poolIndex);
			lightEntity->entity = entity;
			lightEntity->poolIndex = poolIndex;
			lightEntity->onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, lightEntity](const Node* /*node*/)
			{
				m_invalidatedLightWorldNode.insert(lightEntity);
			});

			lightEntity->onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, lightEntity](const Node* /*node*/)
			{
				m_invalidatedLightWorldNode.insert(lightEntity);
			});

			lightEntity->onLightAttached.Connect(entityLight.OnLightAttached, [this, lightEntity](LightComponent* light, std::size_t lightIndex)
			{
				if (!light->IsVisible())
					return;

				const auto& lightEntry = light->GetLightEntry(lightIndex);
				lightEntity->lightIndices[lightIndex] = m_pipeline->RegisterLight(lightEntry.light.get(), lightEntry.renderMask);
			});

			lightEntity->onLightDetach.Connect(entityLight.OnLightDetach, [this, lightEntity](LightComponent* light, std::size_t lightIndex)
			{
				if (!light->IsVisible())
					return;

				m_pipeline->UnregisterLight(lightEntity->lightIndices[lightIndex]);
			});

			lightEntity->onVisibilityUpdate.Connect(entityLight.OnVisibilityUpdate, [this, lightEntity](LightComponent* light, bool isVisible)
			{
				UpdateLightVisibility(lightEntity, *light, isVisible);
			});

			m_invalidatedLightWorldNode.insert(lightEntity);

			if (entityLight.IsVisible())
				UpdateLightVisibility(lightEntity, m_registry.get<LightComponent>(entity), true);
		});

		m_lightEntities.OnEntityRemove.Connect([&](entt::entity entity)
		{
			LightEntity* lightEntity = m_lightEntities.Get(entity);

			m_invalidatedLightWorldNode.erase(lightEntity);

			LightComponent& entityLight = m_registry.get<LightComponent>(entity);
			if (entityLight.IsVisible())
			{
				for (std::size_t lightIndex = 0; lightIndex < LightComponent::MaxLightCount; ++lightIndex)
				{
					const auto& lightEntry = entityLight.GetLightEntry(lightIndex);
					if (!lightEntry.light)
						continue;

					m_pipeline->UnregisterLight(lightEntity->lightIndices[lightIndex]);
				}
			}

			m_lightEntityPool.Free(lightEntity->poolIndex);
		});

		m_sharedSkeletonObserver.OnEntityAdded.Connect([&](entt::entity entity)
		{
			GraphicsEntity** graphicsEntity = m_graphicsEntities.TryGet(entity);
			if (!graphicsEntity)
				return;

			SharedSkeletonComponent& skeletonComponent = m_registry.get<SharedSkeletonComponent>(entity);
			RegisterSharedSkeleton(*graphicsEntity, skeletonComponent);
		});

		m_sharedSkeletonObserver.OnEntityRemove.Connect([&](entt::entity entity)
		{
			SharedSkeletonComponent& skeletonComponent = m_registry.get<SharedSkeletonComponent>(entity);
			Skeleton* skeleton = skeletonComponent.GetSkeleton().get();

			auto skeletonInstanceIt = m_sharedSkeletonInstances.find(skeleton);
			assert(skeletonInstanceIt != m_sharedSkeletonInstances.end());

			SharedSkeleton& sharedSkeleton = skeletonInstanceIt->second;
			assert(sharedSkeleton.useCount > 0);
			if (--sharedSkeleton.useCount == 0)
			{
				m_pipeline->UnregisterSkeleton(sharedSkeleton.skeletonInstanceIndex);
				m_sharedSkeletonInstances.erase(skeletonInstanceIt);
			}

			GraphicsEntity* graphicsEntity = m_graphicsEntities.Get(entity);
			graphicsEntity->skeletonInstanceIndex = NoInstance;
		});

		m_skeletonObserver.OnEntityAdded.Connect([&](entt::entity entity)
		{
			GraphicsEntity** graphicsEntity = m_graphicsEntities.TryGet(entity);
			if (!graphicsEntity)
				return;

			SkeletonComponent& skeletonComponent = m_registry.get<SkeletonComponent>(entity);
			RegisterSkeleton(*graphicsEntity, skeletonComponent);
		});

		m_skeletonObserver.OnEntityRemove.Connect([&](entt::entity entity)
		{
			GraphicsEntity* graphicsEntity = m_graphicsEntities.Get(entity);

			m_pipeline->UnregisterSkeleton(graphicsEntity->skeletonInstanceIndex);
			graphicsEntity->skeletonInstanceIndex = NoInstance;
		});
	}

	void RenderSystem::RegisterSharedSkeleton(GraphicsEntity* graphicsEntity, SharedSkeletonComponent& sharedSkeletonComponent)
	{
		const std::shared_ptr<Skeleton>& skeleton = sharedSkeletonComponent.GetSkeleton();

		if (auto it = m_sharedSkeletonInstances.find(skeleton.get()); it == m_sharedSkeletonInstances.end())
		{
			SharedSkeleton& sharedSkeleton = m_sharedSkeletonInstances[skeleton.get()];
			sharedSkeleton.skeletonInstanceIndex = m_pipeline->RegisterSkeleton(std::make_shared<SkeletonInstance>(skeleton));
			sharedSkeleton.useCount = 1;

			graphicsEntity->skeletonInstanceIndex = sharedSkeleton.skeletonInstanceIndex;
		}
		else
		{
			it->second.useCount++;
			graphicsEntity->skeletonInstanceIndex = it->second.skeletonInstanceIndex;
		}

		for (std::size_t renderableIndex : graphicsEntity->renderableIndices)
		{
			if (renderableIndex != NoInstance)
				m_pipeline->UpdateRenderableSkeletonInstance(renderableIndex, graphicsEntity->skeletonInstanceIndex);
		}
	}

	void RenderSystem::RegisterSkeleton(GraphicsEntity* graphicsEntity, SkeletonComponent& skeletonComponent)
	{
		const std::shared_ptr<Skeleton>& skeleton = skeletonComponent.GetSkeleton();

		graphicsEntity->skeletonInstanceIndex = m_pipeline->RegisterSkeleton(std::make_shared<SkeletonInstance>(skeleton));

		for (std::size_t renderableIndex : graphicsEntity->renderableIndices)
		{
			if (renderableIndex != NoInstance)
				m_pipeline->UpdateRenderableSkeletonInstance(renderableIndex, graphicsEntity->skeletonInstanceIndex);
		}
	}

	void RenderSystem::UpdateGraphicsVisibility(GraphicsEntity* gfxData, GraphicsComponent& gfxComponent, bool isVisible)
	{
		if (isVisible)
		{
			for (std::size_t renderableIndex = 0; renderableIndex < GraphicsComponent::MaxRenderableCount; ++renderableIndex)
			{
				const auto& renderableEntry = gfxComponent.GetRenderableEntry(renderableIndex);
				if (!renderableEntry.renderable)
					continue;

				gfxData->renderableIndices[renderableIndex] = m_pipeline->RegisterRenderable(gfxData->worldInstanceIndex, gfxData->skeletonInstanceIndex, renderableEntry.renderable.get(), renderableEntry.renderMask, gfxComponent.GetScissorBox());
			}
		}
		else
		{
			for (std::size_t renderableIndex = 0; renderableIndex < GraphicsComponent::MaxRenderableCount; ++renderableIndex)
			{
				const auto& renderableEntry = gfxComponent.GetRenderableEntry(renderableIndex);
				if (!renderableEntry.renderable)
					continue;

				m_pipeline->UnregisterRenderable(gfxData->renderableIndices[renderableIndex]);
			}
		}
	}

	void RenderSystem::UpdateLightVisibility(LightEntity* gfxData, LightComponent& lightComponent, bool isVisible)
	{
		if (isVisible)
		{
			for (std::size_t renderableIndex = 0; renderableIndex < LightComponent::MaxLightCount; ++renderableIndex)
			{
				const auto& lightEntry = lightComponent.GetLightEntry(renderableIndex);
				if (!lightEntry.light)
					continue;

				gfxData->lightIndices[renderableIndex] = m_pipeline->RegisterLight(lightEntry.light.get(), lightEntry.renderMask);
			}
		}
		else
		{
			for (std::size_t lightIndex = 0; lightIndex < LightComponent::MaxLightCount; ++lightIndex)
			{
				const auto& lightEntry = lightComponent.GetLightEntry(lightIndex);
				if (!lightEntry.light)
					continue;

				m_pipeline->UnregisterLight(gfxData->lightIndices[lightIndex]);
			}
		}
	}

	void RenderSystem::UpdateInstances()
	{
		for (entt::entity cameraEntity : m_invalidatedCameraNode)
		{
			const NodeComponent& entityNode = m_registry.get<const NodeComponent>(cameraEntity);
			CameraComponent& entityCamera = m_registry.get<CameraComponent>(cameraEntity);

			Vector3f cameraPosition = entityNode.GetGlobalPosition();

			ViewerInstance& viewerInstance = entityCamera.GetViewerInstance();
			viewerInstance.UpdateEyePosition(cameraPosition);
			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(cameraPosition, entityNode.GetGlobalRotation()));
		}
		m_invalidatedCameraNode.clear();

		for (GraphicsEntity* graphicsEntity : m_invalidatedGfxWorldNode)
		{
			entt::entity entity = graphicsEntity->entity;

			const NodeComponent& entityNode = m_registry.get<const NodeComponent>(entity);
			GraphicsComponent& entityGraphics = m_registry.get<GraphicsComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGraphics.GetWorldInstance();
			worldInstance->UpdateWorldMatrix(entityNode.GetTransformMatrix());
		}
		m_invalidatedGfxWorldNode.clear();

		for (LightEntity* lightEntity : m_invalidatedLightWorldNode)
		{
			entt::entity entity = lightEntity->entity;

			const NodeComponent& entityNode = m_registry.get<const NodeComponent>(entity);
			LightComponent& entityLight = m_registry.get<LightComponent>(entity);

			const Vector3f& position = entityNode.GetGlobalPosition();
			const Quaternionf& rotation = entityNode.GetGlobalRotation();
			const Vector3f& scale = entityNode.GetGlobalScale();

			for (const auto& lightEntry : entityLight.GetLights())
			{
				if (!lightEntry.light)
					continue;

				lightEntry.light->UpdateTransform(position, rotation, scale);
			}
		}
		m_invalidatedLightWorldNode.clear();
	}
}
