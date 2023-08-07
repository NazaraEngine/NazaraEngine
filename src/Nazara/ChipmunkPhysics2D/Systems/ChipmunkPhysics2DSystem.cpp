// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/Systems/ChipmunkPhysics2DSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	namespace
	{
		inline RadianAnglef AngleFromQuaternion(const Quaternionf& quat)
		{
			float siny_cosp = 2.f * (quat.w * quat.z + quat.x * quat.y);
			float cosy_cosp = 1.f - 2.f * (quat.y * quat.y + quat.z * quat.z);

			return std::atan2(siny_cosp, cosy_cosp); //<FIXME: not very efficient
		}
	}

	ChipmunkPhysics2DSystem::ChipmunkPhysics2DSystem(entt::registry& registry) :
	m_registry(registry),
	m_physicsConstructObserver(m_registry, entt::collector.group<ChipmunkRigidBody2DComponent, NodeComponent>())
	{
		m_bodyConstructConnection = registry.on_construct<ChipmunkRigidBody2DComponent>().connect<&ChipmunkPhysics2DSystem::OnBodyConstruct>(this);
	}

	ChipmunkPhysics2DSystem::~ChipmunkPhysics2DSystem()
	{
		m_physicsConstructObserver.disconnect();

		// Ensure every body is destroyed before world is
		auto rigidBodyView = m_registry.view<ChipmunkRigidBody2DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy();
	}

	void ChipmunkPhysics2DSystem::Update(Time elapsedTime)
	{
		// Move newly-created physics entities to their node position/rotation
		m_physicsConstructObserver.each([&](entt::entity entity)
		{
			ChipmunkRigidBody2DComponent& entityPhysics = m_registry.get<ChipmunkRigidBody2DComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityPhysics.TeleportTo(Vector2f(entityNode.GetPosition()), AngleFromQuaternion(entityNode.GetRotation()));
		});

		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		auto view = m_registry.view<NodeComponent, const ChipmunkRigidBody2DComponent>(entt::exclude<DisabledComponent>);
		for (auto [entity, nodeComponent, rigidBodyComponent] : view.each())
		{
			if (rigidBodyComponent.IsSleeping())
				continue;

			nodeComponent.SetTransform(rigidBodyComponent.GetPosition(), rigidBodyComponent.GetRotation());
		}
	}

	void ChipmunkPhysics2DSystem::OnBodyConstruct(entt::registry& registry, entt::entity entity)
	{
		ChipmunkRigidBody2DComponent& rigidBody = registry.get<ChipmunkRigidBody2DComponent>(entity);
		rigidBody.Construct(m_physWorld);
	}
}
