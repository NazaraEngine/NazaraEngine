// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <newton/Newton.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	RigidBody3D::RigidBody3D(PhysWorld3D* world, const Matrix4f& mat) :
	RigidBody3D(world, std::make_shared<NullCollider3D>(), mat)
	{
	}

	RigidBody3D::RigidBody3D(PhysWorld3D* world, std::shared_ptr<Collider3D> geom, const Matrix4f& mat) :
	m_geom(std::move(geom)),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(world),
	m_gravityFactor(1.f),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");

		if (!m_geom)
			m_geom = std::make_shared<NullCollider3D>();

		m_body = NewtonCreateDynamicBody(m_world->GetHandle(), m_geom->GetHandle(m_world), &mat.m11);
		NewtonBodySetUserData(m_body, this);
	}

	RigidBody3D::RigidBody3D(const RigidBody3D& object) :
	m_geom(object.m_geom),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");
		NazaraAssert(m_geom, "Invalid geometry");

		std::array<float, 16> transformMatrix;
		NewtonBodyGetMatrix(object.GetHandle(), transformMatrix.data());

		m_body = NewtonCreateDynamicBody(m_world->GetHandle(), m_geom->GetHandle(m_world), transformMatrix.data());
		NewtonBodySetUserData(m_body, this);

		SetMass(object.m_mass);
		SetAngularDamping(object.GetAngularDamping());
		SetAngularVelocity(object.GetAngularVelocity());
		SetLinearDamping(object.GetLinearDamping());
		SetLinearVelocity(object.GetLinearVelocity());
		SetMassCenter(object.GetMassCenter());
		SetPosition(object.GetPosition());
		SetRotation(object.GetRotation());
	}

	RigidBody3D::RigidBody3D(RigidBody3D&& object) noexcept :
	m_geom(std::move(object.m_geom)),
	m_forceAccumulator(std::move(object.m_forceAccumulator)),
	m_torqueAccumulator(std::move(object.m_torqueAccumulator)),
	m_body(std::move(object.m_body)),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.m_mass)
	{
		if (m_body)
			NewtonBodySetUserData(m_body, this);
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
			case CoordSys::Global:
				m_forceAccumulator += force;
				break;

			case CoordSys::Local:
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
			case CoordSys::Global:
				m_forceAccumulator += force;
				m_torqueAccumulator += Vector3f::CrossProduct(point - GetMassCenter(CoordSys::Global), force);
				break;

			case CoordSys::Local:
			{
				Matrix4f transformMatrix = GetMatrix();
				return AddForce(transformMatrix.Transform(force, 0.f), transformMatrix.Transform(point), CoordSys::Global);
			}
		}

		// On réveille le corps pour que le callback soit appelé et que les forces soient appliquées
		NewtonBodySetSleepState(m_body, 0);
	}

	void RigidBody3D::AddTorque(const Vector3f& torque, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				m_torqueAccumulator += torque;
				break;

			case CoordSys::Local:
				Matrix4f transformMatrix = GetMatrix();
				m_torqueAccumulator += transformMatrix.Transform(torque, 0.f);
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
		NewtonBodyGetAABB(m_body, &min.x, &max.x);

		return Boxf(min, max);
	}

	Vector3f RigidBody3D::GetAngularDamping() const
	{
		Vector3f angularDamping;
		NewtonBodyGetAngularDamping(m_body, &angularDamping.x);

		return angularDamping;
	}

	Vector3f RigidBody3D::GetAngularVelocity() const
	{
		Vector3f angularVelocity;
		NewtonBodyGetOmega(m_body, &angularVelocity.x);

		return angularVelocity;
	}

	const std::shared_ptr<Collider3D>& RigidBody3D::GetGeom() const
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
		NewtonBodyGetVelocity(m_body, &velocity.x);

		return velocity;
	}

	float RigidBody3D::GetMass() const
	{
		return m_mass;
	}

	Vector3f RigidBody3D::GetMassCenter(CoordSys coordSys) const
	{
		Vector3f center;
		NewtonBodyGetCentreOfMass(m_body, &center.x);

		switch (coordSys)
		{
			case CoordSys::Global:
			{
				Matrix4f transformMatrix = GetMatrix();
				center = transformMatrix.Transform(center);
				break;
			}

			case CoordSys::Local:
				break; // Aucune opération à effectuer sur le centre de rotation
		}

		return center;
	}

	int RigidBody3D::GetMaterial() const
	{
		return NewtonBodyGetMaterialGroupID(m_body);
	}

	Matrix4f RigidBody3D::GetMatrix() const
	{
		Matrix4f matrix;
		NewtonBodyGetMatrix(m_body, &matrix.m11);

		return matrix;
	}

	Vector3f RigidBody3D::GetPosition() const
	{
		Vector3f pos;
		NewtonBodyGetPosition(m_body, &pos.x);

		return pos;
	}

	Quaternionf RigidBody3D::GetRotation() const
	{
		// NewtonBodyGetRotation output X, Y, Z, W and Nz::Quaternion stores W, X, Y, Z so we use a temporary array
		std::array<float, 4> rot;
		NewtonBodyGetRotation(m_body, rot.data());

		return Quaternionf(rot[3], rot[0], rot[1], rot[2]);
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
		NewtonBodySetAngularDamping(m_body, &angularDamping.x);
	}

	void RigidBody3D::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		NewtonBodySetOmega(m_body, &angularVelocity.x);
	}

	void RigidBody3D::SetGeom(std::shared_ptr<Collider3D> geom)
	{
		if (m_geom != geom)
		{
			if (geom)
				m_geom = std::move(geom);
			else
				m_geom = std::make_shared<NullCollider3D>();

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
		NewtonBodySetVelocity(m_body, &velocity.x);
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
				NewtonBodyGetMass(m_body, &m_mass, &Ix, &Iy, &Iz);

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
			NewtonBodySetCentreOfMass(m_body, &center.x);
	}

	void RigidBody3D::SetMaterial(const std::string& materialName)
	{
		SetMaterial(m_world->GetMaterial(materialName));
	}

	void RigidBody3D::SetMaterial(int materialIndex)
	{
		NewtonBodySetMaterialGroupID(m_body, materialIndex);
	}

	void RigidBody3D::SetPosition(const Vector3f& position)
	{
		Matrix4f transformMatrix = GetMatrix();
		transformMatrix.SetTranslation(position);

		UpdateBody(transformMatrix);
	}

	void RigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		Matrix4f transformMatrix = GetMatrix();
		transformMatrix.SetRotation(rotation);

		UpdateBody(transformMatrix);
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

	RigidBody3D& RigidBody3D::operator=(RigidBody3D&& object) noexcept
	{
		if (m_body)
			NewtonDestroyBody(m_body);

		m_body               = std::move(object.m_body);
		m_forceAccumulator   = std::move(object.m_forceAccumulator);
		m_geom               = std::move(object.m_geom);
		m_gravityFactor      = object.m_gravityFactor;
		m_mass               = object.m_mass;
		m_torqueAccumulator  = std::move(object.m_torqueAccumulator);
		m_world              = object.m_world;

		if (m_body)
			NewtonBodySetUserData(m_body, this);

		return *this;
	}

	void RigidBody3D::UpdateBody(const Matrix4f& transformMatrix)
	{
		NewtonBodySetMatrix(m_body, &transformMatrix.m11);

		if (NumberEquals(m_mass, 0.f))
		{
			// Moving a static body in Newton does not update bodies at the target location
			// http://newtondynamics.com/wiki/index.php5?title=Can_i_dynamicly_move_a_TriMesh%3F
			Vector3f min, max;
			NewtonBodyGetAABB(m_body, &min.x, &max.x);

			NewtonWorldForEachBodyInAABBDo(m_world->GetHandle(), &min.x, &max.x, [](const NewtonBody* const body, void* const userData) -> int
			{
				NazaraUnused(userData);
				NewtonBodySetSleepState(body, 0);
				return 1;
			},
			nullptr);
		}
	}

	void RigidBody3D::ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex)
	{
		NazaraUnused(timeStep);
		NazaraUnused(threadIndex);

		RigidBody3D* me = static_cast<RigidBody3D*>(NewtonBodyGetUserData(body));

		if (!NumberEquals(me->m_gravityFactor, 0.f))
			me->m_forceAccumulator += me->m_world->GetGravity() * me->m_gravityFactor * me->m_mass;

		NewtonBodySetForce(body, &me->m_forceAccumulator.x);
		NewtonBodySetTorque(body, &me->m_torqueAccumulator.x);

		me->m_torqueAccumulator.Set(0.f);
		me->m_forceAccumulator.Set(0.f);

		///TODO: Implement gyroscopic force?
	}
}
