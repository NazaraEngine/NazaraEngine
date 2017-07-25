// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
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
	m_userData(nullptr),
	m_world(world),
	m_gravityFactor(1.f),
	m_mass(1.f)
	{
		NazaraAssert(m_world, "Invalid world");

		Create();

		SetGeom(geom);
		SetMass(mass);
	}

	RigidBody2D::RigidBody2D(const RigidBody2D& object) :
	m_geom(object.m_geom),
	m_userData(object.m_userData),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(0.f)
	{
		NazaraAssert(m_world, "Invalid world");
		NazaraAssert(m_geom, "Invalid geometry");

		Create();

		SetGeom(object.GetGeom());
		SetMass(object.GetMass());
	}

	RigidBody2D::RigidBody2D(RigidBody2D&& object) :
	OnRigidBody2DMove(std::move(object.OnRigidBody2DMove)),
	OnRigidBody2DRelease(std::move(object.OnRigidBody2DRelease)),
	m_shapes(std::move(object.m_shapes)),
	m_geom(std::move(object.m_geom)),
	m_userData(object.m_userData),
	m_handle(object.m_handle),
	m_world(object.m_world),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.m_mass)
	{
		cpBodySetUserData(m_handle, this);
		for (cpShape* shape : m_shapes)
			cpShapeSetUserData(shape, this);

		object.m_handle = nullptr;

		OnRigidBody2DMove(&object, this);
	}

	RigidBody2D::~RigidBody2D()
	{
		OnRigidBody2DRelease(this);

		Destroy();
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
				cpBodyApplyForceAtWorldPoint(m_handle, cpv(force.x, force.y), cpv(point.x, point.y));
				break;

			case CoordSys_Local:
				cpBodyApplyForceAtLocalPoint(m_handle, cpv(force.x, force.y), cpv(point.x, point.y));
				break;
		}
	}

	void RigidBody2D::AddImpulse(const Vector2f& impulse, CoordSys coordSys)
	{
		return AddImpulse(impulse, GetCenterOfGravity(coordSys), coordSys);
	}

	void RigidBody2D::AddImpulse(const Vector2f& impulse, const Vector2f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys_Global:
				cpBodyApplyImpulseAtWorldPoint(m_handle, cpv(impulse.x, impulse.y), cpv(point.x, point.y));
				break;

			case CoordSys_Local:
				cpBodyApplyImpulseAtLocalPoint(m_handle, cpv(impulse.x, impulse.y), cpv(point.x, point.y));
				break;
		}
	}

	void RigidBody2D::AddTorque(float torque)
	{
		cpBodySetTorque(m_handle, cpBodyGetTorque(m_handle) + torque);
	}

	Rectf RigidBody2D::GetAABB() const
	{
		if (m_shapes.empty())
			return Rectf::Zero();

		auto it = m_shapes.begin();
		cpBB bb = cpShapeGetBB(*it++);
		for (; it != m_shapes.end(); ++it)
			bb = cpBBMerge(bb, cpShapeGetBB(*it));

		return Rectf(Rect<cpFloat>(bb.l, bb.b, bb.r - bb.l, bb.t - bb.b));
	}

	float RigidBody2D::GetAngularVelocity() const
	{
		return static_cast<float>(cpBodyGetAngularVelocity(m_handle));
	}

	const Collider2DRef& RigidBody2D::GetGeom() const
	{
		return m_geom;
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

		return Vector2f(static_cast<float>(cog.x), static_cast<float>(cog.y));
	}

	Vector2f RigidBody2D::GetPosition() const
	{
		cpVect pos = cpBodyGetPosition(m_handle);
		return Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y));
	}

	float RigidBody2D::GetRotation() const
	{
		return static_cast<float>(cpBodyGetAngle(m_handle));
	}

	void* RigidBody2D::GetUserdata() const
	{
		return m_userData;
	}

	Vector2f RigidBody2D::GetVelocity() const
	{
		cpVect vel = cpBodyGetVelocity(m_handle);
		return Vector2f(static_cast<float>(vel.x), static_cast<float>(vel.y));
	}

	PhysWorld2D* RigidBody2D::GetWorld() const
	{
		return m_world;
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
		// We have no public way of getting rid of an existing geom without removing the whole body
		// So let's save some attributes of the body, destroy it and rebuild it
		if (m_geom)
		{
			cpVect pos = cpBodyGetPosition(m_handle);
			cpFloat mass = cpBodyGetMass(m_handle);
			cpFloat moment = cpBodyGetMoment(m_handle);
			cpFloat rot = cpBodyGetAngle(m_handle);
			cpVect vel = cpBodyGetVelocity(m_handle);

			Destroy();
			Create(float(mass), float(moment));

			cpBodySetAngle(m_handle, rot);
			cpBodySetPosition(m_handle, pos);
			cpBodySetVelocity(m_handle, vel);
		}

		if (geom)
			m_geom = geom;
		else
			m_geom = NullCollider2D::New();

		m_shapes = m_geom->GenerateShapes(this);

		cpSpace* space = m_world->GetHandle();
		for (cpShape* shape : m_shapes)
		{
			cpShapeSetUserData(shape, this);
			cpSpaceAddShape(space, shape);
		}

		cpBodySetMoment(m_handle, m_geom->ComputeInertialMatrix(m_mass));
	}

	void RigidBody2D::SetMass(float mass)
	{
		if (m_mass > 0.f)
		{
			if (mass > 0.f)
			{
				m_world->RegisterPostStep(this, [mass](Nz::RigidBody2D* body)
				{
					cpBodySetMass(body->GetHandle(), mass);
					cpBodySetMoment(body->GetHandle(), body->GetGeom()->ComputeInertialMatrix(mass));
				});
			}
			else
				m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body) { cpBodySetType(body->GetHandle(), CP_BODY_TYPE_STATIC); } );
		}
		else if (mass > 0.f)
		{
			m_world->RegisterPostStep(this, [mass](Nz::RigidBody2D* body)
			{
				if (cpBodyGetType(body->GetHandle()) == CP_BODY_TYPE_STATIC)
				{
					cpBodySetType(body->GetHandle(), CP_BODY_TYPE_DYNAMIC);
					cpBodySetMass(body->GetHandle(), mass);
					cpBodySetMoment(body->GetHandle(), body->GetGeom()->ComputeInertialMatrix(mass));
				}
			});
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
		if (cpBodyGetType(m_handle) == CP_BODY_TYPE_STATIC)
			cpSpaceReindexShapesForBody(m_world->GetHandle(), m_handle);
	}

	void RigidBody2D::SetRotation(float rotation)
	{
		cpBodySetAngle(m_handle, rotation);
	}

	void RigidBody2D::SetUserdata(void* ud)
	{
		m_userData = ud;
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
		Destroy();

		OnRigidBody2DMove    = std::move(object.OnRigidBody2DMove);
		OnRigidBody2DRelease = std::move(object.OnRigidBody2DRelease);

		m_handle             = object.m_handle;
		m_geom               = std::move(object.m_geom);
		m_gravityFactor      = object.m_gravityFactor;
		m_mass               = object.m_mass;
		m_shapes             = std::move(object.m_shapes);
		m_userData           = object.m_userData;
		m_world              = object.m_world;

		cpBodySetUserData(m_handle, this);
		for (cpShape* shape : m_shapes)
			cpShapeSetUserData(shape, this);

		object.m_handle = nullptr;

		OnRigidBody2DMove(&object, this);

		return *this;
	}

	void RigidBody2D::Create(float mass, float moment)
	{
		m_handle = cpBodyNew(mass, moment);
		cpBodySetUserData(m_handle, this);

		cpSpaceAddBody(m_world->GetHandle(), m_handle);
	}

	void RigidBody2D::Destroy()
	{
		cpSpace* space = m_world->GetHandle();
		for (cpShape* shape : m_shapes)
		{
			cpSpaceRemoveShape(space, shape);
			cpShapeFree(shape);
		}

		if (m_handle)
		{
			cpSpaceRemoveBody(space, m_handle);
			cpBodyFree(m_handle);
		}
	}
}
