// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/PhysObject.hpp>
#include <Nazara/Physics/Config.hpp>
#include <Nazara/Physics/Geom.hpp>
#include <Nazara/Physics/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics/Debug.hpp>

NzPhysObject::NzPhysObject(NzPhysWorld* world, const NzMatrix4f& mat) :
m_matrix(mat),
m_forceAccumulator(NzVector3f::Zero()),
m_torqueAccumulator(NzVector3f::Zero()),
m_world(world),
m_ownsGeom(true),
m_gravityFactor(1.f),
m_mass(0.f)
{
	#if NAZARA_PHYSICS_SAFE
	if (!world)
		NazaraError("Invalid physics world"); ///TODO: Unexcepted
	#endif

	m_geom = new NzNullGeom(world);
	m_body = NewtonCreateDynamicBody(world->GetHandle(), m_geom->GetHandle(), mat);
	NewtonBodySetUserData(m_body, this);
}

NzPhysObject::NzPhysObject(NzPhysWorld* world, const NzBaseGeom* geom, const NzMatrix4f& mat) :
m_matrix(mat),
m_forceAccumulator(NzVector3f::Zero()),
m_torqueAccumulator(NzVector3f::Zero()),
m_geom(geom),
m_world(world),
m_ownsGeom(false),
m_gravityFactor(1.f),
m_mass(0.f)
{
	#if NAZARA_PHYSICS_SAFE
	if (!world)
		NazaraError("Invalid physics world"); ///TODO: Unexcepted
	#endif

	m_body = NewtonCreateDynamicBody(world->GetHandle(), geom->GetHandle(), mat);
	NewtonBodySetUserData(m_body, this);
}

NzPhysObject::~NzPhysObject()
{
	NewtonDestroyBody(m_world->GetHandle(), m_body);

	if (m_ownsGeom)
		delete m_geom;
}

void NzPhysObject::AddForce(const NzVector3f& force, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			m_forceAccumulator += force;
			break;

		case nzCoordSys_Local:
			m_forceAccumulator += GetRotation()*force;
			break;
	}

	// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
	NewtonBodySetSleepState(m_body, 0);
}

void NzPhysObject::AddForce(const NzVector3f& force, const NzVector3f& point, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			m_forceAccumulator += force;
			m_torqueAccumulator += NzVector3f::CrossProduct(point - GetMassCenter(nzCoordSys_Global), force);
			break;

		case nzCoordSys_Local:
			AddForce(m_matrix.Transform(force, 0.f), m_matrix.Transform(point));
			return;
	}

	// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
	NewtonBodySetSleepState(m_body, 0);
}

void NzPhysObject::AddTorque(const NzVector3f& torque, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			m_torqueAccumulator += torque;
			break;

		case nzCoordSys_Local:
			m_torqueAccumulator += m_matrix.Transform(torque, 0.f);
			break;
	}

	// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
	NewtonBodySetSleepState(m_body, 0);
}

void NzPhysObject::EnableAutoSleep(bool autoSleep)
{
	NewtonBodySetAutoSleep(m_body, autoSleep);
}

NzVector3f NzPhysObject::GetAngularVelocity() const
{
	NzVector3f angularVelocity;
	NewtonBodyGetOmega(m_body, angularVelocity);

	return angularVelocity;
}

float NzPhysObject::GetGravityFactor() const
{
	return m_gravityFactor;
}

NewtonBody* NzPhysObject::GetHandle() const
{
	return m_body;
}

float NzPhysObject::GetMass() const
{
	return m_mass;
}

NzVector3f NzPhysObject::GetMassCenter(nzCoordSys coordSys) const
{
	NzVector3f center;
	NewtonBodyGetCentreOfMass(m_body, center);

	switch (coordSys)
	{
		case nzCoordSys_Global:
			center = m_matrix.Transform(center);
			break;

		case nzCoordSys_Local:
			break; // Aucune opération à effectuer sur le centre de rotation
	}

	return center;
}

const NzMatrix4f& NzPhysObject::GetMatrix() const
{
	return m_matrix;
}

NzVector3f NzPhysObject::GetPosition() const
{
	return m_matrix.GetTranslation();
}

NzQuaternionf NzPhysObject::GetRotation() const
{
	return m_matrix.GetRotation();
}

NzVector3f NzPhysObject::GetVelocity() const
{
	NzVector3f velocity;
	NewtonBodyGetVelocity(m_body, velocity);

	return velocity;
}

bool NzPhysObject::IsAutoSleepEnabled() const
{
	return NewtonBodyGetAutoSleep(m_body) != 0;
}

bool NzPhysObject::IsMoveable() const
{
	return m_mass > 0.f;
}

bool NzPhysObject::IsSleeping() const
{
	return NewtonBodyGetSleepState(m_body) != 0;
}

void NzPhysObject::SetGravityFactor(float gravityFactor)
{
	m_gravityFactor = gravityFactor;
}

void NzPhysObject::SetMass(float mass)
{
	if (m_mass > 0.f)
	{
		float Ix, Iy, Iz;
		NewtonBodyGetMassMatrix(m_body, &m_mass, &Ix, &Iy, &Iz);
		float scale = mass/m_mass;
		NewtonBodySetMassMatrix(m_body, mass, Ix*scale, Iy*scale, Iz*scale);
	}
	else if (mass > 0.f)
	{
		NzVector3f inertia, origin;
		m_geom->ComputeInertialMatrix(&inertia, &origin);

		NewtonBodySetCentreOfMass(m_body, &origin.x);
		NewtonBodySetMassMatrix(m_body, mass, inertia.x*mass, inertia.y*mass, inertia.z*mass);
		NewtonBodySetForceAndTorqueCallback(m_body, &ForceAndTorqueCallback);
		NewtonBodySetTransformCallback(m_body, &TransformCallback);
	}

	m_mass = mass;
}

void NzPhysObject::SetMassCenter(const NzVector3f& center)
{
	if (m_mass > 0.f)
		NewtonBodySetCentreOfMass(m_body, center);
}

void NzPhysObject::SetPosition(const NzVector3f& position)
{
	m_matrix.SetTranslation(position);
	UpdateBody();
}

void NzPhysObject::SetRotation(const NzQuaternionf& rotation)
{
	m_matrix.SetRotation(rotation);
	UpdateBody();
}

void NzPhysObject::UpdateBody()
{
	NewtonBodySetMatrix(m_body, m_matrix);
	/*for (std::set<PhysObjectListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
		(*it)->PhysObjectOnUpdate(this);*/
}

void NzPhysObject::ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex)
{
	NazaraUnused(timeStep);
	NazaraUnused(threadIndex);

	NzPhysObject* me = static_cast<NzPhysObject*>(NewtonBodyGetUserData(body));

	if (!NzNumberEquals(me->m_gravityFactor, 0.f))
		me->m_forceAccumulator += me->m_world->GetGravity() * me->m_gravityFactor * me->m_mass;

	/*for (std::set<PhysObjectListener*>::iterator it = me->m_listeners.begin(); it != me->m_listeners.end(); ++it)
		(*it)->PhysObjectApplyForce(me);*/

	NewtonBodySetForce(body, me->m_forceAccumulator);
	NewtonBodySetTorque(body, me->m_torqueAccumulator);

	me->m_torqueAccumulator.Set(0.f);
	me->m_forceAccumulator.Set(0.f);

	///TODO: Implanter la force gyroscopique?
}

void NzPhysObject::TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex)
{
	NazaraUnused(threadIndex);

	NzPhysObject* me = static_cast<NzPhysObject*>(NewtonBodyGetUserData(body));
	me->m_matrix.Set(matrix);

	/*for (std::set<PhysObjectListener*>::iterator it = me->m_listeners.begin(); it != me->m_listeners.end(); ++it)
		(*it)->PhysObjectOnUpdate(me);*/
}
