// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics/Debug.hpp>

NzPhysWorld::NzPhysWorld()
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

void NzPhysWorld::SetGravity(const NzVector3f& gravity)
{
	m_gravity = gravity;
}

void NzPhysWorld::SetSize(const NzBoxf& box)
{
	NewtonSetWorldSize(m_world, box.GetPosition(), box.GetPosition()+box.GetSize());
}

void NzPhysWorld::SetSize(const NzVector3f& min, const NzVector3f& max)
{
	NewtonSetWorldSize(m_world, min, max);
}

void NzPhysWorld::SetSolverModel(unsigned int model)
{
	NewtonSetSolverModel(m_world, model);
}

void NzPhysWorld::Update(float timestep)
{
	NewtonUpdate(m_world, timestep);
}
