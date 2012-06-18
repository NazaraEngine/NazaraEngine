// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PHYSICSENTITY_HPP
#define PHYSICSENTITY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Newton/Newton.h>
#include <Nazara/Math/Matrix4.hpp>
//#include <Nazara/Physics/CollisionShape.hpp>
#include "CollisionShape.hpp"

class NzPhysicsWorld;

class NzPhysicsEntity
{
    public:
        NzPhysicsEntity(NzPhysicsWorld* world, const NzCollisionShape& shape, const NzVector3f& position, float mass);
        virtual void Init();
        virtual ~NzPhysicsEntity();
    protected:
        NzPhysicsWorld* m_world;
        NewtonBody* m_body;
        float m_mass;
        NzMatrix4f m_entityMatrix;
    private:

};

#endif // PHYSICSENTITY_HPP
