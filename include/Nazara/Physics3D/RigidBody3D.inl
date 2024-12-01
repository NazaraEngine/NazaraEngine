// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline RigidBody3D::RigidBody3D(PhysWorld3D& world, const DynamicSettings& settings)
	{
		Create(world, settings);
	}

	inline RigidBody3D::RigidBody3D(PhysWorld3D& world, const StaticSettings& settings)
	{
		Create(world, settings);
	}

	inline RigidBody3D::RigidBody3D(PhysWorld3D& world, const Settings& settings)
	{
		Create(world, settings);
	}

	inline void RigidBody3D::DisableSimulation()
	{
		return EnableSimulation(false);
	}

	inline void RigidBody3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline JPH::Body* RigidBody3D::GetBody()
	{
		return m_body;
	}

	inline const JPH::Body* RigidBody3D::GetBody() const
	{
		return m_body;
	}

	inline const std::shared_ptr<Collider3D>& RigidBody3D::GetCollider() const
	{
		return m_collider;
	}

	inline PhysWorld3D& RigidBody3D::GetWorld() const
	{
		return *m_world;
	}

	inline bool RigidBody3D::IsSimulationEnabled() const
	{
		return m_isSimulationEnabled;
	}
}

