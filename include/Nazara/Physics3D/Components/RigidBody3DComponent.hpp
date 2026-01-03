// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_COMPONENTS_RIGIDBODY3DCOMPONENT_HPP
#define NAZARA_PHYSICS3D_COMPONENTS_RIGIDBODY3DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/Enums.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <entt/fwd.hpp>
#include <functional>
#include <variant>

namespace Nz
{
	class NAZARA_PHYSICS3D_API RigidBody3DComponent : public RigidBody3D
	{
		friend class Physics3DSystem;

		public:
			using CustomReplicationCallback = std::function<void(entt::handle entity, RigidBody3DComponent& rigidBodyComponent)>;

			inline RigidBody3DComponent(const RigidBody3D::DynamicSettings& settings, PhysicsReplication3D replication = PhysicsReplication3D::Local);
			inline RigidBody3DComponent(const RigidBody3D::StaticSettings& settings, PhysicsReplication3D replication = PhysicsReplication3D::None);
			inline RigidBody3DComponent(const Settings& settings, PhysicsReplication3D replication);
			RigidBody3DComponent(const RigidBody3DComponent&) = delete;
			RigidBody3DComponent(RigidBody3DComponent&&) noexcept = default;
			~RigidBody3DComponent() = default;

			inline const CustomReplicationCallback& GetReplicationCallback() const;
			inline PhysicsReplication3D GetReplicationMode() const;

			inline void SetReplicationCallback(CustomReplicationCallback customCallback);
			inline void SetReplicationMode(PhysicsReplication3D replicationMode);

			RigidBody3DComponent& operator=(const RigidBody3DComponent&) = delete;
			RigidBody3DComponent& operator=(RigidBody3DComponent&&) noexcept = default;

		private:
			inline void Construct(PhysWorld3D& world);

			std::unique_ptr<Settings> m_settings;
			CustomReplicationCallback m_replicationCustomCallback;
			PhysicsReplication3D m_replicationMode;
	};
}

#include <Nazara/Physics3D/Components/RigidBody3DComponent.inl>

#endif // NAZARA_PHYSICS3D_COMPONENTS_RIGIDBODY3DCOMPONENT_HPP
