// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysObject.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <algorithm>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysObject::PhysObject(PhysWorld* world, const Matrix4f& mat) :
	PhysObject(world, NullGeom::New(), mat)
	{
	}

	PhysObject::PhysObject(PhysWorld* world, PhysGeomRef geom, const Matrix4f& mat) :
	m_matrix(mat),
	m_geom(std::move(geom)),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(world),
	m_gravityFactor(1.f),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");

		if (!m_geom)
			m_geom = NullGeom::New();

		m_body = NewtonCreateDynamicBody(m_world->GetHandle(), m_geom->GetHandle(m_world), m_matrix);
		NewtonBodySetUserData(m_body, this);
	}

	PhysObject::PhysObject(const PhysObject& object) :
	m_matrix(object.m_matrix),
	m_geom(object.m_geom),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");
		NazaraAssert(m_geom, "Invalid geometry");

		m_body = NewtonCreateDynamicBody(m_world->GetHandle(), m_geom->GetHandle(m_world), m_matrix);
		NewtonBodySetUserData(m_body, this);
		SetMass(object.m_mass);
	}

	PhysObject::PhysObject(PhysObject&& object) :
	m_matrix(std::move(object.m_matrix)),
	m_geom(std::move(object.m_geom)),
	m_forceAccumulator(std::move(object.m_forceAccumulator)),
	m_torqueAccumulator(std::move(object.m_torqueAccumulator)),
	m_body(object.m_body),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.m_mass)
	{
		object.m_body = nullptr;
	}

	PhysObject::~PhysObject()
	{
		if (m_body)
			NewtonDestroyBody(m_body);
	}

	void PhysObject::AddForce(const Vector3f& force, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys_Global:
				m_forceAccumulator += force;
				break;

			case CoordSys_Local:
				m_forceAccumulator += GetRotation() * force;
				break;
		}

		// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
		NewtonBodySetSleepState(m_body, 0);
	}

	void PhysObject::AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys_Global:
				m_forceAccumulator += force;
				m_torqueAccumulator += Vector3f::CrossProduct(point - GetMassCenter(CoordSys_Global), force);
				break;

			case CoordSys_Local:
				return AddForce(m_matrix.Transform(force, 0.f), m_matrix.Transform(point), CoordSys_Global);
		}

		// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
		NewtonBodySetSleepState(m_body, 0);
	}

	void PhysObject::AddTorque(const Vector3f& torque, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys_Global:
				m_torqueAccumulator += torque;
				break;

			case CoordSys_Local:
				m_torqueAccumulator += m_matrix.Transform(torque, 0.f);
				break;
		}

		// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
		NewtonBodySetSleepState(m_body, 0);
	}

	void PhysObject::EnableAutoSleep(bool autoSleep)
	{
		NewtonBodySetAutoSleep(m_body, autoSleep);
	}

	Boxf PhysObject::GetAABB() const
	{
		Vector3f min, max;
		NewtonBodyGetAABB(m_body, min, max);

		return Boxf(min, max);
	}

	Vector3f PhysObject::GetAngularVelocity() const
	{
		Vector3f angularVelocity;
		NewtonBodyGetOmega(m_body, angularVelocity);

		return angularVelocity;
	}

	const PhysGeomRef& PhysObject::GetGeom() const
	{
		return m_geom;
	}

	float PhysObject::GetGravityFactor() const
	{
		return m_gravityFactor;
	}

	NewtonBody* PhysObject::GetHandle() const
	{
		return m_body;
	}

	float PhysObject::GetMass() const
	{
		return m_mass;
	}

	Vector3f PhysObject::GetMassCenter(CoordSys coordSys) const
	{
		Vector3f center;
		NewtonBodyGetCentreOfMass(m_body, center);

		switch (coordSys)
		{
			case CoordSys_Global:
				center = m_matrix.Transform(center);
				break;

			case CoordSys_Local:
				break; // Aucune opération à effectuer sur le centre de rotation
		}

		return center;
	}

	const Matrix4f& PhysObject::GetMatrix() const
	{
		return m_matrix;
	}

	Vector3f PhysObject::GetPosition() const
	{
		return m_matrix.GetTranslation();
	}

	Quaternionf PhysObject::GetRotation() const
	{
		return m_matrix.GetRotation();
	}

	Vector3f PhysObject::GetVelocity() const
	{
		Vector3f velocity;
		NewtonBodyGetVelocity(m_body, velocity);

		return velocity;
	}

	bool PhysObject::IsAutoSleepEnabled() const
	{
		return NewtonBodyGetAutoSleep(m_body) != 0;
	}

	bool PhysObject::IsMoveable() const
	{
		return m_mass > 0.f;
	}

	bool PhysObject::IsSleeping() const
	{
		return NewtonBodyGetSleepState(m_body) != 0;
	}

	void PhysObject::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		NewtonBodySetOmega(m_body, angularVelocity);
	}

	void PhysObject::SetGeom(PhysGeomRef geom)
	{
		if (m_geom.Get() != geom)
		{
			if (geom)
				m_geom = geom;
			else
				m_geom = NullGeom::New();

			NewtonBodySetCollision(m_body, m_geom->GetHandle(m_world));
		}
	}

	void PhysObject::SetGravityFactor(float gravityFactor)
	{
		m_gravityFactor = gravityFactor;
	}

	void PhysObject::SetMass(float mass)
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
			Vector3f inertia, origin;
			m_geom->ComputeInertialMatrix(&inertia, &origin);

			NewtonBodySetCentreOfMass(m_body, &origin.x);
			NewtonBodySetMassMatrix(m_body, mass, inertia.x*mass, inertia.y*mass, inertia.z*mass);
			NewtonBodySetForceAndTorqueCallback(m_body, &ForceAndTorqueCallback);
			NewtonBodySetTransformCallback(m_body, &TransformCallback);
		}

		m_mass = mass;
	}

	void PhysObject::SetMassCenter(const Vector3f& center)
	{
		if (m_mass > 0.f)
			NewtonBodySetCentreOfMass(m_body, center);
	}

	void PhysObject::SetPosition(const Vector3f& position)
	{
		m_matrix.SetTranslation(position);
		UpdateBody();
	}

	void PhysObject::SetRotation(const Quaternionf& rotation)
	{
		m_matrix.SetRotation(rotation);
		UpdateBody();
	}

	void PhysObject::SetVelocity(const Vector3f& velocity)
	{
		NewtonBodySetVelocity(m_body, velocity);
	}

	PhysObject& PhysObject::operator=(const PhysObject& object)
	{
		PhysObject physObj(object);
		return operator=(std::move(physObj));
	}

	void PhysObject::UpdateBody()
	{
		NewtonBodySetMatrix(m_body, m_matrix);

		if (NumberEquals(m_mass, 0.f))
		{
			// http://newtondynamics.com/wiki/index.php5?title=Can_i_dynamicly_move_a_TriMesh%3F
			Vector3f min, max;
			NewtonBodyGetAABB(m_body, min, max);

			NewtonWorldForEachBodyInAABBDo(m_world->GetHandle(), min, max, [](const NewtonBody* const body, void* const userData) -> int
			{
				NazaraUnused(userData);
				NewtonBodySetSleepState(body, 0);
				return 1;
			}, 
			nullptr);
		}
		/*for (std::set<PhysObjectListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->PhysObjectOnUpdate(this);*/
	}

	PhysObject& PhysObject::operator=(PhysObject&& object)
	{
		if (m_body)
			NewtonDestroyBody(m_body);

		m_body               = object.m_body;
		m_forceAccumulator   = std::move(object.m_forceAccumulator);
		m_geom               = std::move(object.m_geom);
		m_gravityFactor      = object.m_gravityFactor;
		m_mass               = object.m_mass;
		m_matrix             = std::move(object.m_matrix);
		m_torqueAccumulator  = std::move(object.m_torqueAccumulator);
		m_world              = object.m_world;

		object.m_body = nullptr;

		return *this;
	}

	void PhysObject::ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex)
	{
		NazaraUnused(timeStep);
		NazaraUnused(threadIndex);

		PhysObject* me = static_cast<PhysObject*>(NewtonBodyGetUserData(body));

		if (!NumberEquals(me->m_gravityFactor, 0.f))
			me->m_forceAccumulator += me->m_world->GetGravity() * me->m_gravityFactor * me->m_mass;

		/*for (std::set<PhysObjectListener*>::iterator it = me->m_listeners.begin(); it != me->m_listeners.end(); ++it)
			(*it)->PhysObjectApplyForce(me);*/

		NewtonBodySetForce(body, me->m_forceAccumulator);
		NewtonBodySetTorque(body, me->m_torqueAccumulator);

		me->m_torqueAccumulator.Set(0.f);
		me->m_forceAccumulator.Set(0.f);

		///TODO: Implanter la force gyroscopique?
	}

	void PhysObject::TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex)
	{
		NazaraUnused(threadIndex);

		PhysObject* me = static_cast<PhysObject*>(NewtonBodyGetUserData(body));
		me->m_matrix.Set(matrix);

		/*for (std::set<PhysObjectListener*>::iterator it = me->m_listeners.begin(); it != me->m_listeners.end(); ++it)
			(*it)->PhysObjectOnUpdate(me);*/
		}
}
