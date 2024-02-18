// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Systems/RenderSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Components/SkeletonComponent.hpp>
#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Components/CameraComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	RenderSystem::RenderSystem(entt::registry& registry) :
	m_registry(registry),
	m_cameraConstructObserver(registry, entt::collector.group<CameraComponent, NodeComponent>(entt::exclude<DisabledComponent>)),
	m_graphicsConstructObserver(registry, entt::collector.group<GraphicsComponent, NodeComponent>(entt::exclude<DisabledComponent>)),
	m_lightConstructObserver(registry, entt::collector.group<LightComponent, NodeComponent>(entt::exclude<DisabledComponent>)),
	m_sharedSkeletonConstructObserver(registry, entt::collector.group<GraphicsComponent, NodeComponent, SharedSkeletonComponent>(entt::exclude<DisabledComponent, SkeletonComponent>)),
	m_skeletonConstructObserver(registry, entt::collector.group<GraphicsComponent, NodeComponent, SkeletonComponent>(entt::exclude<DisabledComponent, SharedSkeletonComponent>)),
	m_cameraEntityPool(8),
	m_graphicsEntityPool(1024),
	m_lightEntityPool(32)
	{
		m_cameraDestroyConnection = registry.on_destroy<CameraComponent>().connect<&RenderSystem::OnCameraDestroy>(this);
		m_disabledConstructedConnection = registry.on_construct<DisabledComponent>().connect<&RenderSystem::OnDisabledConstructed>(this);
		m_graphicsDestroyConnection = registry.on_destroy<GraphicsComponent>().connect<&RenderSystem::OnGraphicsDestroy>(this);
		m_lightDestroyConnection = registry.on_destroy<LightComponent>().connect<&RenderSystem::OnLightDestroy>(this);
		m_nodeDestroyConnection = registry.on_destroy<NodeComponent>().connect<&RenderSystem::OnNodeDestroy>(this);
		m_sharedSkeletonDestroyConnection = registry.on_destroy<SharedSkeletonComponent>().connect<&RenderSystem::OnSharedSkeletonDestroy>(this);
		m_skeletonDestroyConnection = registry.on_destroy<SkeletonComponent>().connect<&RenderSystem::OnSkeletonDestroy>(this);

		m_pipeline = std::make_unique<ForwardFramePipeline>(m_elementRegistry);
	}

	RenderSystem::~RenderSystem()
	{
		m_cameraConstructObserver.disconnect();
		m_graphicsConstructObserver.disconnect();
		m_lightConstructObserver.disconnect();
		m_pipeline.reset();
	}

	WindowSwapchain& RenderSystem::CreateSwapchain(Window& window, const SwapchainParameters& parameters)
	{
		return *m_windowSwapchains.emplace_back(std::make_unique<WindowSwapchain>(Graphics::Instance()->GetRenderDevice(), window, parameters));
	}

	void RenderSystem::DestroySwapchain(WindowSwapchain& swapchain)
	{
		auto it = std::find_if(m_windowSwapchains.begin(), m_windowSwapchains.end(), [&](const auto& ptr) { return ptr.get() == &swapchain; });
		NazaraAssert(it != m_windowSwapchains.end(), "invalid swapchain");

		m_windowSwapchains.erase(it);
	}

	void RenderSystem::Update(Time /*elapsedTime*/)
	{
		UpdateObservers();
		UpdateInstances();

		auto HandleSwapchain = [&](WindowSwapchain& swapchain)
		{
			RenderFrame frame = swapchain.AcquireFrame();
			if (!frame)
				return;

			if (!m_cameraEntities.empty())
				m_pipeline->Render(frame);

			frame.Present();
		};

		for (auto& swapchainPtr : m_windowSwapchains)
			HandleSwapchain(*swapchainPtr);

		for (WindowSwapchain& swapchain : m_externalSwapchains)
			HandleSwapchain(swapchain);
	}

	void RenderSystem::OnCameraDestroy([[maybe_unused]] entt::registry& registry, entt::entity entity)
	{
		assert(&m_registry == &registry);

		auto it = m_cameraEntities.find(entity);
		if (it == m_cameraEntities.end())
			return;

		CameraEntity* cameraEntity = it->second;

		m_cameraEntities.erase(it);
		m_invalidatedCameraNode.erase(cameraEntity);
		m_pipeline->UnregisterViewer(cameraEntity->viewerIndex);

		m_cameraEntityPool.Free(cameraEntity->poolIndex);
	}

	void RenderSystem::OnDisabledConstructed(entt::registry& registry, entt::entity entity)
	{
		// This is essentially the same
		OnNodeDestroy(registry, entity);
	}

	void RenderSystem::OnGraphicsDestroy([[maybe_unused]] entt::registry& registry, entt::entity entity)
	{
		assert(&m_registry == &registry);

		auto it = m_graphicsEntities.find(entity);
		if (it == m_graphicsEntities.end())
			return;

		GraphicsEntity* graphicsEntity = it->second;

		m_graphicsEntities.erase(entity);
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
	}

	void RenderSystem::OnLightDestroy([[maybe_unused]] entt::registry& registry, entt::entity entity)
	{
		assert(&m_registry == &registry);

		auto it = m_lightEntities.find(entity);
		if (it == m_lightEntities.end())
			return;

		LightEntity* lightEntity = it->second;

		m_lightEntities.erase(entity);
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
	}

	void RenderSystem::OnNodeDestroy(entt::registry& registry, entt::entity entity)
	{
		assert(&m_registry == &registry);

		if (m_registry.try_get<CameraComponent>(entity))
			OnCameraDestroy(registry, entity);

		if (m_registry.try_get<GraphicsComponent>(entity))
			OnGraphicsDestroy(registry, entity);

		if (m_registry.try_get<LightComponent>(entity))
			OnLightDestroy(registry, entity);

		if (m_registry.try_get<SharedSkeletonComponent>(entity))
			OnSharedSkeletonDestroy(registry, entity);

		if (m_registry.try_get<SkeletonComponent>(entity))
			OnSkeletonDestroy(registry, entity);
	}

	void RenderSystem::OnSharedSkeletonDestroy(entt::registry& registry, entt::entity entity)
	{
		assert(&m_registry == &registry);

		SharedSkeletonComponent& skeletonComponent = registry.get<SharedSkeletonComponent>(entity);
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

		auto it = m_graphicsEntities.find(entity);
		if (it == m_graphicsEntities.end())
			return;

		GraphicsEntity* graphicsEntity = it->second;
		graphicsEntity->skeletonInstanceIndex = NoInstance;
	}

	void RenderSystem::OnSkeletonDestroy([[maybe_unused]] entt::registry& registry, entt::entity entity)
	{
		assert(&m_registry == &registry);

		auto it = m_graphicsEntities.find(entity);
		if (it == m_graphicsEntities.end())
			return;

		GraphicsEntity* graphicsEntity = it->second;

		m_pipeline->UnregisterSkeleton(graphicsEntity->skeletonInstanceIndex);
		graphicsEntity->skeletonInstanceIndex = NoInstance;
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
		for (CameraEntity* cameraEntity : m_invalidatedCameraNode)
		{
			entt::entity entity = cameraEntity->entity;

			const NodeComponent& entityNode = m_registry.get<const NodeComponent>(entity);
			CameraComponent& entityCamera = m_registry.get<CameraComponent>(entity);

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

	void RenderSystem::UpdateObservers()
	{
		m_cameraConstructObserver.each([&](entt::entity entity)
		{
			CameraComponent& entityCamera = m_registry.get<CameraComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			std::size_t poolIndex;
			CameraEntity* cameraEntity = m_cameraEntityPool.Allocate(poolIndex);
			cameraEntity->poolIndex = poolIndex;
			cameraEntity->entity = entity;
			cameraEntity->viewerIndex = m_pipeline->RegisterViewer(&entityCamera, entityCamera.GetRenderOrder());
			cameraEntity->onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, cameraEntity](const Node* /*node*/)
			{
				m_invalidatedCameraNode.insert(cameraEntity);
			});

			m_invalidatedCameraNode.insert(cameraEntity);

			assert(m_cameraEntities.find(entity) == m_cameraEntities.end());
			m_cameraEntities.emplace(entity, cameraEntity);
		});

		m_graphicsConstructObserver.each([&](entt::entity entity)
		{
			GraphicsComponent& entityGfx = m_registry.get<GraphicsComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			std::size_t poolIndex;
			GraphicsEntity* graphicsEntity = m_graphicsEntityPool.Allocate(poolIndex);
			graphicsEntity->entity = entity;
			graphicsEntity->poolIndex = poolIndex;
			graphicsEntity->renderableIndices.fill(NoInstance);
			graphicsEntity->skeletonInstanceIndex = NoInstance; //< will be set in skeleton observer
			graphicsEntity->worldInstanceIndex = m_pipeline->RegisterWorldInstance(entityGfx.GetWorldInstance());
			graphicsEntity->onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, graphicsEntity](const Node* /*node*/)
			{
				m_invalidatedGfxWorldNode.insert(graphicsEntity);
			});

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

			assert(m_graphicsEntities.find(entity) == m_graphicsEntities.end());
			m_graphicsEntities.emplace(entity, graphicsEntity);
		});

		m_lightConstructObserver.each([&](entt::entity entity)
		{
			LightComponent& entityLight = m_registry.get<LightComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			std::size_t poolIndex;
			LightEntity* lightEntity = m_lightEntityPool.Allocate(poolIndex);
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

			assert(m_lightEntities.find(entity) == m_lightEntities.end());
			m_lightEntities.emplace(entity, lightEntity);
		});

		m_sharedSkeletonConstructObserver.each([&](entt::entity entity)
		{
			GraphicsEntity* graphicsEntity = Retrieve(m_graphicsEntities, entity);

			SharedSkeletonComponent& skeletonComponent = m_registry.get<SharedSkeletonComponent>(entity);
			const std::shared_ptr<Skeleton>& skeleton = skeletonComponent.GetSkeleton();

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
		});

		m_skeletonConstructObserver.each([&](entt::entity entity)
		{
			GraphicsEntity* graphicsEntity = Retrieve(m_graphicsEntities, entity);

			SkeletonComponent& skeletonComponent = m_registry.get<SkeletonComponent>(entity);
			const std::shared_ptr<Skeleton>& skeleton = skeletonComponent.GetSkeleton();

			graphicsEntity->skeletonInstanceIndex = m_pipeline->RegisterSkeleton(std::make_shared<SkeletonInstance>(skeleton));

			for (std::size_t renderableIndex : graphicsEntity->renderableIndices)
			{
				if (renderableIndex != NoInstance)
					m_pipeline->UpdateRenderableSkeletonInstance(renderableIndex, graphicsEntity->skeletonInstanceIndex);
			}
		});
	}
}
