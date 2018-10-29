// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

namespace Ndk
{
	inline float PhysicsSystem2D::GetDamping() const
	{
		return GetPhysWorld().GetDamping();
	}

	inline Nz::Vector2f PhysicsSystem2D::GetGravity() const
	{
		return GetPhysWorld().GetGravity();
	}

	inline std::size_t PhysicsSystem2D::GetIterationCount() const
	{
		return GetPhysWorld().GetIterationCount();
	}

	inline std::size_t PhysicsSystem2D::GetMaxStepCount() const
	{
		return GetPhysWorld().GetMaxStepCount();
	}

	inline float PhysicsSystem2D::GetStepSize() const
	{
		return GetPhysWorld().GetStepSize();
	}

	inline void PhysicsSystem2D::SetDamping(float dampingValue)
	{
		GetPhysWorld().SetDamping(dampingValue);
	}

	inline void PhysicsSystem2D::SetGravity(const Nz::Vector2f& gravity)
	{
		GetPhysWorld().SetGravity(gravity);
	}

	inline void PhysicsSystem2D::SetIterationCount(std::size_t iterationCount)
	{
		GetPhysWorld().SetIterationCount(iterationCount);
	}

	inline void PhysicsSystem2D::SetMaxStepCount(std::size_t maxStepCount)
	{
		GetPhysWorld().SetMaxStepCount(maxStepCount);
	}

	inline void PhysicsSystem2D::SetStepSize(float stepSize)
	{
		GetPhysWorld().SetStepSize(stepSize);
	}

	inline void PhysicsSystem2D::UseSpatialHash(float cellSize, std::size_t entityCount)
	{
		GetPhysWorld().UseSpatialHash(cellSize, entityCount);
	}

	/*!
	* \brief Gets the physical world
	* \return A reference to the physical world
	*/

	inline Nz::PhysWorld2D& PhysicsSystem2D::GetPhysWorld()
	{
		if (!m_physWorld)
			CreatePhysWorld();

		return *m_physWorld;
	}

	/*!
	* \brief Gets the physical world
	* \return A constant reference to the physical world
	*/

	inline const Nz::PhysWorld2D& PhysicsSystem2D::GetPhysWorld() const
	{
		if (!m_physWorld)
			CreatePhysWorld();

		return *m_physWorld;
	}
}
