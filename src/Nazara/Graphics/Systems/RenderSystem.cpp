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
	m_lightConstructObserver(registry, entt::collector.group<LightComponent, NodeComponent>())
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

			m_pipeline->RegisterViewer(&entityCamera, entityCamera.GetRenderOrder());

			m_invalidatedCameraNode.insert(entity);

			assert(m_cameraEntities.find(entity) == m_cameraEntities.end());
			auto& cameraEntity = m_cameraEntities[entity];
			cameraEntity.onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Node* /*node*/)
			{
				m_invalidatedCameraNode.insert(entity);
			});
		});
		
		m_graphicsConstructObserver.each([&](entt::entity entity)
		{
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

			if (entityGfx.IsVisible())
			{
				const WorldInstancePtr& worldInstance = entityGfx.GetWorldInstance();
				for (const auto& renderableEntry : entityGfx.GetRenderables())
					m_pipeline->RegisterInstancedDrawable(worldInstance, renderableEntry.renderable.get(), renderableEntry.renderMask);
			}

			m_invalidatedGfxWorldNode.insert(entity);

			assert(m_graphicsEntities.find(entity) == m_graphicsEntities.end());
			auto& graphicsEntity = m_graphicsEntities[entity];
			graphicsEntity.onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Node* /*node*/)
			{
				m_invalidatedGfxWorldNode.insert(entity);
			});

			graphicsEntity.onRenderableAttached.Connect(entityGfx.OnRenderableAttached, [this](GraphicsComponent* gfx, const GraphicsComponent::Renderable& renderableEntry)
			{
				if (!gfx->IsVisible())
					return;

				const WorldInstancePtr& worldInstance = gfx->GetWorldInstance();
				m_pipeline->RegisterInstancedDrawable(worldInstance, renderableEntry.renderable.get(), renderableEntry.renderMask);
			});

			graphicsEntity.onRenderableDetach.Connect(entityGfx.OnRenderableDetach, [this](GraphicsComponent* gfx, const GraphicsComponent::Renderable& renderableEntry)
			{
				if (!gfx->IsVisible())
					return;

				const WorldInstancePtr& worldInstance = gfx->GetWorldInstance();
				m_pipeline->UnregisterInstancedDrawable(worldInstance, renderableEntry.renderable.get());
			});

			graphicsEntity.onVisibilityUpdate.Connect(entityGfx.OnVisibilityUpdate, [this, entity](GraphicsComponent* /*gfx*/, bool isVisible)
			{
				if (isVisible)
				{
					m_newlyHiddenGfxEntities.erase(entity);
					m_newlyVisibleGfxEntities.insert(entity);
				}
				else
				{
					m_newlyHiddenGfxEntities.insert(entity);
					m_newlyVisibleGfxEntities.erase(entity);
				}
			});
		});

		m_lightConstructObserver.each([&](entt::entity entity)
		{
			LightComponent& entityLight = registry.get<LightComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

			if (entityLight.IsVisible())
			{
				for (const auto& lightEntry : entityLight.GetLights())
					m_pipeline->RegisterLight(lightEntry.light, lightEntry.renderMask);
			}

			m_invalidatedLightWorldNode.insert(entity);

			assert(m_lightEntities.find(entity) == m_lightEntities.end());
			auto& lightEntity = m_lightEntities[entity];
			lightEntity.onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Node* /*node*/)
			{
				m_invalidatedLightWorldNode.insert(entity);
			});

			lightEntity.onLightAttached.Connect(entityLight.OnLightAttached, [this](LightComponent* light, const LightComponent::LightEntry& lightEntry)
			{
				if (!light->IsVisible())
					return;

				m_pipeline->RegisterLight(lightEntry.light, lightEntry.renderMask);
			});

			lightEntity.onLightDetach.Connect(entityLight.OnLightDetach, [this](LightComponent* light, const LightComponent::LightEntry& lightEntry)
			{
				if (!light->IsVisible())
					return;

				m_pipeline->UnregisterLight(lightEntry.light.get());
			});

			lightEntity.onVisibilityUpdate.Connect(entityLight.OnVisibilityUpdate, [this, entity](LightComponent* /*light*/, bool isVisible)
			{
				if (isVisible)
				{
					m_newlyHiddenLightEntities.erase(entity);
					m_newlyVisibleLightEntities.insert(entity);
				}
				else
				{
					m_newlyHiddenLightEntities.insert(entity);
					m_newlyVisibleLightEntities.erase(entity);
				}
			});
		});

		UpdateVisibility(registry);
		UpdateInstances(registry);

		m_pipeline->Render(renderFrame);
	}

	void RenderSystem::OnCameraDestroy(entt::registry& registry, entt::entity entity)
	{
		m_cameraEntities.erase(entity);
		m_invalidatedCameraNode.erase(entity);

		CameraComponent& entityCamera = registry.get<CameraComponent>(entity);
		m_pipeline->UnregisterViewer(&entityCamera);
	}

	void RenderSystem::OnGraphicsDestroy(entt::registry& registry, entt::entity entity)
	{
		m_graphicsEntities.erase(entity);
		m_invalidatedGfxWorldNode.erase(entity);
		m_newlyHiddenGfxEntities.erase(entity);
		m_newlyVisibleGfxEntities.erase(entity);

		GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);
		const WorldInstancePtr& worldInstance = entityGfx.GetWorldInstance();
		for (const auto& renderableEntry : entityGfx.GetRenderables())
			m_pipeline->UnregisterInstancedDrawable(worldInstance, renderableEntry.renderable.get());
	}

	void RenderSystem::OnLightDestroy(entt::registry& registry, entt::entity entity)
	{
		m_lightEntities.erase(entity);
		m_invalidatedLightWorldNode.erase(entity);
		m_newlyHiddenLightEntities.erase(entity);
		m_newlyVisibleLightEntities.erase(entity);

		LightComponent& entityLight = registry.get<LightComponent>(entity);
		for (const auto& lightEntry : entityLight.GetLights())
			m_pipeline->UnregisterLight(lightEntry.light.get());
	}

	void RenderSystem::OnNodeDestroy(entt::registry& registry, entt::entity entity)
	{
		m_newlyHiddenGfxEntities.erase(entity);
		m_newlyVisibleGfxEntities.erase(entity);
		m_newlyHiddenLightEntities.erase(entity);
		m_newlyVisibleLightEntities.erase(entity);

		if (registry.try_get<CameraComponent>(entity))
			OnCameraDestroy(registry, entity);

		if (registry.try_get<GraphicsComponent>(entity))
			OnGraphicsDestroy(registry, entity);

		if (registry.try_get<LightComponent>(entity))
			OnLightDestroy(registry, entity);
	}

	void RenderSystem::UpdateInstances(entt::registry& registry)
	{
		for (entt::entity entity : m_invalidatedCameraNode)
		{
			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			CameraComponent& entityCamera = registry.get<CameraComponent>(entity);

			Vector3f cameraPosition = entityNode.GetPosition(CoordSys::Global);
			
			ViewerInstance& viewerInstance = entityCamera.GetViewerInstance();
			viewerInstance.UpdateEyePosition(cameraPosition);
			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::ViewMatrix(cameraPosition, entityNode.GetRotation(CoordSys::Global)));

			m_pipeline->InvalidateViewer(&entityCamera);
		}
		m_invalidatedCameraNode.clear();

		for (entt::entity entity : m_invalidatedGfxWorldNode)
		{
			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			GraphicsComponent& entityGraphics = registry.get<GraphicsComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGraphics.GetWorldInstance();
			worldInstance->UpdateWorldMatrix(entityNode.GetTransformMatrix());

			m_pipeline->InvalidateWorldInstance(worldInstance.get());
		}
		m_invalidatedGfxWorldNode.clear();

		for (entt::entity entity : m_invalidatedLightWorldNode)
		{
			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			LightComponent& entityLight = registry.get<LightComponent>(entity);

			const Vector3f& position = entityNode.GetPosition(CoordSys::Global);
			const Quaternionf& rotation = entityNode.GetRotation(CoordSys::Global);
			const Vector3f& scale = entityNode.GetScale(CoordSys::Global);

			for (const auto& lightEntry : entityLight.GetLights())
				lightEntry.light->UpdateTransform(position, rotation, scale);
		}
		m_invalidatedLightWorldNode.clear();
	}

	void RenderSystem::UpdateVisibility(entt::registry& registry)
	{
		// Unregister drawable for hidden entities
		for (entt::entity entity : m_newlyHiddenGfxEntities)
		{
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGfx.GetWorldInstance();
			for (const auto& renderableEntry : entityGfx.GetRenderables())
				m_pipeline->UnregisterInstancedDrawable(worldInstance, renderableEntry.renderable.get());
		}
		m_newlyHiddenGfxEntities.clear();

		// Register drawable for newly visible entities
		for (entt::entity entity : m_newlyVisibleGfxEntities)
		{
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGfx.GetWorldInstance();
			for (const auto& renderableEntry : entityGfx.GetRenderables())
				m_pipeline->RegisterInstancedDrawable(worldInstance, renderableEntry.renderable.get(), renderableEntry.renderMask);
		}
		m_newlyVisibleGfxEntities.clear();
	}
}
