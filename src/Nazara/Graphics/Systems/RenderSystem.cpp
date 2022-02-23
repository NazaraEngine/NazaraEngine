// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Systems/RenderSystem.hpp>
#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Components/CameraComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	RenderSystem::RenderSystem(entt::registry& registry) :
	m_cameraConstructObserver(registry, entt::collector.group<CameraComponent, NodeComponent>()),
	m_graphicsConstructObserver(registry, entt::collector.group<GraphicsComponent, NodeComponent>()),
	m_lightConstructObserver(registry, entt::collector.group<LightComponent, NodeComponent>()),
	m_cameraEntityPool(8),
	m_graphicsEntityPool(1024),
	m_lightEntityPool(32)
	{
		m_cameraDestroyConnection = registry.on_destroy<CameraComponent>().connect<&RenderSystem::OnCameraDestroy>(this);
		m_graphicsDestroyConnection = registry.on_destroy<GraphicsComponent>().connect<&RenderSystem::OnGraphicsDestroy>(this);
		m_lightDestroyConnection = registry.on_destroy<LightComponent>().connect<&RenderSystem::OnLightDestroy>(this);
		m_nodeDestroyConnection = registry.on_destroy<NodeComponent>().connect<&RenderSystem::OnNodeDestroy>(this);

		m_pipeline = std::make_unique<ForwardFramePipeline>();
	}

	RenderSystem::~RenderSystem()
	{
		m_cameraConstructObserver.disconnect();
		m_graphicsConstructObserver.disconnect();
		m_lightConstructObserver.disconnect();
		m_cameraDestroyConnection.release();
		m_graphicsDestroyConnection.release();
		m_lightDestroyConnection.release();
		m_nodeDestroyConnection.release();
	}

	void RenderSystem::Render(entt::registry& registry, RenderFrame& renderFrame)
	{
		m_cameraConstructObserver.each([&](entt::entity entity)
		{
			CameraComponent& entityCamera = registry.get<CameraComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

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
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

			std::size_t poolIndex;
			GraphicsEntity* graphicsEntity = m_graphicsEntityPool.Allocate(poolIndex);
			graphicsEntity->entity = entity;
			graphicsEntity->poolIndex = poolIndex;
			graphicsEntity->renderableIndices.fill(std::numeric_limits<std::size_t>::max());
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
				graphicsEntity->renderableIndices[renderableIndex] = m_pipeline->RegisterRenderable(graphicsEntity->worldInstanceIndex, renderableEntry.renderable.get(), renderableEntry.renderMask, gfx->GetScissorBox());
			});

			graphicsEntity->onRenderableDetach.Connect(entityGfx.OnRenderableDetach, [this, graphicsEntity](GraphicsComponent* gfx, std::size_t renderableIndex)
			{
				if (!gfx->IsVisible())
					return;

				m_pipeline->UnregisterRenderable(graphicsEntity->renderableIndices[renderableIndex]);
			});

			graphicsEntity->onScissorBoxUpdate.Connect(entityGfx.OnScissorBoxUpdate, [this, graphicsEntity](GraphicsComponent* gfx, const Recti& scissorBox)
			{
				if (!gfx->IsVisible())
					return;

				for (std::size_t renderableIndex = 0; renderableIndex < LightComponent::MaxLightCount; ++renderableIndex)
				{
					const auto& renderableEntry = gfx->GetRenderableEntry(renderableIndex);
					if (!renderableEntry.renderable)
						continue;

					m_pipeline->UpdateRenderableScissorBox(graphicsEntity->renderableIndices[renderableIndex], scissorBox);
				}
			});

			graphicsEntity->onVisibilityUpdate.Connect(entityGfx.OnVisibilityUpdate, [this, graphicsEntity](GraphicsComponent* /*gfx*/, bool isVisible)
			{
				if (isVisible)
				{
					m_newlyHiddenGfxEntities.erase(graphicsEntity);
					m_newlyVisibleGfxEntities.insert(graphicsEntity);
				}
				else
				{
					m_newlyHiddenGfxEntities.insert(graphicsEntity);
					m_newlyVisibleGfxEntities.erase(graphicsEntity);
				}
			});
			m_invalidatedGfxWorldNode.insert(graphicsEntity);

			if (entityGfx.IsVisible())
			{
				for (std::size_t renderableIndex = 0; renderableIndex < LightComponent::MaxLightCount; ++renderableIndex)
				{
					const auto& renderableEntry = entityGfx.GetRenderableEntry(renderableIndex);
					if (!renderableEntry.renderable)
						continue;

					graphicsEntity->renderableIndices[renderableIndex] = m_pipeline->RegisterRenderable(graphicsEntity->worldInstanceIndex, renderableEntry.renderable.get(), renderableEntry.renderMask, entityGfx.GetScissorBox());
				}
			}

			assert(m_graphicsEntities.find(entity) == m_graphicsEntities.end());
			m_graphicsEntities.emplace(entity, graphicsEntity);
		});

		m_lightConstructObserver.each([&](entt::entity entity)
		{
			LightComponent& entityLight = registry.get<LightComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

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
				lightEntity->lightIndices[lightIndex] =	m_pipeline->RegisterLight(lightEntry.light, lightEntry.renderMask);
			});

			lightEntity->onLightDetach.Connect(entityLight.OnLightDetach, [this, lightEntity](LightComponent* light, std::size_t lightIndex)
			{
				if (!light->IsVisible())
					return;

				m_pipeline->UnregisterLight(lightEntity->lightIndices[lightIndex]);
			});

			lightEntity->onVisibilityUpdate.Connect(entityLight.OnVisibilityUpdate, [this, lightEntity](LightComponent* /*light*/, bool isVisible)
			{
				if (isVisible)
				{
					m_newlyHiddenLightEntities.erase(lightEntity);
					m_newlyVisibleLightEntities.insert(lightEntity);
				}
				else
				{
					m_newlyHiddenLightEntities.insert(lightEntity);
					m_newlyVisibleLightEntities.erase(lightEntity);
				}
			});

			m_invalidatedLightWorldNode.insert(lightEntity);

			if (entityLight.IsVisible())
			{
				for (std::size_t lightIndex = 0; lightIndex < LightComponent::MaxLightCount; ++lightIndex)
				{
					const auto& lightEntry = entityLight.GetLightEntry(lightIndex);
					if (!lightEntry.light)
						continue;

					lightEntity->lightIndices[lightIndex] = m_pipeline->RegisterLight(lightEntry.light, lightEntry.renderMask);
				}
			}

			assert(m_lightEntities.find(entity) == m_lightEntities.end());
			m_lightEntities.emplace(entity, lightEntity);
		});

		UpdateVisibility(registry);
		UpdateInstances(registry);

		m_pipeline->Render(renderFrame);
	}

	void RenderSystem::OnCameraDestroy(entt::registry& /*registry*/, entt::entity entity)
	{
		auto it = m_cameraEntities.find(entity);
		if (it == m_cameraEntities.end())
			return;

		CameraEntity* cameraEntity = it->second;

		m_cameraEntities.erase(it);
		m_invalidatedCameraNode.erase(cameraEntity);
		m_pipeline->UnregisterViewer(cameraEntity->viewerIndex);

		m_cameraEntityPool.Free(cameraEntity->poolIndex);
	}

	void RenderSystem::OnGraphicsDestroy(entt::registry& registry, entt::entity entity)
	{
		auto it = m_graphicsEntities.find(entity);
		if (it == m_graphicsEntities.end())
			return;

		GraphicsEntity* graphicsEntity = it->second;

		m_graphicsEntities.erase(entity);
		m_invalidatedGfxWorldNode.erase(graphicsEntity);
		m_newlyHiddenGfxEntities.erase(graphicsEntity);
		m_newlyVisibleGfxEntities.erase(graphicsEntity);

		GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);
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

	void RenderSystem::OnLightDestroy(entt::registry& registry, entt::entity entity)
	{
		auto it = m_lightEntities.find(entity);
		if (it == m_lightEntities.end())
			return;

		LightEntity* lightEntity = it->second;

		m_lightEntities.erase(entity);
		m_invalidatedLightWorldNode.erase(lightEntity);
		m_newlyHiddenLightEntities.erase(lightEntity);
		m_newlyVisibleLightEntities.erase(lightEntity);

		LightComponent& entityLight = registry.get<LightComponent>(entity);
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
		if (registry.try_get<CameraComponent>(entity))
			OnCameraDestroy(registry, entity);

		if (registry.try_get<GraphicsComponent>(entity))
			OnGraphicsDestroy(registry, entity);

		if (registry.try_get<LightComponent>(entity))
			OnLightDestroy(registry, entity);
	}

	void RenderSystem::UpdateInstances(entt::registry& registry)
	{
		for (CameraEntity* cameraEntity : m_invalidatedCameraNode)
		{
			entt::entity entity = cameraEntity->entity;

			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			CameraComponent& entityCamera = registry.get<CameraComponent>(entity);

			Vector3f cameraPosition = entityNode.GetPosition(CoordSys::Global);
			
			ViewerInstance& viewerInstance = entityCamera.GetViewerInstance();
			viewerInstance.UpdateEyePosition(cameraPosition);
			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::ViewMatrix(cameraPosition, entityNode.GetRotation(CoordSys::Global)));

			m_pipeline->InvalidateViewer(cameraEntity->viewerIndex);
		}
		m_invalidatedCameraNode.clear();

		for (GraphicsEntity* graphicsEntity : m_invalidatedGfxWorldNode)
		{
			entt::entity entity = graphicsEntity->entity;

			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			GraphicsComponent& entityGraphics = registry.get<GraphicsComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGraphics.GetWorldInstance();
			worldInstance->UpdateWorldMatrix(entityNode.GetTransformMatrix());

			m_pipeline->InvalidateWorldInstance(graphicsEntity->worldInstanceIndex);
		}
		m_invalidatedGfxWorldNode.clear();

		for (LightEntity* lightEntity : m_invalidatedLightWorldNode)
		{
			entt::entity entity = lightEntity->entity;

			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			LightComponent& entityLight = registry.get<LightComponent>(entity);

			const Vector3f& position = entityNode.GetPosition(CoordSys::Global);
			const Quaternionf& rotation = entityNode.GetRotation(CoordSys::Global);
			const Vector3f& scale = entityNode.GetScale(CoordSys::Global);

			for (const auto& lightEntry : entityLight.GetLights())
			{
				if (!lightEntry.light)
					continue;

				lightEntry.light->UpdateTransform(position, rotation, scale);
			}
		}
		m_invalidatedLightWorldNode.clear();
	}

	void RenderSystem::UpdateVisibility(entt::registry& registry)
	{
		// Unregister drawable for hidden entities
		for (GraphicsEntity* graphicsEntity : m_newlyHiddenGfxEntities)
		{
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(graphicsEntity->entity);

			for (std::size_t renderableIndex = 0; renderableIndex < GraphicsComponent::MaxRenderableCount; ++renderableIndex)
			{
				const auto& renderableEntry = entityGfx.GetRenderableEntry(renderableIndex);
				if (!renderableEntry.renderable)
					continue;

				m_pipeline->UnregisterRenderable(graphicsEntity->renderableIndices[renderableIndex]);
			}
		}
		m_newlyHiddenGfxEntities.clear();

		// Register drawable for newly visible entities
		for (GraphicsEntity* graphicsEntity : m_newlyVisibleGfxEntities)
		{
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(graphicsEntity->entity);

			for (std::size_t renderableIndex = 0; renderableIndex < GraphicsComponent::MaxRenderableCount; ++renderableIndex)
			{
				const auto& renderableEntry = entityGfx.GetRenderableEntry(renderableIndex);
				if (!renderableEntry.renderable)
					continue;

				graphicsEntity->renderableIndices[renderableIndex] = m_pipeline->RegisterRenderable(graphicsEntity->worldInstanceIndex, renderableEntry.renderable.get(), renderableEntry.renderMask, entityGfx.GetScissorBox());
			}
		}
		m_newlyVisibleGfxEntities.clear();

		// Unregister lights for hidden entities
		for (LightEntity* lightEntity : m_newlyHiddenLightEntities)
		{
			LightComponent& entityLights = registry.get<LightComponent>(lightEntity->entity);

			for (std::size_t lightIndex = 0; lightIndex < LightComponent::MaxLightCount; ++lightIndex)
			{
				const auto& lightEntry = entityLights.GetLightEntry(lightIndex);
				if (!lightEntry.light)
					continue;

				m_pipeline->UnregisterLight(lightEntity->lightIndices[lightIndex]);
			}
		}
		m_newlyHiddenGfxEntities.clear();

		// Register lights for newly visible entities
		for (LightEntity* lightEntity : m_newlyVisibleLightEntities)
		{
			LightComponent& entityLights = registry.get<LightComponent>(lightEntity->entity);

			for (std::size_t renderableIndex = 0; renderableIndex < LightComponent::MaxLightCount; ++renderableIndex)
			{
				const auto& lightEntry = entityLights.GetLightEntry(renderableIndex);
				if (!lightEntry.light)
					continue;

				lightEntity->lightIndices[renderableIndex] = m_pipeline->RegisterLight(lightEntry.light, lightEntry.renderMask);
			}
		}
		m_newlyVisibleGfxEntities.clear();

		//FIXME: Handle light visibility
	}
}
