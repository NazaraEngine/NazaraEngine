// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline ChipmunkRigidBody2D::ChipmunkRigidBody2D(ChipmunkPhysWorld2D& world, const DynamicSettings& settings)
	{
		Create(world, settings);
	}

	inline ChipmunkRigidBody2D::ChipmunkRigidBody2D(ChipmunkPhysWorld2D& world, const StaticSettings& settings)
	{
		Create(world, settings);
	}

	inline ChipmunkRigidBody2D::~ChipmunkRigidBody2D()
	{
		Destroy();
	}

	inline void ChipmunkRigidBody2D::AddForce(const Vector2f& force, CoordSys coordSys)
	{
		return AddForce(force, GetMassCenter(coordSys), coordSys);
	}

	inline void ChipmunkRigidBody2D::AddImpulse(const Vector2f& impulse, CoordSys coordSys)
	{
		return AddImpulse(impulse, GetMassCenter(coordSys), coordSys);
	}

	inline UInt32 ChipmunkRigidBody2D::GetBodyIndex() const
	{
		return m_bodyIndex;
	}

	inline const std::shared_ptr<ChipmunkCollider2D>& ChipmunkRigidBody2D::GetGeom() const
	{
		return m_geom;
	}

	inline cpBody* ChipmunkRigidBody2D::GetHandle() const
	{
		return m_handle;
	}

	inline float ChipmunkRigidBody2D::GetMass() const
	{
		return m_mass;
	}

	inline const Vector2f& ChipmunkRigidBody2D::GetPositionOffset() const
	{
		return m_positionOffset;
	}

	inline std::size_t ChipmunkRigidBody2D::GetShapeIndex(cpShape* shape) const
	{
		auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
		if (it == m_shapes.end())
			return InvalidShapeIndex;

		return std::distance(m_shapes.begin(), it);
	}

	inline std::size_t ChipmunkRigidBody2D::GetShapeCount() const
	{
		return m_shapes.size();
	}

	inline const ChipmunkRigidBody2D::VelocityFunc& ChipmunkRigidBody2D::GetVelocityFunction() const
	{
		return m_velocityFunc;
	}

	inline ChipmunkPhysWorld2D* ChipmunkRigidBody2D::GetWorld() const
	{
		return m_world;
	}

	inline bool ChipmunkRigidBody2D::IsKinematic() const
	{
		return m_mass <= 0.f;
	}

	inline bool ChipmunkRigidBody2D::IsSimulationEnabled() const
	{
		return m_isSimulationEnabled;
	}

	inline bool ChipmunkRigidBody2D::IsStatic() const
	{
		return m_isStatic;
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
