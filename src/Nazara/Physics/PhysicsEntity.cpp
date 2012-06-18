// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "PhysicsWorld.hpp"
//#include <Nazara/Physics/PhysicsWorld.hpp>
#include "PhysicsEntity.hpp"
//#include <Nazara/Physics/PhysicsEntity.hpp>
#include <Nazara/Core/Debug.hpp>

NzPhysicsEntity::NzPhysicsEntity(NzPhysicsWorld* world, const NzCollisionShape& shape, const NzVector3f& position, float mass) : m_world(world), m_mass(mass)
{
    m_entityMatrix.SetIdentity();
    m_entityMatrix.SetTranslation(position);

    m_body = NewtonCreateBody(world->newtonWorld, shape.newtonCollisionShape,NULL);

    //NewtonBodySetMatrix(m_pBody, /*&m_entityMatrix.matrice [0][0]*/);//Passage dgMatrix a NzMatrix4 ??

//Pour rigid bodies uniquement
 /*
      // On calcul l'inertie du corps, en passant par une petite formule
      CVector inertie;

      inertie.x = 0.7f * m_masse * (m_taille.y * m_taille.y + m_taille.z * m_taille.z) / 12;
      inertie.y = 0.7f * m_masse * (m_taille.x * m_taille.x + m_taille.z * m_taille.z) / 12;
      inertie.z = 0.7f * m_masse * (m_taille.x * m_taille.x + m_taille.y * m_taille.y) / 12;

      // On définit ensuite la masse et l'inertie pour ce corps
      NewtonBodySetMassMatrix (m_pBody, m_masse, inertie.x, inertie.y, inertie.z);

      // On règle enfin le Callback, qui sera nécessaire pour que le corps bouge
      NewtonBodySetForceAndTorqueCallback (m_pBody, ApplyForceAndTorqueCallback);*/
}

void NzPhysicsEntity::Init()
{

}

NzPhysicsEntity::~NzPhysicsEntity()
{
    NewtonDestroyBody(m_world->newtonWorld,m_body);
}
