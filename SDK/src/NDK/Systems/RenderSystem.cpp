// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	RenderSystem::RenderSystem()
	{
	}

	void RenderSystem::Update(float elapsedTime)
	{
		for (const Ndk::EntityHandle& camera : m_cameras)
		{
			CameraComponent& camComponent = camera->GetComponent<CameraComponent>();
			NodeComponent& cameraNode = camera->GetComponent<NodeComponent>();

			camComponent.ApplyView();
		}
	}

	void RenderSystem::OnEntityRemoved(Entity* entity)
	{
		m_cameras.Remove(entity);
		m_drawables.Remove(entity);
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

	}

	SystemIndex RenderSystem::systemIndex;
}
