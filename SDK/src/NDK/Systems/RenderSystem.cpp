// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/RenderSystem.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	RenderSystem::RenderSystem()
	{
	}

	void RenderSystem::Update(float elapsedTime)
	{
		UpdateShadowMaps();

		for (const Ndk::EntityHandle& camera : m_cameras)
		{
			CameraComponent& camComponent = camera->GetComponent<CameraComponent>();
			camComponent.ApplyView();

			NzAbstractRenderQueue* renderQueue = m_renderTechnique.GetRenderQueue();
			renderQueue->Clear();

			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

				graphicsComponent.AddToRenderQueue(renderQueue);
			}

			for (const Ndk::EntityHandle& light : m_lights)
			{
				LightComponent& lightComponent = light->GetComponent<LightComponent>();
				NodeComponent& lightNode = light->GetComponent<NodeComponent>();

				lightComponent.AddToRenderQueue(renderQueue, lightNode.GetTransformMatrix());
			}

			NzColorBackground background;

			NzSceneData sceneData;
			sceneData.ambientColor = NzColor(25, 25, 25);
			sceneData.background = &background;
			sceneData.viewer = &camComponent;

			m_renderTechnique.Draw(sceneData);
		}
	}

	void RenderSystem::OnEntityRemoved(Entity* entity)
	{
		m_cameras.Remove(entity);
		m_drawables.Remove(entity);
		m_lights.Remove(entity);
	}

	void RenderSystem::OnEntityValidation(Entity* entity, bool justAdded)
	{
		if (entity->HasComponent<CameraComponent>() && entity->HasComponent<NodeComponent>())
		{
			m_cameras.Insert(entity);
			std::sort(m_cameras.begin(), m_cameras.end(), [](const EntityHandle& handle1, const EntityHandle& handle2)
			{
				return handle1->GetComponent<CameraComponent>().GetLayer() < handle2->GetComponent<CameraComponent>().GetLayer();
			});
		}
		else
			m_cameras.Remove(entity);

		if (entity->HasComponent<GraphicsComponent>() && entity->HasComponent<NodeComponent>())
			m_drawables.Insert(entity);
		else
			m_drawables.Remove(entity);

		if (entity->HasComponent<LightComponent>() && entity->HasComponent<NodeComponent>())
			m_lights.Insert(entity);
		else
			m_lights.Remove(entity);
	}

	void RenderSystem::UpdateShadowMaps()
	{
		if (!m_shadowRT.IsValid())
			m_shadowRT.Create();

		for (const Ndk::EntityHandle& light : m_lights)
		{
			LightComponent& lightComponent = light->GetComponent<LightComponent>();
			NodeComponent& lightNode = light->GetComponent<NodeComponent>();

			if (!lightComponent.IsShadowCastingEnabled() || lightComponent.GetLightType() != nzLightType_Spot)
				continue;

			NzVector2ui shadowMapSize(lightComponent.GetShadowMap()->GetSize());

			m_shadowRT.AttachTexture(nzAttachmentPoint_Depth, 0, lightComponent.GetShadowMap());

			///TODO: Cache the matrices in the light?
			NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(lightComponent.GetOuterAngle(), 1.f, 1.f, 1000.f));
			NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::ViewMatrix(lightNode.GetPosition(), lightNode.GetRotation()));
			NzRenderer::SetTarget(&m_shadowRT);
			NzRenderer::SetViewport(NzRecti(0, 0, shadowMapSize.x, shadowMapSize.y));

			NzAbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
			renderQueue->Clear();

			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

				graphicsComponent.AddToRenderQueue(renderQueue);
			}

			NzSceneData sceneData;
			sceneData.ambientColor = NzColor(0, 0, 0);
			sceneData.background = nullptr;
			sceneData.viewer = nullptr; //< Depth technique doesn't require any viewer

			m_shadowTechnique.Draw(sceneData);
		}
	}

	SystemIndex RenderSystem::systemIndex;
}
