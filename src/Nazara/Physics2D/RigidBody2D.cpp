// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_private.h>
#include <algorithm>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	RigidBody2D::RigidBody2D(PhysWorld2D* world, float mass) :
	RigidBody2D(world, mass, nullptr)
	{
	}

	RigidBody2D::RigidBody2D(PhysWorld2D* world, float mass, Collider2DRef geom) :
	m_geom(),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(world),
	m_gravityFactor(1.f),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");

		m_handle = cpBodyNew(0.f, 0.f);
		cpBodySetUserData(m_handle, this);
		cpSpaceAddBody(m_world->GetHandle(), m_handle);

		SetGeom(geom);
		SetMass(mass);
	}

	RigidBody2D::RigidBody2D(const RigidBody2D& object) :
	m_geom(object.m_geom),
	m_forceAccumulator(Vector3f::Zero()),
	m_torqueAccumulator(Vector3f::Zero()),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");
		NazaraAssert(m_geom, "Invalid geometry");

		m_handle = cpBodyNew(0.f, 0.f);
		cpBodySetUserData(m_handle, this);
		cpSpaceAddBody(m_world->GetHandle(), m_handle);

		SetGeom(object.GetGeom());
		SetMass(object.GetMass());
	}

	RigidBody2D::RigidBody2D(RigidBody2D&& object) :
	m_geom(std::move(object.m_geom)),
	m_forceAccumulator(std::move(object.m_forceAccumulator)),
	m_torqueAccumulator(std::move(object.m_torqueAccumulator)),
	m_handle(object.m_handle),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.m_mass)
	{
		object.m_handle = nullptr;
	}

	RigidBody2D::~RigidBody2D()
	{
		if (m_handle)
			cpBodyFree(m_handle);
	}

	void RigidBody2D::AddForce(const Vector2f& force, CoordSys coordSys)
	{
		return AddForce(force, GetCenterOfGravity(coordSys), coordSys);
	}

	void RigidBody2D::AddForce(const Vector2f& force, const Vector2f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys_Global:
				cpBodyApplyForceAtWorldPoint(m_handle, cpv(force.x, force.y), cpv(force.x, force.y));
				break;

			case CoordSys_Local:
				cpBodyApplyForceAtLocalPoint(m_handle, cpv(force.x, force.y), cpv(point.x, point.y));
				break;
		}
	}

	void RigidBody2D::AddTorque(float torque)
	{
		cpBodySetTorque(m_handle, cpBodyGetTorque(m_handle) + torque);
	}

	Rectf RigidBody2D::GetAABB() const
	{
		cpBB bb = cpBBNew(0.f, 0.f, 0.f, 0.f);
		for (cpShape* shape : m_shapes)
			bb = cpBBMerge(bb, cpShapeGetBB(shape));

		return Rectf(bb.l, bb.t, bb.r - bb.l, bb.b - bb.t);
	}

	float RigidBody2D::GetAngularVelocity() const
	{
		return cpBodyGetAngularVelocity(m_handle);
	}

	const Collider2DRef& RigidBody2D::GetGeom() const
	{
		return m_geom;
	}

	float RigidBody2D::GetGravityFactor() const
	{
		return m_gravityFactor;
	}

	cpBody* RigidBody2D::GetHandle() const
	{
		return m_handle;
	}

	float RigidBody2D::GetMass() const
	{
		return m_mass;
	}

	Vector2f RigidBody2D::GetCenterOfGravity(CoordSys coordSys) const
	{
		cpVect cog = cpBodyGetCenterOfGravity(m_handle);

		switch (coordSys)
		{
			case CoordSys_Global:
				cog = cpBodyLocalToWorld(m_handle, cog);
				break;

			case CoordSys_Local:
				break; // Nothing to do
		}

		return Vector2f(cog.x, cog.y);
	}

	Vector2f RigidBody2D::GetPosition() const
	{
		cpVect pos = cpBodyGetPosition(m_handle);
		return Vector2f(pos.x, pos.y);
	}

	float RigidBody2D::GetRotation() const
	{
		return cpBodyGetAngle(m_handle);
	}

	Vector2f RigidBody2D::GetVelocity() const
	{
		cpVect vel = cpBodyGetVelocity(m_handle);
		return Vector2f(vel.x, vel.y);
	}

	bool RigidBody2D::IsMoveable() const
	{
		return m_mass > 0.f;
	}

	bool RigidBody2D::IsSleeping() const
	{
		return cpBodyIsSleeping(m_handle) != 0;
	}

	void RigidBody2D::SetAngularVelocity(float angularVelocity)
	{
		cpBodySetAngularVelocity(m_handle, angularVelocity);
	}

	void RigidBody2D::SetGeom(Collider2DRef geom)
	{
		if (m_geom.Get() != geom)
		{
			for (cpShape* shape : m_shapes)
				cpBodyRemoveShape(m_handle, shape);

			if (geom)
				m_geom = geom;
			else
				m_geom = NullCollider2D::New();

			m_shapes = m_geom->CreateShapes(this);
		}
	}

	void RigidBody2D::SetGravityFactor(float gravityFactor)
	{
		m_gravityFactor = gravityFactor;
	}

	void RigidBody2D::SetMass(float mass)
	{
		if (m_mass > 0.f)
		{
			if (mass > 0.f)
				cpBodySetMass(m_handle, mass);
			else
				cpBodySetType(m_handle, CP_BODY_TYPE_STATIC);
		}
		else if (mass > 0.f)
		{
			if (cpBodyGetType(m_handle) == CP_BODY_TYPE_STATIC)
				cpBodySetType(m_handle, CP_BODY_TYPE_DYNAMIC);
		}

		m_mass = mass;
	}

	void RigidBody2D::SetMassCenter(const Vector2f& center)
	{
		if (m_mass > 0.f)
			cpBodySetCenterOfGravity(m_handle, cpv(center.x, center.y));
	}

	void RigidBody2D::SetPosition(const Vector2f& position)
	{
		cpBodySetPosition(m_handle, cpv(position.x, position.y));
	}

	void RigidBody2D::SetRotation(float rotation)
	{
		cpBodySetAngle(m_handle, rotation);
	}

	void RigidBody2D::SetVelocity(const Vector2f& velocity)
	{
		cpBodySetVelocity(m_handle, cpv(velocity.x, velocity.y));
	}

	RigidBody2D& RigidBody2D::operator=(const RigidBody2D& object)
	{
		RigidBody2D physObj(object);
		return operator=(std::move(physObj));
	}

	RigidBody2D& RigidBody2D::operator=(RigidBody2D&& object)
	{
		if (m_handle)
			cpBodyFree(m_handle);

		m_handle             = object.m_handle;
		m_forceAccumulator   = std::move(object.m_forceAccumulator);
		m_geom               = std::move(object.m_geom);
		m_gravityFactor      = object.m_gravityFactor;
		m_mass               = object.m_mass;
		m_torqueAccumulator  = std::move(object.m_torqueAccumulator);
		m_world              = object.m_world;

		object.m_handle = nullptr;

		return *this;
	}
}
