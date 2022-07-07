// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/Arbiter2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_private.h>
#include <algorithm>
#include <cmath>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	RigidBody2D::RigidBody2D(PhysWorld2D* world, float mass) :
	RigidBody2D(world, mass, nullptr)
	{
	}

	RigidBody2D::RigidBody2D(PhysWorld2D* world, float mass, std::shared_ptr<Collider2D> geom) :
	m_positionOffset(Vector2f::Zero()),
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
		SetGeom(std::move(geom));
	}

	RigidBody2D::RigidBody2D(const RigidBody2D& object) :
	m_positionOffset(object.m_positionOffset),
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
		SetGeom(object.GetGeom(), false, false);

		CopyBodyData(object.GetHandle(), m_handle);

		for (std::size_t i = 0; i < m_shapes.size(); ++i)
		{
			CopyShapeData(object.m_shapes[i], m_shapes[i]);
			m_shapes[i]->bb = cpShapeCacheBB(object.m_shapes[i]);
		}
	}

	RigidBody2D::RigidBody2D(RigidBody2D&& object) noexcept :
	OnRigidBody2DMove(std::move(object.OnRigidBody2DMove)),
	OnRigidBody2DRelease(std::move(object.OnRigidBody2DRelease)),
	m_positionOffset(std::move(object.m_positionOffset)),
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
			case CoordSys::Global:
				cpBodyApplyForceAtWorldPoint(m_handle, cpv(force.x, force.y), cpv(point.x, point.y));
				break;

			case CoordSys::Local:
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
			case CoordSys::Global:
				cpBodyApplyImpulseAtWorldPoint(m_handle, cpv(impulse.x, impulse.y), cpv(point.x, point.y));
				break;

			case CoordSys::Local:
				cpBodyApplyImpulseAtLocalPoint(m_handle, cpv(impulse.x, impulse.y), cpv(point.x, point.y));
				break;
		}
	}

	void RigidBody2D::AddTorque(const RadianAnglef& torque)
	{
		cpBodySetTorque(m_handle, cpBodyGetTorque(m_handle) + torque.value);
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

		if (closestDistance)
			*closestDistance = minDistance;

		return true;
	}

	void RigidBody2D::EnableSimulation(bool simulation)
	{
		if (m_isSimulationEnabled != simulation)
		{
			m_isSimulationEnabled = simulation;

			if (simulation)
				RegisterToSpace();
			else
				UnregisterFromSpace();
		}
	}

	void RigidBody2D::ForEachArbiter(std::function<void(Nz::Arbiter2D&)> callback)
	{
		using CallbackType = decltype(callback);

		auto RealCallback = [](cpBody* /*body*/, cpArbiter* arbiter, void* data)
		{
			CallbackType& cb = *static_cast<CallbackType*>(data);

			Arbiter2D nzArbiter(arbiter);
			cb(nzArbiter);
		};

		cpBodyEachArbiter(m_handle, RealCallback, &callback);
	}

	void RigidBody2D::ForceSleep()
	{
		m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
		{
			if (cpBodyGetType(body->GetHandle()) == CP_BODY_TYPE_DYNAMIC)
				cpBodySleep(body->GetHandle());
		});
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

	RadianAnglef RigidBody2D::GetAngularVelocity() const
	{
		return float(cpBodyGetAngularVelocity(m_handle));
	}

	float Nz::RigidBody2D::GetElasticity(std::size_t shapeIndex) const
	{
		assert(shapeIndex < m_shapes.size());
		return float(cpShapeGetElasticity(m_shapes[shapeIndex]));
	}

	float Nz::RigidBody2D::GetFriction(std::size_t shapeIndex) const
	{
		assert(shapeIndex < m_shapes.size());
		return float(cpShapeGetFriction(m_shapes[shapeIndex]));
	}

	const std::shared_ptr<Collider2D>& RigidBody2D::GetGeom() const
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
			case CoordSys::Global:
				massCenter = cpBodyLocalToWorld(m_handle, massCenter);
				break;

			case CoordSys::Local:
				break; // Nothing to do
		}

		return Vector2f(static_cast<float>(massCenter.x), static_cast<float>(massCenter.y));
	}

	float RigidBody2D::GetMomentOfInertia() const
	{
		return float(cpBodyGetMoment(m_handle));
	}

	Vector2f RigidBody2D::GetPosition() const
	{
		cpVect pos = cpBodyLocalToWorld(m_handle, cpv(-m_positionOffset.x, -m_positionOffset.y));
		return Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y));
	}

	RadianAnglef RigidBody2D::GetRotation() const
	{
		return float(cpBodyGetAngle(m_handle));
	}

	std::size_t RigidBody2D::GetShapeIndex(cpShape* shape) const
	{
		auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
		if (it == m_shapes.end())
			return InvalidShapeIndex;

		return std::distance(m_shapes.begin(), it);
	}

	Vector2f Nz::RigidBody2D::GetSurfaceVelocity(std::size_t shapeIndex) const
	{
		assert(shapeIndex < m_shapes.size());
		cpVect vel = cpShapeGetSurfaceVelocity(m_shapes[shapeIndex]);
		return Vector2f(static_cast<float>(vel.x), static_cast<float>(vel.y));
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

	const RigidBody2D::VelocityFunc& RigidBody2D::GetVelocityFunction() const
	{
		return m_velocityFunc;
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
		return m_isSimulationEnabled;
	}

	bool RigidBody2D::IsSleeping() const
	{
		return cpBodyIsSleeping(m_handle) != 0;
	}

	bool RigidBody2D::IsStatic() const
	{
		return m_isStatic;
	}

	void RigidBody2D::ResetVelocityFunction()
	{
		m_handle->velocity_func = cpBodyUpdateVelocity;
	}

	void RigidBody2D::SetAngularVelocity(const RadianAnglef& angularVelocity)
	{
		cpBodySetAngularVelocity(m_handle, angularVelocity.value);
	}

	void RigidBody2D::SetElasticity(float friction)
	{
		cpFloat frict(friction);
		for (cpShape* shape : m_shapes)
			cpShapeSetElasticity(shape, frict);
	}

	void RigidBody2D::SetElasticity(std::size_t shapeIndex, float friction)
	{
		assert(shapeIndex < m_shapes.size());
		cpShapeSetElasticity(m_shapes[shapeIndex], cpFloat(friction));
	}

	void RigidBody2D::SetFriction(float friction)
	{
		cpFloat frict(friction);
		for (cpShape* shape : m_shapes)
			cpShapeSetFriction(shape, frict);
	}

	void RigidBody2D::SetFriction(std::size_t shapeIndex, float friction)
	{
		assert(shapeIndex < m_shapes.size());
		cpShapeSetFriction(m_shapes[shapeIndex], cpFloat(friction));
	}

	void RigidBody2D::SetGeom(std::shared_ptr<Collider2D> geom, bool recomputeMoment, bool recomputeMassCenter)
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
			m_geom = std::move(geom);
		else
			m_geom = std::make_shared<NullCollider2D>();

		m_geom->GenerateShapes(m_handle, &m_shapes);

		for (cpShape* shape : m_shapes)
			cpShapeSetUserData(shape, this);

		if (m_isSimulationEnabled)
			RegisterToSpace();

		if (recomputeMoment)
		{
			if (!IsStatic() && !IsKinematic())
				cpBodySetMoment(m_handle, m_geom->ComputeMomentOfInertia(m_mass));
		}

		if (recomputeMassCenter)
			SetMassCenter(m_geom->ComputeCenterOfMass());
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
			case CoordSys::Global:
				massCenter = cpBodyWorldToLocal(m_handle, massCenter);
				break;

			case CoordSys::Local:
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
		// Use cpTransformVect to rotate/scale the position offset
		cpBodySetPosition(m_handle, cpvadd(cpv(position.x, position.y), cpTransformVect(m_handle->transform, cpv(m_positionOffset.x, m_positionOffset.y))));
		if (m_isStatic)
		{
			m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
			{
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			});
		}
	}

	void RigidBody2D::SetPositionOffset(const Vector2f& offset)
	{
		Nz::Vector2f position = GetPosition();
		m_positionOffset = offset;
		SetPosition(position);
	}

	void RigidBody2D::SetRotation(const RadianAnglef& rotation)
	{
		cpBodySetAngle(m_handle, rotation.value);
		if (m_isStatic)
		{
			m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
			{
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			});
		}
	}

	void RigidBody2D::SetSurfaceVelocity(const Vector2f& surfaceVelocity)
	{
		Vector2<cpFloat> velocity(surfaceVelocity.x, surfaceVelocity.y);
		for (cpShape* shape : m_shapes)
			cpShapeSetSurfaceVelocity(shape, cpv(velocity.x, velocity.y));
	}

	void RigidBody2D::SetSurfaceVelocity(std::size_t shapeIndex, const Vector2f& surfaceVelocity)
	{
		assert(shapeIndex < m_shapes.size());
		cpShapeSetSurfaceVelocity(m_shapes[shapeIndex], cpv(cpFloat(surfaceVelocity.x), cpFloat(surfaceVelocity.y)));
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

	void RigidBody2D::SetVelocityFunction(VelocityFunc velocityFunc)
	{
		m_velocityFunc = std::move(velocityFunc);

		if (m_velocityFunc)
		{
			m_handle->velocity_func = [](cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt)
			{
				RigidBody2D* rigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(body));
				const auto& callback = rigidBody->GetVelocityFunction();
				assert(callback);

				callback(*rigidBody, Nz::Vector2f(float(gravity.x), float(gravity.y)), float(damping), float(dt));
			};
		}
		else
			m_handle->velocity_func = cpBodyUpdateVelocity;
	}

	void RigidBody2D::UpdateVelocity(const Nz::Vector2f & gravity, float damping, float deltaTime)
	{
		cpBodyUpdateVelocity(m_handle, cpv(gravity.x, gravity.y), damping, deltaTime);
	}

	void RigidBody2D::Wakeup()
	{
		m_world->RegisterPostStep(this, [](Nz::RigidBody2D* body)
		{
			if (cpBodyGetType(body->GetHandle()) != CP_BODY_TYPE_STATIC)
				cpBodyActivate(body->GetHandle());
			else
				cpBodyActivateStatic(body->GetHandle(), nullptr);
		});
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
		m_positionOffset      = object.m_positionOffset;
		m_shapes              = std::move(object.m_shapes);
		m_userData            = object.m_userData;
		m_velocityFunc        = std::move(object.m_velocityFunc);
		m_world               = object.m_world;

		if (m_handle)
		{
			cpBodySetUserData(m_handle, this);
			for (cpShape* shape : m_shapes)
				cpShapeSetUserData(shape, this);
		}

		object.m_handle = nullptr;

		OnRigidBody2DMove(&object, this);

		return *this;
	}

	void RigidBody2D::Destroy()
	{
		UnregisterFromSpace();

		for (cpShape* shape : m_shapes)
			cpShapeFree(shape);

		if (m_handle)
		{
			cpBodyFree(m_handle);
			m_handle = nullptr;
		}

		m_shapes.clear();
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
		cpBodySetCenterOfGravity(to, cpBodyGetCenterOfGravity(from));

		cpBodySetAngle(to, cpBodyGetAngle(from));
		cpBodySetAngularVelocity(to, cpBodyGetAngularVelocity(from));
		cpBodySetForce(to, cpBodyGetForce(from));
		cpBodySetPosition(to, cpBodyGetPosition(from));
		cpBodySetTorque(to, cpBodyGetTorque(from));
		cpBodySetVelocity(to, cpBodyGetVelocity(from));

		cpBodySetType(to, cpBodyGetType(from));

		to->velocity_func = from->velocity_func;
	}

	void RigidBody2D::CopyShapeData(cpShape* from, cpShape* to)
	{
		cpShapeSetElasticity(to, cpShapeGetElasticity(from));
		cpShapeSetFriction(to, cpShapeGetFriction(from));
		cpShapeSetSurfaceVelocity(to, cpShapeGetSurfaceVelocity(from));
	}
}
