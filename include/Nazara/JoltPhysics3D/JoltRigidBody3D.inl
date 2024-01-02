// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline JoltRigidBody3D::JoltRigidBody3D(JoltPhysWorld3D& world, const DynamicSettings& settings)
	{
		Create(world, settings);
	}

	inline JoltRigidBody3D::JoltRigidBody3D(JoltPhysWorld3D& world, const StaticSettings& settings)
	{
		Create(world, settings);
	}

	inline void JoltRigidBody3D::DisableSimulation()
	{
		return EnableSimulation(false);
	}

	inline void JoltRigidBody3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline JPH::Body* JoltRigidBody3D::GetBody()
	{
		return m_body;
	}

	inline const JPH::Body* JoltRigidBody3D::GetBody() const
	{
		return m_body;
	}

	inline const std::shared_ptr<JoltCollider3D>& JoltRigidBody3D::GetGeom() const
	{
		return m_geom;
	}

	inline JoltPhysWorld3D& JoltRigidBody3D::GetWorld() const
	{
		return *m_world;
	}

	inline bool JoltRigidBody3D::IsSimulationEnabled() const
	{
		return m_isSimulationEnabled;
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
