// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Newton/Newton.h>

enum nzFrictionModel
{
  nzExact,
  nzAdaptative
};

class NzPhysicsSolver;

class NzPhysicsWorld
{
    public:
        NzPhysicsWorld();
        ~NzPhysicsWorld();

        void SetPhysicsSolver(const NzPhysicsSolver& solver);
        const NzPhysicsSolver& GetPhysicsSolver() const;

        void SetFrictionModel(nzFrictionModel model);
        const nzFrictionModel& GetFrictionModel();

        void UpdatePhysics(nzUint64 timestep);

    protected:
    private:
        NewtonWorld* m_world;
        NzPhysicsSolver m_solver;
        nzFrictionModel m_frictionModel;
};

#endif // PHYSICSWORLD_HPP
