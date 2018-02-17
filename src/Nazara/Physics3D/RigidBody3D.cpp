// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Newton/Newton.h>
#include <algorithm>
#include <cmath>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	RigidBody3D::RigidBody3D(PhysWorld3D* world, const Matrix4f& mat) :
	RigidBody3D(world, NullCollider3D::New(), mat)
	{
	}

	RigidBody3D::RigidBody3D(PhysWorld3D* world, Collider3DRef geom, const Matrix4f& mat) :
	m_geom(std::move(geom)),
	m_matrix(mat),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(world),
	m_gravityFactor(1.f),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");

		if (!m_geom)
			m_geom = NullCollider3D::New();

		m_body = NewtonCreateDynamicBody(m_world->GetHandle(), m_geom->GetHandle(m_world), m_matrix);
		NewtonBodySetUserData(m_body, this);
		NewtonBodySetTransformCallback(m_body, &TransformCallback);
	}

	RigidBody3D::RigidBody3D(const RigidBody3D& object) :
	m_geom(object.m_geom),
	m_matrix(object.m_matrix),
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
		NewtonBodySetTransformCallback(m_body, &TransformCallback);

		SetMass(object.m_mass);
		SetAngularDamping(object.GetAngularDamping());
		SetAngularVelocity(object.GetAngularVelocity());
		SetLinearDamping(object.GetLinearDamping());
		SetLinearVelocity(object.GetLinearVelocity());
		SetMassCenter(object.GetMassCenter());
		SetPosition(object.GetPosition());
		SetRotation(object.GetRotation());
	}

	RigidBody3D::RigidBody3D(RigidBody3D&& object) :
	m_geom(std::move(object.m_geom)),
	m_matrix(std::move(object.m_matrix)),
	m_forceAccumulator(std::move(object.m_forceAccumulator)),
	m_torqueAccumulator(std::move(object.m_torqueAccumulator)),
	m_body(object.m_body),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.m_mass)
	{
		object.m_body = nullptr;
	}

	RigidBody3D::~RigidBody3D()
	{
		if (m_body)
			NewtonDestroyBody(m_body);
	}

	void RigidBody3D::AddForce(const Vector3f& force, CoordSys coordSys)
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

	void RigidBody3D::AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys)
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

	void RigidBody3D::AddTorque(const Vector3f& torque, CoordSys coordSys)
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

	void RigidBody3D::EnableAutoSleep(bool autoSleep)
	{
		NewtonBodySetAutoSleep(m_body, autoSleep);
	}

	void RigidBody3D::EnableSimulation(bool simulation)
	{
		NewtonBodySetSimulationState(m_body, simulation);
	}

	Boxf RigidBody3D::GetAABB() const
	{
		Vector3f min, max;
		NewtonBodyGetAABB(m_body, min, max);

		return Boxf(min, max);
	}

	Vector3f RigidBody3D::GetAngularDamping() const
	{
		Vector3f angularDamping;
		NewtonBodyGetAngularDamping(m_body, angularDamping);

		return angularDamping;
	}

	Vector3f RigidBody3D::GetAngularVelocity() const
	{
		Vector3f angularVelocity;
		NewtonBodyGetOmega(m_body, angularVelocity);

		return angularVelocity;
	}

	const Collider3DRef& RigidBody3D::GetGeom() const
	{
		return m_geom;
	}

	float RigidBody3D::GetGravityFactor() const
	{
		return m_gravityFactor;
	}

	NewtonBody* RigidBody3D::GetHandle() const
	{
		return m_body;
	}

	float RigidBody3D::GetLinearDamping() const
	{
		return NewtonBodyGetLinearDamping(m_body);
	}

	Vector3f RigidBody3D::GetLinearVelocity() const
	{
		Vector3f velocity;
		NewtonBodyGetVelocity(m_body, velocity);

		return velocity;
	}

	float RigidBody3D::GetMass() const
	{
		return m_mass;
	}

	Vector3f RigidBody3D::GetMassCenter(CoordSys coordSys) const
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

	int RigidBody3D::GetMaterial() const
	{
		return NewtonBodyGetMaterialGroupID(m_body);
	}

	const Matrix4f& RigidBody3D::GetMatrix() const
	{
		return m_matrix;
	}

	Vector3f RigidBody3D::GetPosition() const
	{
		return m_matrix.GetTranslation();
	}

	Quaternionf RigidBody3D::GetRotation() const
	{
		return m_matrix.GetRotation();
	}

	void* RigidBody3D::GetUserdata() const
	{
		return m_userdata;
	}

	PhysWorld3D* RigidBody3D::GetWorld() const
	{
		return m_world;
	}

	bool RigidBody3D::IsAutoSleepEnabled() const
	{
		return NewtonBodyGetAutoSleep(m_body) != 0;
	}

	bool RigidBody3D::IsMoveable() const
	{
		return m_mass > 0.f;
	}

	bool RigidBody3D::IsSimulationEnabled() const
	{
		return NewtonBodyGetSimulationState(m_body) != 0;
	}

	bool RigidBody3D::IsSleeping() const
	{
		return NewtonBodyGetSleepState(m_body) != 0;
	}

	void RigidBody3D::SetAngularDamping(const Vector3f& angularDamping)
	{
		NewtonBodySetAngularDamping(m_body, angularDamping);
	}

	void RigidBody3D::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		NewtonBodySetOmega(m_body, angularVelocity);
	}

	void RigidBody3D::SetGeom(Collider3DRef geom)
	{
		if (m_geom.Get() != geom)
		{
			if (geom)
				m_geom = geom;
			else
				m_geom = NullCollider3D::New();

			NewtonBodySetCollision(m_body, m_geom->GetHandle(m_world));
		}
	}

	void RigidBody3D::SetGravityFactor(float gravityFactor)
	{
		m_gravityFactor = gravityFactor;
	}

	void RigidBody3D::SetLinearDamping(float damping)
	{
		NewtonBodySetLinearDamping(m_body, damping);
	}

	void RigidBody3D::SetLinearVelocity(const Vector3f& velocity)
	{
		NewtonBodySetVelocity(m_body, velocity);
	}

	void RigidBody3D::SetMass(float mass)
	{
		NazaraAssert(mass >= 0.f, "Mass must be positive and finite");
		NazaraAssert(std::isfinite(mass), "Mass must be positive and finite");

		if (m_mass > 0.f)
		{
			if (mass > 0.f)
			{
				// If we already have a mass, we already have an inertial matrix as well, just rescale it
				float Ix, Iy, Iz;
				NewtonBodyGetMassMatrix(m_body, &m_mass, &Ix, &Iy, &Iz);

				float scale = mass / m_mass;
				NewtonBodySetMassMatrix(m_body, mass, Ix*scale, Iy*scale, Iz*scale);
			}
			else
			{
				NewtonBodySetMassMatrix(m_body, 0.f, 0.f, 0.f, 0.f);
				NewtonBodySetForceAndTorqueCallback(m_body, nullptr);
			}
		}
		else
		{
			Vector3f inertia, origin;
			m_geom->ComputeInertialMatrix(&inertia, &origin);

			NewtonBodySetCentreOfMass(m_body, &origin.x);
			NewtonBodySetMassMatrix(m_body, mass, inertia.x*mass, inertia.y*mass, inertia.z*mass);
			NewtonBodySetForceAndTorqueCallback(m_body, &ForceAndTorqueCallback);
		}

		m_mass = mass;
	}

	void RigidBody3D::SetMassCenter(const Vector3f& center)
	{
		if (m_mass > 0.f)
			NewtonBodySetCentreOfMass(m_body, center);
	}

	void RigidBody3D::SetMaterial(const String& materialName)
	{
		SetMaterial(m_world->GetMaterial(materialName));
	}

	void RigidBody3D::SetMaterial(int materialIndex)
	{
		NewtonBodySetMaterialGroupID(m_body, materialIndex);
	}

	void RigidBody3D::SetPosition(const Vector3f& position)
	{
		m_matrix.SetTranslation(position);

		UpdateBody();
	}

	void RigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		m_matrix.SetRotation(rotation);

		UpdateBody();
	}

	void RigidBody3D::SetUserdata(void* ud)
	{
		m_userdata = ud;
	}

	RigidBody3D& RigidBody3D::operator=(const RigidBody3D& object)
	{
		RigidBody3D physObj(object);
		return operator=(std::move(physObj));
	}

	void RigidBody3D::UpdateBody()
	{
		NewtonBodySetMatrix(m_body, m_matrix);

		if (NumberEquals(m_mass, 0.f))
		{
			// Moving a static body in Newton does not update bodies at the target location
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
	}

	RigidBody3D& RigidBody3D::operator=(RigidBody3D&& object)
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

	void RigidBody3D::ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex)
	{
		NazaraUnused(timeStep);
		NazaraUnused(threadIndex);

		RigidBody3D* me = static_cast<RigidBody3D*>(NewtonBodyGetUserData(body));

		if (!NumberEquals(me->m_gravityFactor, 0.f))
			me->m_forceAccumulator += me->m_world->GetGravity() * me->m_gravityFactor * me->m_mass;

		NewtonBodySetForce(body, me->m_forceAccumulator);
		NewtonBodySetTorque(body, me->m_torqueAccumulator);

		me->m_torqueAccumulator.Set(0.f);
		me->m_forceAccumulator.Set(0.f);

		///TODO: Implement gyroscopic force?
	}

	void RigidBody3D::TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex)
	{
		NazaraUnused(threadIndex);

		RigidBody3D* me = static_cast<RigidBody3D*>(NewtonBodyGetUserData(body));
		me->m_matrix.Set(matrix);
	}
}
