// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline void JoltRigidBody3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline UInt32 JoltRigidBody3D::GetBodyIndex() const
	{
		return m_bodyIndex;
	}

	inline const std::shared_ptr<JoltCollider3D>& JoltRigidBody3D::GetGeom() const
	{
		return m_geom;
	}

	inline JoltPhysWorld3D* JoltRigidBody3D::GetWorld() const
	{
		return m_world;
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
