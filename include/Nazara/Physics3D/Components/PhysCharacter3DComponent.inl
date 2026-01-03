// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline PhysCharacter3DComponent::PhysCharacter3DComponent(const Settings& settings, PhysicsReplication3D replication) :
	m_replicationMode(replication)
	{
		m_settings = std::make_unique<PhysCharacter3D::Settings>(settings);
	}

	inline auto PhysCharacter3DComponent::GetReplicationCallback() const -> const CustomReplicationCallback&
	{
		return m_replicationCustomCallback;
	}

	inline PhysicsReplication3D PhysCharacter3DComponent::GetReplicationMode() const
	{
		return m_replicationMode;
	}

	inline void PhysCharacter3DComponent::SetReplicationCallback(CustomReplicationCallback customCallback)
	{
		m_replicationCustomCallback = std::move(customCallback);
	}

	inline void PhysCharacter3DComponent::SetReplicationMode(PhysicsReplication3D replicationMode)
	{
		m_replicationMode = replicationMode;
	}

	inline void PhysCharacter3DComponent::Construct(PhysWorld3D& world)
	{
		assert(m_settings);
		Create(world, *m_settings);
		m_settings.reset();
	}
}
