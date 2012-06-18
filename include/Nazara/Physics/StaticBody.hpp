// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef STATICBODY_HPP
#define STATICBODY_HPP

#include <Nazara/Prerequesites.hpp>
//#include <Nazara/Physics/PhysicsEntity.hpp>
#include "PhysicsEntity.hpp"

class NzStaticBody : public NzPhysicsEntity
{
    public:
        NzStaticBody(NzPhysicsWorld* world, const NzCollisionShape& shape, const NzVector3f& position, float mass);
        ~NzStaticBody();
    protected:
    private:
};

#endif // STATICBODY_HPP
