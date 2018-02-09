// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysWorld3D::PhysWorld3D() :
	m_gravity(Vector3f::Zero()),
	m_maxStepCount(50),
	m_stepSize(0.005f),
	m_timestepAccumulator(0.f)
	{
		m_world = NewtonCreate();
		NewtonWorldSetUserData(m_world, this);
	}

	PhysWorld3D::~PhysWorld3D()
	{
		NewtonDestroy(m_world);
	}

	Vector3f PhysWorld3D::GetGravity() const
	{
		return m_gravity;
	}

	NewtonWorld* PhysWorld3D::GetHandle() const
	{
		return m_world;
	}

	std::size_t PhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	float PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	void PhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_gravity = gravity;
	}

	void PhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void PhysWorld3D::SetSolverModel(unsigned int model)
	{
		NewtonSetSolverModel(m_world, model);
	}

	void PhysWorld3D::SetStepSize(float stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld3D::Step(float timestep)
	{
		m_timestepAccumulator += timestep;

		std::size_t stepCount = 0;
		while (m_timestepAccumulator >= m_stepSize && stepCount < m_maxStepCount)
		{
			NewtonUpdate(m_world, m_stepSize);
			m_timestepAccumulator -= m_stepSize;
			stepCount++;
		}
	}
}
