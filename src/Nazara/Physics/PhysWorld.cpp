// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics/Debug.hpp>

NzPhysWorld::NzPhysWorld() :
m_gravity(NzVector3f::Zero()),
m_stepSize(0.005f),
m_timestepAccumulator(0.f)
{
	m_world = NewtonCreate();
	NewtonWorldSetUserData(m_world, this);
}

NzPhysWorld::~NzPhysWorld()
{
	NewtonDestroy(m_world);
}

NzVector3f NzPhysWorld::GetGravity() const
{
	return m_gravity;
}

NewtonWorld* NzPhysWorld::GetHandle() const
{
	return m_world;
}

float NzPhysWorld::GetStepSize() const
{
	return m_stepSize;
}

void NzPhysWorld::SetGravity(const NzVector3f& gravity)
{
	m_gravity = gravity;
}

void NzPhysWorld::SetSolverModel(unsigned int model)
{
	NewtonSetSolverModel(m_world, model);
}

void NzPhysWorld::SetStepSize(float stepSize)
{
	m_stepSize = stepSize;
}

void NzPhysWorld::Step(float timestep)
{
	m_timestepAccumulator += timestep;

	while (m_timestepAccumulator >= m_stepSize)
	{
		NewtonUpdate(m_world, m_stepSize);
		m_timestepAccumulator -= m_stepSize;
	}
}
