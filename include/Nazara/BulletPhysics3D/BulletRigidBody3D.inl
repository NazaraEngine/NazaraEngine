// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	inline void BulletRigidBody3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline const std::shared_ptr<BulletCollider3D>& BulletRigidBody3D::GetGeom() const
	{
		return m_geom;
	}

	inline btRigidBody* BulletRigidBody3D::GetRigidBody() const
	{
		return m_body;
	}
	inline std::size_t BulletRigidBody3D::GetUniqueIndex() const
	{
		return m_bodyPoolIndex;
	}

	inline BulletPhysWorld3D* BulletRigidBody3D::GetWorld() const
	{
		return m_world;
	}
}

#include <Nazara/BulletPhysics3D/DebugOff.hpp>
