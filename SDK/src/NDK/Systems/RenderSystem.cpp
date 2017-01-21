// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/RenderSystem.hpp>
#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::RenderSystem
	* \brief NDK class that represents the rendering system
	*
	* \remark This system is enabled if the entity is a 'camera' with the trait: CameraComponent and NodeComponent
	* or a drawable element with trait: GraphicsComponent and NodeComponent
	* or a light element with trait: LightComponent and NodeComponent
	* or a set of particles with trait: ParticleGroupComponent
	*/

	/*!
	* \brief Constructs an RenderSystem object by default
	*/
	RenderSystem::RenderSystem() :
	m_coordinateSystemMatrix(Nz::Matrix4f::Identity()),
	m_coordinateSystemInvalidated(true),
	m_forceRenderQueueInvalidation(false)
	{
		ChangeRenderTechnique<Nz::ForwardRenderTechnique>();
		SetDefaultBackground(Nz::ColorBackground::New());
		SetUpdateOrder(100); //< Render last, after every movement is done
		SetUpdateRate(0.f);  //< We don't want any rate limit
	}

	/*!
	* \brief Operation to perform when an entity is removed
	*
	* \param entity Pointer to the entity
	*/
	void RenderSystem::OnEntityRemoved(Entity* entity)
	{
		m_forceRenderQueueInvalidation = true; //< Hackfix until lights and particles are handled by culling list

		m_cameras.Remove(entity);
		m_directionalLights.Remove(entity);
		m_drawables.Remove(entity);
		m_lights.Remove(entity);
		m_particleGroups.Remove(entity);
		m_pointSpotLights.Remove(entity);

		if (entity->HasComponent<GraphicsComponent>())
		{
			GraphicsComponent& gfxComponent = entity->GetComponent<GraphicsComponent>();
			gfxComponent.RemoveFromCullingList(&m_drawableCulling);
		}
	}

	/*!
	* \brief Operation to perform when entity is validated for the system
	*
	* \param entity Pointer to the entity
	* \param justAdded Is the entity newly added
	*/
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
		{
			m_drawables.Insert(entity);

			if (justAdded)
			{
				GraphicsComponent& gfxComponent = entity->GetComponent<GraphicsComponent>();
				gfxComponent.AddToCullingList(&m_drawableCulling);
			}
		}
		else
		{
			m_drawables.Remove(entity);

			if (entity->HasComponent<GraphicsComponent>())
			{
				GraphicsComponent& gfxComponent = entity->GetComponent<GraphicsComponent>();
				gfxComponent.RemoveFromCullingList(&m_drawableCulling);
			}
		}

		if (entity->HasComponent<LightComponent>() && entity->HasComponent<NodeComponent>())
		{
			m_forceRenderQueueInvalidation = true; //< Hackfix until lights and particles are handled by culling list

			LightComponent& lightComponent = entity->GetComponent<LightComponent>();
			if (lightComponent.GetLightType() == Nz::LightType_Directional)
			{
				m_directionalLights.Insert(entity);
				m_pointSpotLights.Remove(entity);
			}
			else
			{
				m_directionalLights.Remove(entity);
				m_pointSpotLights.Insert(entity);
			}

			m_lights.Insert(entity);
		}
		else
		{
			m_forceRenderQueueInvalidation = true; //< Hackfix until lights and particles are handled by culling list

			m_directionalLights.Remove(entity);
			m_lights.Remove(entity);
			m_pointSpotLights.Remove(entity);
		}

		if (entity->HasComponent<ParticleGroupComponent>())
		{
			m_forceRenderQueueInvalidation = true; //< Hackfix until lights and particles are handled by culling list

			m_particleGroups.Insert(entity);
		}
		else
		{
			m_forceRenderQueueInvalidation = true; //< Hackfix until lights and particles are handled by culling list

			m_particleGroups.Remove(entity);
		}
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void RenderSystem::OnUpdate(float /*elapsedTime*/)
	{
		// Invalidate every renderable if the coordinate system changed
		if (m_coordinateSystemInvalidated)
		{
			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				graphicsComponent.InvalidateTransformMatrix();
			}

			m_coordinateSystemInvalidated = false;
		}

		UpdatePointSpotShadowMaps();

		for (const Ndk::EntityHandle& camera : m_cameras)
		{
			CameraComponent& camComponent = camera->GetComponent<CameraComponent>();

			//UpdateDirectionalShadowMaps(camComponent);

			Nz::AbstractRenderQueue* renderQueue = m_renderTechnique->GetRenderQueue();

			// To make sure the bounding volume used by the culling list is updated
			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				graphicsComponent.EnsureBoundingVolumeUpdate();
			}

			bool forceInvalidation = false;

			std::size_t visibilityHash = m_drawableCulling.Cull(camComponent.GetFrustum(), &forceInvalidation);

			// Always regenerate renderqueue if particle groups are present for now (FIXME)
			if (!m_particleGroups.empty())
				forceInvalidation = true;

			if (camComponent.UpdateVisibility(visibilityHash) || m_forceRenderQueueInvalidation || forceInvalidation)
			{
				renderQueue->Clear();
				for (const GraphicsComponent* gfxComponent : m_drawableCulling)
					gfxComponent->AddToRenderQueue(renderQueue);

				for (const Ndk::EntityHandle& light : m_lights)
				{
					LightComponent& lightComponent = light->GetComponent<LightComponent>();
					NodeComponent& lightNode = light->GetComponent<NodeComponent>();

					///TODO: Cache somehow?
					lightComponent.AddToRenderQueue(renderQueue, Nz::Matrix4f::ConcatenateAffine(m_coordinateSystemMatrix, lightNode.GetTransformMatrix()));
				}

				for (const Ndk::EntityHandle& particleGroup : m_particleGroups)
				{
					ParticleGroupComponent& groupComponent = particleGroup->GetComponent<ParticleGroupComponent>();

					groupComponent.AddToRenderQueue(renderQueue, Nz::Matrix4f::Identity()); //< ParticleGroup doesn't use any transform matrix (yet)
				}

				m_forceRenderQueueInvalidation = false;
			}

			camComponent.ApplyView();

			Nz::SceneData sceneData;
			sceneData.ambientColor = Nz::Color(25, 25, 25);
			sceneData.background = m_background;
			sceneData.globalReflectionTexture = nullptr;
			sceneData.viewer = &camComponent;

			if (m_background && m_background->GetBackgroundType() == Nz::BackgroundType_Skybox)
				sceneData.globalReflectionTexture = static_cast<Nz::SkyboxBackground*>(m_background.Get())->GetTexture();

			m_renderTechnique->Clear(sceneData);
			m_renderTechnique->Draw(sceneData);
		}
	}

	/*!
	* \brief Updates the directional shadow maps according to the position of the viewer
	*
	* \param viewer Viewer of the scene
	*/

	void RenderSystem::UpdateDirectionalShadowMaps(const Nz::AbstractViewer& /*viewer*/)
	{
		if (!m_shadowRT.IsValid())
			m_shadowRT.Create();

		Nz::SceneData dummySceneData;
		dummySceneData.ambientColor = Nz::Color(0, 0, 0);
		dummySceneData.background = nullptr;
		dummySceneData.viewer = nullptr; //< Depth technique doesn't require any viewer

		for (const Ndk::EntityHandle& light : m_directionalLights)
		{
			LightComponent& lightComponent = light->GetComponent<LightComponent>();
			NodeComponent& lightNode = light->GetComponent<NodeComponent>();

			if (!lightComponent.IsShadowCastingEnabled())
				continue;

			Nz::Vector2ui shadowMapSize(lightComponent.GetShadowMap()->GetSize());

			m_shadowRT.AttachTexture(Nz::AttachmentPoint_Depth, 0, lightComponent.GetShadowMap());
			Nz::Renderer::SetTarget(&m_shadowRT);
			Nz::Renderer::SetViewport(Nz::Recti(0, 0, shadowMapSize.x, shadowMapSize.y));

			Nz::AbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
			renderQueue->Clear();

			///TODO: Culling
			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();

				graphicsComponent.AddToRenderQueue(renderQueue);
			}

			///TODO: Cache the matrices in the light?
			Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, Nz::Matrix4f::Ortho(0.f, 100.f, 100.f, 0.f, 1.f, 100.f));
			Nz::Renderer::SetMatrix(Nz::MatrixType_View, Nz::Matrix4f::ViewMatrix(lightNode.GetRotation() * Nz::Vector3f::Forward() * 100.f, lightNode.GetRotation()));

			m_shadowTechnique.Clear(dummySceneData);
			m_shadowTechnique.Draw(dummySceneData);
		}
	}

	/*!
	* \brief Updates the point spot shadow maps
	*/

	void RenderSystem::UpdatePointSpotShadowMaps()
	{
		if (!m_shadowRT.IsValid())
			m_shadowRT.Create();

		Nz::SceneData dummySceneData;
		dummySceneData.ambientColor = Nz::Color(0, 0, 0);
		dummySceneData.background = nullptr;
		dummySceneData.viewer = nullptr; //< Depth technique doesn't require any viewer

		for (const Ndk::EntityHandle& light : m_pointSpotLights)
		{
			LightComponent& lightComponent = light->GetComponent<LightComponent>();
			NodeComponent& lightNode = light->GetComponent<NodeComponent>();

			if (!lightComponent.IsShadowCastingEnabled())
				continue;

			Nz::Vector2ui shadowMapSize(lightComponent.GetShadowMap()->GetSize());

			switch (lightComponent.GetLightType())
			{
				case Nz::LightType_Directional:
					NazaraInternalError("Directional lights included in point/spot light list");
					break;

				case Nz::LightType_Point:
				{
					static Nz::Quaternionf rotations[6] =
					{
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(),  Nz::Vector3f::UnitX()), // nzCubemapFace_PositiveX
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -Nz::Vector3f::UnitX()), // nzCubemapFace_NegativeX
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -Nz::Vector3f::UnitY()), // nzCubemapFace_PositiveY
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(),  Nz::Vector3f::UnitY()), // nzCubemapFace_NegativeY
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -Nz::Vector3f::UnitZ()), // nzCubemapFace_PositiveZ
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(),  Nz::Vector3f::UnitZ())  // nzCubemapFace_NegativeZ
					};

					for (unsigned int face = 0; face < 6; ++face)
					{
						m_shadowRT.AttachTexture(Nz::AttachmentPoint_Depth, 0, lightComponent.GetShadowMap(), face);
						Nz::Renderer::SetTarget(&m_shadowRT);
						Nz::Renderer::SetViewport(Nz::Recti(0, 0, shadowMapSize.x, shadowMapSize.y));

						///TODO: Cache the matrices in the light?
						Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, Nz::Matrix4f::Perspective(Nz::FromDegrees(90.f), 1.f, 0.1f, lightComponent.GetRadius()));
						Nz::Renderer::SetMatrix(Nz::MatrixType_View, Nz::Matrix4f::ViewMatrix(lightNode.GetPosition(), rotations[face]));

						Nz::AbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
						renderQueue->Clear();

						///TODO: Culling
						for (const Ndk::EntityHandle& drawable : m_drawables)
						{
							GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();

							graphicsComponent.AddToRenderQueue(renderQueue);
						}

						m_shadowTechnique.Clear(dummySceneData);
						m_shadowTechnique.Draw(dummySceneData);
					}
					break;
				}

				case Nz::LightType_Spot:
				{
					m_shadowRT.AttachTexture(Nz::AttachmentPoint_Depth, 0, lightComponent.GetShadowMap());
					Nz::Renderer::SetTarget(&m_shadowRT);
					Nz::Renderer::SetViewport(Nz::Recti(0, 0, shadowMapSize.x, shadowMapSize.y));

					///TODO: Cache the matrices in the light?
					Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, Nz::Matrix4f::Perspective(lightComponent.GetOuterAngle()*2.f, 1.f, 0.1f, lightComponent.GetRadius()));
					Nz::Renderer::SetMatrix(Nz::MatrixType_View, Nz::Matrix4f::ViewMatrix(lightNode.GetPosition(), lightNode.GetRotation()));

					Nz::AbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
					renderQueue->Clear();

					///TODO: Culling
					for (const Ndk::EntityHandle& drawable : m_drawables)
					{
						GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();

						graphicsComponent.AddToRenderQueue(renderQueue);
					}

					m_shadowTechnique.Clear(dummySceneData);
					m_shadowTechnique.Draw(dummySceneData);
					break;
				}
			}
		}
	}

	SystemIndex RenderSystem::systemIndex;
}
