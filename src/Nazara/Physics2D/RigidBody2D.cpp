// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_private.h>
#include <algorithm>
#include <cmath>
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
	m_isRegistered(false),
	m_isSimulationEnabled(true),
	m_isStatic(false),
	m_gravityFactor(1.f),
	m_mass(mass)
	{
		NazaraAssert(m_world, "Invalid world");

		m_handle = Create(mass);
		SetGeom(geom);
	}

	RigidBody2D::RigidBody2D(const RigidBody2D& object) :
	m_geom(object.m_geom),
	m_userData(object.m_userData),
	m_world(object.m_world),
	m_isRegistered(false),
	m_isSimulationEnabled(true),
	m_isStatic(object.m_isStatic),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.GetMass())
	{
		NazaraAssert(m_world, "Invalid world");
		NazaraAssert(m_geom, "Invalid geometry");

		m_handle = Create(m_mass, object.GetMomentOfInertia());
		SetGeom(object.GetGeom(), false);

		CopyBodyData(object.GetHandle(), m_handle);

		for (std::size_t i = 0; i < m_shapes.size(); ++i)
			m_shapes[i]->bb = cpShapeCacheBB(object.m_shapes[i]);
	}

	RigidBody2D::RigidBody2D(RigidBody2D&& object) :
	OnRigidBody2DMove(std::move(object.OnRigidBody2DMove)),
	OnRigidBody2DRelease(std::move(object.OnRigidBody2DRelease)),
	m_shapes(std::move(object.m_shapes)),
	m_geom(std::move(object.m_geom)),
	m_handle(object.m_handle),
	m_userData(object.m_userData),
	m_world(object.m_world),
	m_isRegistered(object.m_isRegistered),
	m_isSimulationEnabled(object.m_isSimulationEnabled),
	m_isStatic(object.m_isStatic),
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
		return AddForce(force, GetMassCenter(coordSys), coordSys);
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
		return AddImpulse(impulse, GetMassCenter(coordSys), coordSys);
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
		cpBodySetTorque(m_handle, cpBodyGetTorque(m_handle) + ToRadians(torque));
	}

	bool RigidBody2D::ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint, float* closestDistance) const
	{
		cpVect pos = cpv(cpFloat(position.x), cpFloat(position.y));

		float minDistance = std::numeric_limits<float>::infinity();
		Nz::Vector2f closest;
		for (cpShape* shape : m_shapes)
		{
			cpPointQueryInfo result;
			cpShapePointQuery(shape, pos, &result);

			float resultDistance = float(result.distance);
			if (resultDistance < minDistance)
			{
				closest.Set(float(result.point.x), float(result.point.y));
				minDistance = resultDistance;
			}
		}

		if (std::isinf(minDistance))
			return false;

		if (closestPoint)
			*closestPoint = closest;

		if (minDistance)
			*closestDistance = minDistance;

		return true;
	}

	void RigidBody2D::EnableSimulation(bool simulation)
	{
		if (m_isRegistered != simulation)
		{
			m_isRegistered = simulation;

			if (simulation)
				RegisterToSpace();
			else
				UnregisterFromSpace();
		}
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
		return FromRadians(static_cast<float>(cpBodyGetAngularVelocity(m_handle)));
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

	Vector2f RigidBody2D::GetMassCenter(CoordSys coordSys) const
	{
		cpVect massCenter = cpBodyGetCenterOfGravity(m_handle);

		switch (coordSys)
		{
		case CoordSys_Global:
			massCenter = cpBodyLocalToWorld(m_handle, massCenter);
			break;

		case CoordSys_Local:
			break; // Nothing to do
		}

		return Vector2f(static_cast<float>(massCenter.x), static_cast<float>(massCenter.y));
	}

	float RigidBody2D::GetMomentOfInertia() const
	{
		return float(cpBodyGetMoment(m_handle));
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
		return FromRadians(static_cast<float>(cpBodyGetAngle(m_handle)));
	}

	std::size_t RigidBody2D::GetShapeIndex(cpShape* shape) const
	{
		auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
		if (it == m_shapes.end())
			return InvalidShapeIndex;

		return std::distance(m_shapes.begin(), it);
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

	bool RigidBody2D::IsKinematic() const
	{
		return m_mass <= 0.f;
	}

	bool RigidBody2D::IsSimulationEnabled() const
	{
		return m_isRegistered;
	}

	bool RigidBody2D::IsSleeping() const
	{
		return cpBodyIsSleeping(m_handle) != 0;
	}

	bool RigidBody2D::IsStatic() const
	{
		return m_isStatic;
	}

	void RigidBody2D::SetAngularVelocity(float angularVelocity)
	{
		cpBodySetAngularVelocity(m_handle, ToRadians(angularVelocity));
	}

	void RigidBody2D::SetGeom(Collider2DRef geom, bool recomputeMoment)
	{
		// We have no public way of getting rid of an existing geom without removing the whole body
		// So let's save some attributes of the body, destroy it and rebuild it
		if (m_geom)
		{
			cpFloat mass = cpBodyGetMass(m_handle);
			cpFloat moment = cpBodyGetMoment(m_handle);

			cpBody* newHandle = Create(static_cast<float>(mass), static_cast<float>(moment));

			CopyBodyData(m_handle, newHandle);

			Destroy();

			m_handle = newHandle;
		}

		if (geom)
			m_geom = geom;
		else
			m_geom = NullCollider2D::New();

		m_shapes = m_geom->GenerateShapes(this);

		cpSpace* space = m_world->GetHandle();
		for (cpShape* shape : m_shapes)
			cpShapeSetUserData(shape, this);

		if (m_isSimulationEnabled)
			RegisterToSpace();

		if (recomputeMoment)
		{
			if (!IsStatic() && !IsKinematic())
				cpBodySetMoment(m_handle, m_geom->ComputeMomentOfInertia(m_mass));
		}
	}

	void RigidBody2D::SetMass(float mass, bool recomputeMoment)
	{
		if (m_mass > 0.f)
		{
			if (mass > 0.f)
			{
				m_world->RegisterPostStep(this, [mass, recomputeMoment](Nz::RigidBody2D* body)
				{
					cpBodySetMass(body->GetHandle(), mass);

					if (recomputeMoment)
						cpBodySetMoment(body->GetHandle(), body->GetGeom()->ComputeMomentOfInertia(mass));
				});
			}
			else
				m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body) { cpBodySetType(body->GetHandle(), (body->IsStatic()) ? CP_BODY_TYPE_STATIC : CP_BODY_TYPE_KINEMATIC); } );
		}
		else if (mass > 0.f)
		{
			m_world->RegisterPostStep(this, [mass, recomputeMoment](Nz::RigidBody2D* body)
			{
				if (cpBodyGetType(body->GetHandle()) != CP_BODY_TYPE_DYNAMIC)
				{
					cpBodySetType(body->GetHandle(), CP_BODY_TYPE_DYNAMIC);
					cpBodySetMass(body->GetHandle(), mass);

					if (recomputeMoment)
						cpBodySetMoment(body->GetHandle(), body->GetGeom()->ComputeMomentOfInertia(mass));
				}
			});
		}

		m_mass = mass;
	}

	void RigidBody2D::SetMassCenter(const Vector2f& center, CoordSys coordSys)
	{
		cpVect massCenter = cpv(center.x, center.y);

		switch (coordSys)
		{
		case CoordSys_Global:
			massCenter = cpBodyWorldToLocal(m_handle, massCenter);
			break;

		case CoordSys_Local:
			break; // Nothing to do
		}

		cpBodySetCenterOfGravity(m_handle, massCenter);
	}

	void RigidBody2D::SetMomentOfInertia(float moment)
	{
		// Even though Chipmunk allows us to change this anytime, we need to do it in a post-step to prevent other post-steps to override this
		m_world->RegisterPostStep(this, [moment] (Nz::RigidBody2D* body)
		{
			cpBodySetMoment(body->GetHandle(), moment);
		});
	}

	void RigidBody2D::SetPosition(const Vector2f& position)
	{
		cpBodySetPosition(m_handle, cpv(position.x, position.y));
		if (m_isStatic)
		{
			m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
			{
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			});
		}
	}

	void RigidBody2D::SetRotation(float rotation)
	{
		cpBodySetAngle(m_handle, ToRadians(rotation));
		if (m_isStatic)
		{
			m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
			{
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			});
		}
	}

	void RigidBody2D::SetStatic(bool setStaticBody)
	{
		m_isStatic = setStaticBody;
		m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
		{
			if (body->IsStatic())
			{
				cpBodySetType(body->GetHandle(), CP_BODY_TYPE_STATIC);
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			}
			else if (cpBodyGetMass(body->GetHandle()) > 0.f)
				cpBodySetType(body->GetHandle(), CP_BODY_TYPE_KINEMATIC);
			else
				cpBodySetType(body->GetHandle(), CP_BODY_TYPE_DYNAMIC);
		});
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

		m_handle              = object.m_handle;
		m_isRegistered        = object.m_isRegistered;
		m_isSimulationEnabled = object.m_isSimulationEnabled;
		m_isStatic            = object.m_isStatic;
		m_geom                = std::move(object.m_geom);
		m_gravityFactor       = object.m_gravityFactor;
		m_mass                = object.m_mass;
		m_shapes              = std::move(object.m_shapes);
		m_userData            = object.m_userData;
		m_world               = object.m_world;

		cpBodySetUserData(m_handle, this);
		for (cpShape* shape : m_shapes)
			cpShapeSetUserData(shape, this);

		object.m_handle = nullptr;

		OnRigidBody2DMove(&object, this);

		return *this;
	}

	cpBody* RigidBody2D::Create(float mass, float moment)
	{
		cpBody* handle;
		if (IsKinematic())
		{
			if (IsStatic())
				handle = cpBodyNewStatic();
			else
				handle = cpBodyNewKinematic();
		}
		else
			handle = cpBodyNew(mass, moment);

		cpBodySetUserData(handle, this);

		return handle;
	}

	void RigidBody2D::Destroy()
	{
		UnregisterFromSpace();

		cpSpace* space = m_world->GetHandle();
		for (cpShape* shape : m_shapes)
			cpShapeFree(shape);

		if (m_handle)
			cpBodyFree(m_handle);

		m_shapes.clear();
	}

	void RigidBody2D::RegisterToSpace()
	{
		if (!m_isRegistered)
		{
			cpSpace* space = m_world->GetHandle();
			for (cpShape* shape : m_shapes)
				cpSpaceAddShape(space, shape);

			if (m_handle)
				cpSpaceAddBody(space, m_handle);

			m_isRegistered = true;
		}
	}

	void RigidBody2D::UnregisterFromSpace()
	{
		if (m_isRegistered)
		{
			cpSpace* space = m_world->GetHandle();
			for (cpShape* shape : m_shapes)
				cpSpaceRemoveShape(space, shape);

			if (m_handle)
				cpSpaceRemoveBody(space, m_handle);

			m_isRegistered = false;
		}
	}

	void RigidBody2D::CopyBodyData(cpBody* from, cpBody* to)
	{
		cpBodySetAngle(to, cpBodyGetAngle(from));
		cpBodySetAngularVelocity(to, cpBodyGetAngularVelocity(from));
		cpBodySetCenterOfGravity(to, cpBodyGetCenterOfGravity(from));
		cpBodySetForce(to, cpBodyGetForce(from));
		cpBodySetPosition(to, cpBodyGetPosition(from));
		cpBodySetTorque(to, cpBodyGetTorque(from));
		cpBodySetVelocity(to, cpBodyGetVelocity(from));
	}

}
