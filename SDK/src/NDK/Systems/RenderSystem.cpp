// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/RenderSystem.hpp>
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
		SetDefaultBackground(NzColorBackground::New());
		SetUpdateRate(0.f);
	}

	void RenderSystem::OnEntityRemoved(Entity* entity)
	{
		m_cameras.Remove(entity);
		m_drawables.Remove(entity);
		m_lights.Remove(entity);
	}

	void RenderSystem::OnEntityValidation(Entity* entity, bool justAdded)
	{
		NazaraUnused(justAdded);

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

	void RenderSystem::OnUpdate(float elapsedTime)
	{
		NazaraUnused(elapsedTime);

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

			NzSceneData sceneData;
			sceneData.ambientColor = NzColor(25, 25, 25);
			sceneData.background = m_background;
			sceneData.viewer = &camComponent;

			m_renderTechnique.Draw(sceneData);
		}
	}

	void RenderSystem::UpdateShadowMaps()
	{
		if (!m_shadowRT.IsValid())
			m_shadowRT.Create();

		NzSceneData dummySceneData;
		dummySceneData.ambientColor = NzColor(0, 0, 0);
		dummySceneData.background = nullptr;
		dummySceneData.viewer = nullptr; //< Depth technique doesn't require any viewer

		for (const Ndk::EntityHandle& light : m_lights)
		{
			LightComponent& lightComponent = light->GetComponent<LightComponent>();
			NodeComponent& lightNode = light->GetComponent<NodeComponent>();

			if (!lightComponent.IsShadowCastingEnabled() || lightComponent.GetLightType() == nzLightType_Directional)
				continue;

			NzVector2ui shadowMapSize(lightComponent.GetShadowMap()->GetSize());

			switch (lightComponent.GetLightType())
			{
				case nzLightType_Point:
				{
					static NzQuaternionf rotations[6] =
					{
						NzQuaternionf::RotationBetween(NzVector3f::Forward(),  NzVector3f::UnitX()), // nzCubemapFace_PositiveX
						NzQuaternionf::RotationBetween(NzVector3f::Forward(), -NzVector3f::UnitX()), // nzCubemapFace_NegativeX
						NzQuaternionf::RotationBetween(NzVector3f::Forward(), -NzVector3f::UnitY()), // nzCubemapFace_PositiveY
						NzQuaternionf::RotationBetween(NzVector3f::Forward(),  NzVector3f::UnitY()), // nzCubemapFace_NegativeY
						NzQuaternionf::RotationBetween(NzVector3f::Forward(), -NzVector3f::UnitZ()), // nzCubemapFace_PositiveZ
						NzQuaternionf::RotationBetween(NzVector3f::Forward(),  NzVector3f::UnitZ())  // nzCubemapFace_NegativeZ
					};

					for (unsigned int face = 0; face < 6; ++face)
					{
						m_shadowRT.AttachTexture(nzAttachmentPoint_Depth, 0, lightComponent.GetShadowMap(), face);
						NzRenderer::SetTarget(&m_shadowRT);
						NzRenderer::SetViewport(NzRecti(0, 0, shadowMapSize.x, shadowMapSize.y));

						///TODO: Cache the matrices in the light?
						NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzFromDegrees(90.f), 1.f, 0.1f, lightComponent.GetRadius()));
						NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::ViewMatrix(lightNode.GetPosition(), rotations[face]));

						NzAbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
						renderQueue->Clear();

						///TODO: Culling
						for (const Ndk::EntityHandle& drawable : m_drawables)
						{
							GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
							NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

							graphicsComponent.AddToRenderQueue(renderQueue);
						}

						m_shadowTechnique.Draw(dummySceneData);
					}
					break;
				}

				case nzLightType_Spot:
					m_shadowRT.AttachTexture(nzAttachmentPoint_Depth, 0, lightComponent.GetShadowMap());
					NzRenderer::SetTarget(&m_shadowRT);
					NzRenderer::SetViewport(NzRecti(0, 0, shadowMapSize.x, shadowMapSize.y));

					///TODO: Cache the matrices in the light?
					NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(lightComponent.GetOuterAngle()*2.f, 1.f, 0.1f, lightComponent.GetRadius()));
					NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::ViewMatrix(lightNode.GetPosition(), lightNode.GetRotation()));

					NzAbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
					renderQueue->Clear();

					///TODO: Culling
					for (const Ndk::EntityHandle& drawable : m_drawables)
					{
						GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
						NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

						graphicsComponent.AddToRenderQueue(renderQueue);
					}

					m_shadowTechnique.Draw(dummySceneData);
					break;
			}
		}
	}

	SystemIndex RenderSystem::systemIndex;
}
