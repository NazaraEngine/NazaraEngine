// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "PhysicsWorld.hpp"
//#include <Nazara/Physics/PhysicsWorld.hpp>
#include "PhysicsSolver.hpp"
//#include <Nazara/Physics/PhysicsSolver.hpp>

#include <Nazara/Core/Debug.hpp>

NzPhysicsWorld::NzPhysicsWorld()
{
    m_world = NewtonCreate();
    m_solver.Configure(nzLinear,10);
    m_solver.Set(this);
    SetFrictionModel(nzAdaptative);
}

NzPhysicsWorld::~NzPhysicsWorld()
{
    NewtonDestroy(m_world);
}

void NzPhysicsWorld::SetPhysicsSolverSolver(const NzPhysicsSolver& solver)
{
    m_solver = solver;
    m_solver.Set(this);
}

const nzSolverMode& NzPhysicsWorld::GetPhysicsSolver()
{
    return m_solver;
}

void NzPhysicsWorld::SetFrictionModel(nzFrictionModel model)
{
    switch(model)
    {
        case nzExact:
            NewtonSetFrictionModel(this,0);
        break;

        case nzAdaptative:
            NewtonSetFrictionModel(this,1);
        break;
    }

    m_frictionModel = model;
}

const nzFrictionModel& NzPhysicsWorld::GetFrictionModel()
{
    return m_frictionModel;
}

void NzPhysicsWorld::UpdatePhysics(nzUint64 timestep)
{
    NewtonUpdate(m_world,static_cast<float>(timestep));//FLOAT WTF ?
}
