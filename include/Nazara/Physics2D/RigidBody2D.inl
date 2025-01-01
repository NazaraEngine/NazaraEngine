// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline RigidBody2D::RigidBody2D(PhysWorld2D& world, const DynamicSettings& settings)
	{
		Create(world, settings);
	}

	inline RigidBody2D::RigidBody2D(PhysWorld2D& world, const StaticSettings& settings)
	{
		Create(world, settings);
	}

	inline RigidBody2D::~RigidBody2D()
	{
		Destroy();
	}

	inline void RigidBody2D::AddForce(const Vector2f& force, CoordSys coordSys)
	{
		return AddForce(force, GetMassCenter(coordSys), coordSys);
	}

	inline void RigidBody2D::AddImpulse(const Vector2f& impulse, CoordSys coordSys)
	{
		return AddImpulse(impulse, GetMassCenter(coordSys), coordSys);
	}

	inline UInt32 RigidBody2D::GetBodyIndex() const
	{
		return m_bodyIndex;
	}

	inline const std::shared_ptr<Collider2D>& RigidBody2D::GetCollider() const
	{
		return m_collider;
	}

	inline cpBody* RigidBody2D::GetHandle() const
	{
		return m_handle;
	}

	inline float RigidBody2D::GetMass() const
	{
		return m_mass;
	}

	inline const Vector2f& RigidBody2D::GetPositionOffset() const
	{
		return m_positionOffset;
	}

	inline std::size_t RigidBody2D::GetShapeIndex(cpShape* shape) const
	{
		auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
		if (it == m_shapes.end())
			return InvalidShapeIndex;

		return std::distance(m_shapes.begin(), it);
	}

	inline std::size_t RigidBody2D::GetShapeCount() const
	{
		return m_shapes.size();
	}

	inline const RigidBody2D::VelocityFunc& RigidBody2D::GetVelocityFunction() const
	{
		return m_velocityFunc;
	}

	inline PhysWorld2D* RigidBody2D::GetWorld() const
	{
		return m_world;
	}

	inline bool RigidBody2D::IsKinematic() const
	{
		return m_mass <= 0.f;
	}

	inline bool RigidBody2D::IsSimulationEnabled() const
	{
		return m_isSimulationEnabled;
	}

	inline bool RigidBody2D::IsStatic() const
	{
		return m_isStatic;
	}
}
