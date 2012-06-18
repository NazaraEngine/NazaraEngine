// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef COLLISIONSHAPE_HPP
#define COLLISIONSHAPE_HPP

#include <Newton/Newton.h>
#include "PhysicsWorld.hpp"
//#include <Nazara/Physics/PhysicsWorld.hpp>

class NzCollisionShape
{
    public:
        NzCollisionShape(NzPhysicsWorld* world);
        ~NzCollisionShape();

        void Create(const NzVector3f& cubeSize);
        //void Create(const NzSpheref& sphere);
        //void Create(const NzConef& cone);
        //void Create(const NzMesh& customMesh);
        void Release();

        bool IsValid();

        NewtonCollision* newtonCollisionShape;

    protected:
    private:
        NzPhysicsWorld* m_world;
        bool m_isCreated;
};

#endif // COLLISIONSHAPE_HPP
