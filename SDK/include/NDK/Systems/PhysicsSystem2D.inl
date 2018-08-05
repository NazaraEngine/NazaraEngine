// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

namespace Ndk
{
	inline float PhysicsSystem2D::GetDamping() const
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		return m_physWorld->GetDamping();
	}

	inline Nz::Vector2f PhysicsSystem2D::GetGravity() const
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		return m_physWorld->GetGravity();
	}

	inline std::size_t PhysicsSystem2D::GetIterationCount() const
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		return m_physWorld->GetIterationCount();
	}

	inline std::size_t PhysicsSystem2D::GetMaxStepCount() const
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		return m_physWorld->GetMaxStepCount();
	}

	inline float PhysicsSystem2D::GetStepSize() const
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		return m_physWorld->GetStepSize();
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

	inline void PhysicsSystem2D::SetDamping(float dampingValue)
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		m_physWorld->SetDamping(dampingValue);
	}

	inline void PhysicsSystem2D::SetGravity(const Nz::Vector2f& gravity)
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		m_physWorld->SetGravity(gravity);
	}

	inline void PhysicsSystem2D::SetIterationCount(std::size_t iterationCount)
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		m_physWorld->SetIterationCount(iterationCount);
	}

	inline void PhysicsSystem2D::SetMaxStepCount(std::size_t maxStepCount)
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		m_physWorld->SetMaxStepCount(maxStepCount);
	}

	inline void PhysicsSystem2D::SetStepSize(float stepSize)
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		m_physWorld->SetStepSize(stepSize);
	}

	inline void PhysicsSystem2D::UseSpatialHash(float cellSize, std::size_t entityCount)
	{
		NazaraAssert(m_physWorld, "Invalid physics world");

		m_physWorld->UseSpatialHash(cellSize, entityCount);
	}
}
