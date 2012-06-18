// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "PhysicsWorld.hpp"
//#include <Nazara/Physics/PhysicsWorld.hpp>

#include <Nazara/Core/Debug.hpp>

NzPhysicsWorld::NzPhysicsWorld()
{
    newtonWorld = NewtonCreate();
    m_solver.Configure(nzLinear,10);
    m_solver.Set(this);
    SetFrictionModel(nzAdaptative);
}

NzPhysicsWorld::~NzPhysicsWorld()
{
    NewtonDestroy(newtonWorld);
}

void NzPhysicsWorld::SetSize(const NzCubef& size)
{
    m_size = size;
    float bottom[3];
    bottom[0] = m_size.x;
    bottom[1] = m_size.y;
    bottom[2] = m_size.z;
    float top[3];
    top[0] = m_size.x + m_size.width;
    top[1] = m_size.y + m_size.height;
    top[2] = m_size.z + m_size.depth;

    NewtonSetWorldSize(newtonWorld, static_cast<dFloat*>(bottom),
                                   static_cast<dFloat*>(top));
}

const NzCubef& NzPhysicsWorld::GetSize() const
{
    return m_size;
}

void NzPhysicsWorld::SetPhysicsSolver(const NzPhysicsSolver& solver)
{
    m_solver = solver;
    m_solver.Set(this);
}

const NzPhysicsSolver& NzPhysicsWorld::GetPhysicsSolver() const
{
    return m_solver;
}

void NzPhysicsWorld::SetFrictionModel(nzFrictionModel model)
{
    switch(model)
    {
        case nzExact:
            NewtonSetFrictionModel(newtonWorld,0);
        break;

        case nzAdaptative:
            NewtonSetFrictionModel(newtonWorld,1);
        break;
    }

    m_frictionModel = model;
}

const NzPhysicsWorld::nzFrictionModel& NzPhysicsWorld::GetFrictionModel() const
{
    return m_frictionModel;
}

void NzPhysicsWorld::UpdatePhysics(float timestep)
{
    NewtonUpdate(newtonWorld,timestep);//FLOAT WTF ?
}
