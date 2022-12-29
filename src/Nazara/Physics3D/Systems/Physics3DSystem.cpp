// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Systems/Physics3DSystem.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Physics3DSystem::Physics3DSystem(entt::registry& registry) :
	m_registry(registry)
	{
		m_constructConnection = registry.on_construct<RigidBody3DComponent>().connect<OnConstruct>();
	}

	Physics3DSystem::~Physics3DSystem()
	{
		// Ensure every NewtonBody is destroyed before world is
		auto rigidBodyView = m_registry.view<RigidBody3DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy();
	}

	void Physics3DSystem::Update(Time elapsedTime)
	{
		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		auto view = m_registry.view<NodeComponent, const RigidBody3DComponent>();
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
		// If our entity already has a node component when adding a rigid body, initialize it with its position/rotation
		NodeComponent* node = registry.try_get<NodeComponent>(entity);
		if (node)
		{
			RigidBody3DComponent& rigidBody = registry.get<RigidBody3DComponent>(entity);
			rigidBody.SetPosition(node->GetPosition());
			rigidBody.SetRotation(node->GetRotation());
		}
	}
}
