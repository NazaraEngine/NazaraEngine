// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PHYSICSSOLVER_HPP
#define PHYSICSSOLVER_HPP

#include <Nazara/Prerequesites.hpp>

enum nzSolverMode
{
    nzExact,
    nzAdaptative,
    nzLinear
};

class NzPhysicsWorld;

class NzPhysicsSolver
{
    public:
        NzPhysicsSolver(nzSolverMode mode = nzLinear, unsigned int numberOfPassesLinearMode = 10);
        void Configure(nzSolverMode mode, unsigned int numberOfPassesLinearMode);
        void Set(NzPhysicsWorld* world);
        ~NzPhysicsSolver();
    protected:
    private:
        nzSolverMode m_mode;
        unsigned int m_numberOfPassesLinearMode;
};

#endif // PHYSICSSOLVER_HPP
