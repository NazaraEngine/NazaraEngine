// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void PhysCharacter3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline const std::shared_ptr<Collider3D>& PhysCharacter3D::GetCollider() const
	{
		return m_collider;
	}

	inline PhysWorld3D& PhysCharacter3D::GetPhysWorld()
	{
		return *m_world;
	}

	inline const PhysWorld3D& PhysCharacter3D::GetPhysWorld() const
	{
		return *m_world;
	}

	inline void PhysCharacter3D::SetImpl(std::shared_ptr<PhysCharacter3DImpl> characterImpl)
	{
		m_impl = std::move(characterImpl);
	}
}

