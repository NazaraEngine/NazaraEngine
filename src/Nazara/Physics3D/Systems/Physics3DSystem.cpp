// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Physics3D/Systems/Physics3DSystem.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Physics3DSystem::Physics3DSystem(entt::registry& registry)
	{
		m_constructConnection = registry.on_construct<RigidBody3DComponent>().connect<OnConstruct>();
	}

	Physics3DSystem::~Physics3DSystem()
	{
		m_constructConnection.release();
	}

	void Physics3DSystem::Update(entt::registry& registry, float elapsedTime)
	{
		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		auto view = registry.view<Nz::NodeComponent, const RigidBody3DComponent>();
		for (auto [entity, nodeComponent, rigidBodyComponent] : view.each())
		{
			if (rigidBodyComponent.IsSleeping())
				continue;

			nodeComponent.SetPosition(rigidBodyComponent.GetPosition(), CoordSys::Global);
			nodeComponent.SetRotation(rigidBodyComponent.GetRotation(), CoordSys::Global);
		}
	}

	void Physics3DSystem::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		// If our entity already has a node component when adding a rigid body, initialize it with
		Nz::NodeComponent* node = registry.try_get<NodeComponent>(entity);
		if (node)
		{
			RigidBody3DComponent& rigidBody = registry.get<RigidBody3DComponent>(entity);
			rigidBody.SetPosition(node->GetPosition());
			rigidBody.SetRotation(node->GetRotation());
		}
	}
}
