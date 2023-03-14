// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	inline void RigidBody3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline const std::shared_ptr<Collider3D>& RigidBody3D::GetGeom() const
	{
		return m_geom;
	}

	inline btRigidBody* RigidBody3D::GetRigidBody() const
	{
		return m_body;
	}
	inline std::size_t RigidBody3D::GetUniqueIndex() const
	{
		return m_bodyPoolIndex;
	}

	inline PhysWorld3D* RigidBody3D::GetWorld() const
	{
		return m_world;
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
