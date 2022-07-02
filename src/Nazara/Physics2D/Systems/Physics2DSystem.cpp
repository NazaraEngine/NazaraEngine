// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Systems/Physics2DSystem.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	namespace
	{
		inline Nz::RadianAnglef AngleFromQuaternion(const Nz::Quaternionf& quat)
		{
			float siny_cosp = 2.f * (quat.w * quat.z + quat.x * quat.y);
			float cosy_cosp = 1.f - 2.f * (quat.y * quat.y + quat.z * quat.z);

			return std::atan2(siny_cosp, cosy_cosp); //<FIXME: not very efficient
		}
	}

	Physics2DSystem::Physics2DSystem(entt::registry& registry) :
	m_registry(registry)
	{
		m_constructConnection = registry.on_construct<RigidBody2DComponent>().connect<OnConstruct>();
	}

	Physics2DSystem::~Physics2DSystem()
	{
		// Ensure every NewtonBody is destroyed before world is
		auto rigidBodyView = m_registry.view<RigidBody2DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy();

		m_constructConnection.release();
	}

	void Physics2DSystem::Update(float elapsedTime)
	{
		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		auto view = m_registry.view<NodeComponent, const RigidBody2DComponent>();
		for (auto [entity, nodeComponent, rigidBodyComponent] : view.each())
		{
			if (rigidBodyComponent.IsSleeping())
				continue;

			nodeComponent.SetPosition(rigidBodyComponent.GetPosition(), CoordSys::Global);
			nodeComponent.SetRotation(rigidBodyComponent.GetRotation(), CoordSys::Global);
		}
	}

	void Physics2DSystem::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		// If our entity already has a node component when adding a rigid body, initialize it with its position/rotation
		NodeComponent* node = registry.try_get<NodeComponent>(entity);
		if (node)
		{
			RigidBody2DComponent& rigidBody = registry.get<RigidBody2DComponent>(entity);
			rigidBody.SetPosition(Vector2f(node->GetPosition()));
			rigidBody.SetRotation(AngleFromQuaternion(node->GetRotation()));
		}
	}
}
