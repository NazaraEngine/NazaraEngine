// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline RigidBody3DComponent::RigidBody3DComponent(const RigidBody3D::DynamicSettings& settings, PhysicsReplication3D replication) :
	m_replicationMode(replication)
	{
		m_settings = std::make_unique<Settings>(settings);
	}

	inline RigidBody3DComponent::RigidBody3DComponent(const RigidBody3D::StaticSettings& settings, PhysicsReplication3D replication) :
	m_replicationMode(replication)
	{
		m_settings = std::make_unique<Settings>(settings);
	}

	inline RigidBody3DComponent::RigidBody3DComponent(const Settings& settings, PhysicsReplication3D replication) :
	m_replicationMode(replication)
	{
		m_settings = std::make_unique<Settings>(settings);
	}

	inline auto RigidBody3DComponent::GetReplicationCallback() const -> const CustomReplicationCallback&
	{
		return m_replicationCustomCallback;
	}

	inline PhysicsReplication3D RigidBody3DComponent::GetReplicationMode() const
	{
		return m_replicationMode;
	}

	inline void RigidBody3DComponent::SetReplicationCallback(CustomReplicationCallback customCallback)
	{
		m_replicationCustomCallback = std::move(customCallback);
	}

	inline void RigidBody3DComponent::SetReplicationMode(PhysicsReplication3D replicationMode)
	{
		m_replicationMode = replicationMode;
	}

	inline void RigidBody3DComponent::Construct(PhysWorld3D& world)
	{
		assert(m_settings);
		std::visit([&](auto&& arg)
		{
			Create(world, arg);
		}, *m_settings);
		m_settings.reset();
	}
}
