// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "CollisionShape.hpp"
//#include <Nazara/Physics/CollisionShape.hpp>
#include <Nazara/Core/Debug.hpp>

NzCollisionShape::NzCollisionShape(NzPhysicsWorld* world)
{
    m_world = world;
    m_isCreated = false;
    newtonCollisionShape = nullptr;
}

NzCollisionShape::~NzCollisionShape()
{
    if(m_isCreated)
        NewtonReleaseCollision(m_world->newtonWorld, newtonCollisionShape);
}

void NzCollisionShape::Create(const NzVector3f& cubeSize)
{
    newtonCollisionShape = NewtonCreateBox(m_world->newtonWorld, static_cast<dFloat>(cubeSize.x),
                                                                 static_cast<dFloat>(cubeSize.y),
                                                                 static_cast<dFloat>(cubeSize.z), 0, NULL);
}

void NzCollisionShape::Release()
{
    if(m_isCreated)
        NewtonReleaseCollision(m_world->newtonWorld, newtonCollisionShape);
}
