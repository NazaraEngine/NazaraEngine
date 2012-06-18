// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "StaticBody.hpp"
//#include <Nazara/Physics/StaticBody.hpp>

#include <Nazara/Core/Debug.hpp>

NzStaticBody::NzStaticBody(NzPhysicsWorld* world, const NzCollisionShape& shape, const NzVector3f& position, float mass) : NzPhysicsEntity(world,shape,position,mass)
{
    //ctor
}

NzStaticBody::~NzStaticBody()
{
    //dtor
}
