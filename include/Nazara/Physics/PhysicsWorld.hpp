// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Newton/Newton.h>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Cube.hpp>
#include "PhysicsSolver.hpp"
//#include <Nazara/Physics/PhysicsSolver.hpp>

//TODO : ajouter Axis Aligned Bounding Box

class NzPhysicsWorld
{
    enum nzFrictionModel
    {
      nzExact,
      nzAdaptative
    };

    public:
        NzPhysicsWorld();
        ~NzPhysicsWorld();

        void SetSize(const NzCubef& size);
        const NzCubef& GetSize() const;

        void SetPhysicsSolver(const NzPhysicsSolver& solver);
        const NzPhysicsSolver& GetPhysicsSolver() const;

        void SetFrictionModel(nzFrictionModel model);
        const nzFrictionModel& GetFrictionModel() const;

        void UpdatePhysics(float timestep);

        NewtonWorld* newtonWorld;

    protected:
    private:
        NzPhysicsSolver m_solver;
        nzFrictionModel m_frictionModel;
        NzCubef m_size;
};

#endif // PHYSICSWORLD_HPP
