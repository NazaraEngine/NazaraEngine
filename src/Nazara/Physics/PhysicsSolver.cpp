// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "PhysicsWorld.hpp"
//#include <Nazara/Physics/PhysicsWorld.hpp>
#include "PhysicsSolver.hpp"
//#include <Nazara/Physics/PhysicsSolver.hpp>
#include <Newton/Newton.h>

#include <Nazara/Core/Debug.hpp>

NzPhysicsSolver::NzPhysicsSolver(nzSolverMode mode, unsigned int numberOfPassesLinearMode)
{
    m_mode = mode;
    m_numberOfPassesLinearMode = numberOfPassesLinearMode;
}

void NzPhysicsSolver::Configure(nzSolverMode mode, unsigned int numberOfPassesLinearMode)
{
    m_mode = mode;
    m_numberOfPassesLinearMode = numberOfPassesLinearMode;
}

void NzPhysicsSolver::Set(NzPhysicsWorld* world)
{
    switch(m_mode)
    {
        case nzExact:
            NewtonSetSolverModel(world->newtonWorld, 0);
        break;

        case nzAdaptative:
            NewtonSetSolverModel(world->newtonWorld, 1);
        break;

        case nzLinear:
            NewtonSetSolverModel(world->newtonWorld, m_numberOfPassesLinearMode);

    }
}

NzPhysicsSolver::~NzPhysicsSolver()
{
    //dtor
}
