// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
	RenderSystem::RenderSystem(flecs::world& world) :
	m_world(world),
	m_cameraEntityPool(8),
	m_graphicsEntityPool(1024),
	m_lightEntityPool(32)
	{
		Flecs::EnsureInit();

		m_world.observer<CameraComponent, NodeComponent>()
			.event(flecs::Monitor)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, CameraComponent& entityCamera, NodeComponent& entityNode)
			{
				flecs::entity entity = it.entity(i);
				if (it.event() == flecs::OnAdd)
				{
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
				}
				else if (it.event() == flecs::OnRemove)
				{
					auto camIt = m_cameraEntities.find(entity);
					if (camIt == m_cameraEntities.end())
						return;

					CameraEntity* cameraEntity = camIt->second;

					m_cameraEntities.erase(entity);
					m_invalidatedCameraNode.erase(cameraEntity);
					m_pipeline->UnregisterViewer(cameraEntity->viewerIndex);

					m_cameraEntityPool.Free(cameraEntity->poolIndex);
				}
			});

		m_world.observer<GraphicsComponent, NodeComponent>()
			.event(flecs::Monitor)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, GraphicsComponent& entityGfx, NodeComponent& entityNode)
			{
				flecs::entity entity = it.entity(i);
				if (it.event() == flecs::OnAdd)
				{
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
						UpdateGraphicsVisibility(graphicsEntity, entityGfx, true);

					assert(m_graphicsEntities.find(entity) == m_graphicsEntities.end());
					m_graphicsEntities.emplace(entity, graphicsEntity);
				}
				else if (it.event() == flecs::OnRemove)
				{
					auto gfxIt = m_graphicsEntities.find(entity);
					if (gfxIt == m_graphicsEntities.end())
						return;

					GraphicsEntity* graphicsEntity = gfxIt->second;

					m_graphicsEntities.erase(entity);
					m_invalidatedGfxWorldNode.erase(graphicsEntity);

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
			});

		m_world.observer<LightComponent, NodeComponent>()
			.event(flecs::Monitor)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, LightComponent& entityLight, NodeComponent& entityNode)
			{
				flecs::entity entity = it.entity(i);
				if (it.event() == flecs::OnAdd)
				{
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
						UpdateLightVisibility(lightEntity, entityLight, true);

					assert(m_lightEntities.find(entity) == m_lightEntities.end());
					m_lightEntities.emplace(entity, lightEntity);
				}
				else if (it.event() == flecs::OnRemove)
				{
					auto lightIt = m_lightEntities.find(entity);
					if (lightIt == m_lightEntities.end())
						return;

					LightEntity* lightEntity = lightIt->second;

					m_lightEntities.erase(entity);
					m_invalidatedLightWorldNode.erase(lightEntity);

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
			});

		m_world.observer<GraphicsComponent, NodeComponent, SharedSkeletonComponent>()
			.without<SkeletonComponent>()
			.event(flecs::Monitor)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, GraphicsComponent&, NodeComponent&, SharedSkeletonComponent& skeletonComponent)
			{
				flecs::entity entity = it.entity(i);
				if (it.event() == flecs::OnAdd)
				{
					GraphicsEntity* graphicsEntity = Retrieve(m_graphicsEntities, entity);

					const std::shared_ptr<Skeleton>& skeleton = skeletonComponent.GetSkeleton();

					if (auto sharedSkeletonIt = m_sharedSkeletonInstances.find(skeleton.get()); sharedSkeletonIt == m_sharedSkeletonInstances.end())
					{
						SharedSkeleton& sharedSkeleton = m_sharedSkeletonInstances[skeleton.get()];
						sharedSkeleton.skeletonInstanceIndex = m_pipeline->RegisterSkeleton(std::make_shared<SkeletonInstance>(skeleton));
						sharedSkeleton.useCount = 1;

						graphicsEntity->skeletonInstanceIndex = sharedSkeleton.skeletonInstanceIndex;
					}
					else
					{
						sharedSkeletonIt->second.useCount++;
						graphicsEntity->skeletonInstanceIndex = sharedSkeletonIt->second.skeletonInstanceIndex;
					}

					for (std::size_t renderableIndex : graphicsEntity->renderableIndices)
					{
						if (renderableIndex != NoInstance)
							m_pipeline->UpdateRenderableSkeletonInstance(renderableIndex, graphicsEntity->skeletonInstanceIndex);
					}
				}
				else if (it.event() == flecs::OnRemove)
				{
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

					auto gfxIt = m_graphicsEntities.find(entity);
					if (gfxIt == m_graphicsEntities.end())
						return;

					GraphicsEntity* graphicsEntity = gfxIt->second;
					graphicsEntity->skeletonInstanceIndex = NoInstance;
				}
			});

		m_world.observer<GraphicsComponent, NodeComponent, SkeletonComponent>()
			.without<SharedSkeletonComponent>()
			.event(flecs::Monitor)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, GraphicsComponent&, NodeComponent&, SkeletonComponent& skeletonComponent)
			{
				flecs::entity entity = it.entity(i);
				if (it.event() == flecs::OnAdd)
				{
					GraphicsEntity* graphicsEntity = Retrieve(m_graphicsEntities, entity);

					const std::shared_ptr<Skeleton>& skeleton = skeletonComponent.GetSkeleton();

					graphicsEntity->skeletonInstanceIndex = m_pipeline->RegisterSkeleton(std::make_shared<SkeletonInstance>(skeleton));

					for (std::size_t renderableIndex : graphicsEntity->renderableIndices)
					{
						if (renderableIndex != NoInstance)
							m_pipeline->UpdateRenderableSkeletonInstance(renderableIndex, graphicsEntity->skeletonInstanceIndex);
					}
				}
				else if (it.event() == flecs::OnRemove)
				{
					auto gfxIt = m_graphicsEntities.find(entity);
					if (gfxIt == m_graphicsEntities.end())
						return;

					GraphicsEntity* graphicsEntity = gfxIt->second;

					m_pipeline->UnregisterSkeleton(graphicsEntity->skeletonInstanceIndex);
					graphicsEntity->skeletonInstanceIndex = NoInstance;
				}
			});

		m_pipeline = std::make_unique<DefaultFramePipeline>(m_elementRegistry);
	}

	RenderSystem::~RenderSystem()
	{
		m_cameraConstructObserver.destruct();
		m_graphicsConstructObserver.destruct();
		m_lightConstructObserver.destruct();
		m_sharedSkeletonConstructObserver.destruct();
		m_skeletonConstructObserver.destruct();
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

			if (!m_cameraEntities.empty())
				m_pipeline->Render(frame);

			frame.Present();
		};

		for (auto& swapchainPtr : m_windowSwapchains)
			HandleSwapchain(*swapchainPtr);

		for (WindowSwapchain& swapchain : m_externalSwapchains)
			HandleSwapchain(swapchain);
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
			flecs::entity entity = cameraEntity->entity;
			entity.get([&](const NodeComponent& entityNode, CameraComponent& entityCamera)
			{
				Vector3f cameraPosition = entityNode.GetGlobalPosition();

				ViewerInstance& viewerInstance = entityCamera.GetViewerInstance();
				viewerInstance.UpdateEyePosition(cameraPosition);
				viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(cameraPosition, entityNode.GetGlobalRotation()));
			});
		}
		m_invalidatedCameraNode.clear();

		for (GraphicsEntity* graphicsEntity : m_invalidatedGfxWorldNode)
		{
			flecs::entity entity = graphicsEntity->entity;
			entity.get([&](const NodeComponent& entityNode, GraphicsComponent& entityGraphics)
			{
				const WorldInstancePtr& worldInstance = entityGraphics.GetWorldInstance();
				worldInstance->UpdateWorldMatrix(entityNode.GetTransformMatrix());
			});
		}
		m_invalidatedGfxWorldNode.clear();

		for (LightEntity* lightEntity : m_invalidatedLightWorldNode)
		{
			flecs::entity entity = lightEntity->entity;
			entity.get([&](const NodeComponent& entityNode, LightComponent& entityLight)
			{
				const Vector3f& position = entityNode.GetGlobalPosition();
				const Quaternionf& rotation = entityNode.GetGlobalRotation();
				const Vector3f& scale = entityNode.GetGlobalScale();

				for (const auto& lightEntry : entityLight.GetLights())
				{
					if (!lightEntry.light)
						continue;

					lightEntry.light->UpdateTransform(position, rotation, scale);
				}
			});
		}
		m_invalidatedLightWorldNode.clear();
	}
}
