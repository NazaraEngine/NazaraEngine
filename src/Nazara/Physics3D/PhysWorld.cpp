// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysWorld::PhysWorld() :
	m_gravity(Vector3f::Zero()),
	m_stepSize(0.005f),
	m_timestepAccumulator(0.f)
	{
		m_world = NewtonCreate();
		NewtonWorldSetUserData(m_world, this);
	}

	PhysWorld::~PhysWorld()
	{
		NewtonDestroy(m_world);
	}

	Vector3f PhysWorld::GetGravity() const
	{
		return m_gravity;
	}

	NewtonWorld* PhysWorld::GetHandle() const
	{
		return m_world;
	}

	float PhysWorld::GetStepSize() const
	{
		return m_stepSize;
	}

	void PhysWorld::SetGravity(const Vector3f& gravity)
	{
		m_gravity = gravity;
	}

	void PhysWorld::SetSolverModel(unsigned int model)
	{
		NewtonSetSolverModel(m_world, model);
	}

	void PhysWorld::SetStepSize(float stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld::Step(float timestep)
	{
		m_timestepAccumulator += timestep;

		while (m_timestepAccumulator >= m_stepSize)
		{
			NewtonUpdate(m_world, m_stepSize);
			m_timestepAccumulator -= m_stepSize;
		}
	}
}
