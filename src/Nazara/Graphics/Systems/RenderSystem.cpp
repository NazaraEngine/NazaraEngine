// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

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
	m_graphicsConstructObserver(registry, entt::collector.group<GraphicsComponent, NodeComponent>())
	{
		m_cameraDestroyConnection = registry.on_destroy<CameraComponent>().connect<&RenderSystem::OnCameraDestroy>(this);
		m_graphicsDestroyConnection = registry.on_destroy<GraphicsComponent>().connect<&RenderSystem::OnGraphicsDestroy>(this);
		m_nodeDestroyConnection = registry.on_destroy<NodeComponent>().connect<&RenderSystem::OnNodeDestroy>(this);

		m_pipeline = std::make_unique<ForwardFramePipeline>();
	}

	RenderSystem::~RenderSystem()
	{
		m_cameraConstructObserver.disconnect();
		m_graphicsConstructObserver.disconnect();
		m_cameraDestroyConnection.release();
		m_graphicsDestroyConnection.release();
		m_nodeDestroyConnection.release();
	}

	void RenderSystem::Render(entt::registry& registry, RenderFrame& renderFrame)
	{
		m_cameraConstructObserver.each([&](entt::entity entity)
		{
			CameraComponent& entityCamera = registry.get<CameraComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

			m_pipeline->RegisterViewer(&entityCamera);

			m_invalidatedCameraNode.insert(entity);

			assert(m_cameraEntities.find(entity) == m_cameraEntities.end());
			auto& cameraEntity = m_cameraEntities[entity];
			cameraEntity.onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Node* node)
			{
				m_invalidatedCameraNode.insert(entity);
			});
		});
		
		m_graphicsConstructObserver.each([&](entt::entity entity)
		{
			GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);
			NodeComponent& entityNode = registry.get<NodeComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGfx.GetWorldInstance();
			for (const auto& renderable : entityGfx.GetRenderables())
				m_pipeline->RegisterInstancedDrawable(worldInstance, renderable.get());

			m_invalidatedWorldNode.insert(entity);

			assert(m_graphicsEntities.find(entity) == m_graphicsEntities.end());
			auto& graphicsEntity = m_graphicsEntities[entity];
			graphicsEntity.onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Node* node)
			{
				m_invalidatedWorldNode.insert(entity);
			});

			graphicsEntity.onRenderableAttached.Connect(entityGfx.OnRenderableAttached, [this](GraphicsComponent* gfx, const std::shared_ptr<InstancedRenderable>& renderable)
			{
				const WorldInstancePtr& worldInstance = gfx->GetWorldInstance();
				m_pipeline->RegisterInstancedDrawable(worldInstance, renderable.get());
			});

			graphicsEntity.onRenderableDetach.Connect(entityGfx.OnRenderableDetach, [this](GraphicsComponent* gfx, const std::shared_ptr<InstancedRenderable>& renderable)
			{
				const WorldInstancePtr& worldInstance = gfx->GetWorldInstance();
				m_pipeline->UnregisterInstancedDrawable(worldInstance, renderable.get());
			});
		});

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
		m_invalidatedWorldNode.erase(entity);

		GraphicsComponent& entityGfx = registry.get<GraphicsComponent>(entity);
		const WorldInstancePtr& worldInstance = entityGfx.GetWorldInstance();
		for (const auto& renderable : entityGfx.GetRenderables())
			m_pipeline->UnregisterInstancedDrawable(worldInstance, renderable.get());
	}

	void RenderSystem::OnNodeDestroy(entt::registry& registry, entt::entity entity)
	{
		if (registry.try_get<CameraComponent>(entity))
			OnCameraDestroy(registry, entity);

		if (registry.try_get<GraphicsComponent>(entity))
			OnGraphicsDestroy(registry, entity);
	}

	void RenderSystem::UpdateInstances(entt::registry& registry)
	{
		for (entt::entity entity : m_invalidatedCameraNode)
		{
			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			CameraComponent& entityCamera = registry.get<CameraComponent>(entity);

			ViewerInstance& viewerInstance = entityCamera.GetViewerInstance();
			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::ViewMatrix(entityNode.GetPosition(CoordSys::Global), entityNode.GetRotation(CoordSys::Global)));

			m_pipeline->InvalidateViewer(&entityCamera);
		}
		m_invalidatedCameraNode.clear();

		for (entt::entity entity : m_invalidatedWorldNode)
		{
			const NodeComponent& entityNode = registry.get<const NodeComponent>(entity);
			GraphicsComponent& entityGraphics = registry.get<GraphicsComponent>(entity);

			const WorldInstancePtr& worldInstance = entityGraphics.GetWorldInstance();
			worldInstance->UpdateWorldMatrix(entityNode.GetTransformMatrix());

			m_pipeline->InvalidateWorldInstance(worldInstance.get());
		}
		m_invalidatedWorldNode.clear();
	}
}
