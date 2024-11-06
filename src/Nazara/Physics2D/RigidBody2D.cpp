// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/ChipmunkHelper.hpp>
#include <Nazara/Physics2D/PhysArbiter2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_private.h>
#include <algorithm>
#include <cmath>

namespace Nz
{
	RigidBody2D::RigidBody2D(const RigidBody2D& object) :
	m_collider(object.m_collider),
	m_world(object.m_world),
	m_positionOffset(object.m_positionOffset),
	m_isRegistered(false),
	m_isSimulationEnabled(true),
	m_isStatic(object.m_isStatic),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.GetMass())
	{
		NazaraAssert(m_world, "Invalid world");
		NazaraAssert(m_collider, "Invalid geometry");

		m_bodyIndex = m_world->RegisterBody(*this);
		m_handle = cpBodyNew(m_mass, object.GetMomentOfInertia());
		cpBodySetUserData(m_handle, this);

		SetCollider(object.GetCollider(), false, false);
		SetVelocityFunction(object.m_velocityFunc);

		CopyBodyData(object.GetHandle(), m_handle);

		for (std::size_t i = 0; i < m_shapes.size(); ++i)
		{
			CopyShapeData(object.m_shapes[i], m_shapes[i]);
			m_shapes[i]->bb = cpShapeCacheBB(object.m_shapes[i]);
		}
	}

	RigidBody2D::RigidBody2D(RigidBody2D&& object) noexcept :
	m_shapes(std::move(object.m_shapes)),
	m_collider(std::move(object.m_collider)),
	m_handle(object.m_handle),
	m_world(object.m_world),
	m_bodyIndex(object.m_bodyIndex),
	m_positionOffset(std::move(object.m_positionOffset)),
	m_isRegistered(object.m_isRegistered),
	m_isSimulationEnabled(object.m_isSimulationEnabled),
	m_isStatic(object.m_isStatic),
	m_gravityFactor(object.m_gravityFactor),
	m_mass(object.m_mass)
	{
		if (m_handle)
		{
			cpBodySetUserData(m_handle, this);
			for (cpShape* shape : m_shapes)
				cpShapeSetUserData(shape, this);

			m_world->UpdateBodyPointer(*this);
		}

		object.m_bodyIndex = InvalidBodyIndex;
		object.m_handle = nullptr;
	}

	void RigidBody2D::AddForce(const Vector2f& force, const Vector2f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				cpBodyApplyForceAtWorldPoint(m_handle, ToChipmunk(force), ToChipmunk(point));
				break;

			case CoordSys::Local:
				cpBodyApplyForceAtLocalPoint(m_handle, ToChipmunk(force), ToChipmunk(point));
				break;
		}
	}

	void RigidBody2D::AddImpulse(const Vector2f& impulse, const Vector2f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				cpBodyApplyImpulseAtWorldPoint(m_handle, ToChipmunk(impulse), ToChipmunk(point));
				break;

			case CoordSys::Local:
				cpBodyApplyImpulseAtLocalPoint(m_handle, ToChipmunk(impulse), ToChipmunk(point));
				break;
		}
	}

	void RigidBody2D::AddTorque(const RadianAnglef& torque)
	{
		cpBodySetTorque(m_handle, cpBodyGetTorque(m_handle) + torque.value);
	}

	bool RigidBody2D::ClosestPointQuery(const Vector2f& position, Vector2f* closestPoint, float* closestDistance) const
	{
		cpVect pos = cpv(cpFloat(position.x), cpFloat(position.y));

		float minDistance = std::numeric_limits<float>::infinity();
		Vector2f closest;
		for (cpShape* shape : m_shapes)
		{
			cpPointQueryInfo result;
			cpShapePointQuery(shape, pos, &result);

			float resultDistance = float(result.distance);
			if (resultDistance < minDistance)
			{
				closest = Vector2f(float(result.point.x), float(result.point.y));
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

	void RigidBody2D::ForEachArbiter(const FunctionRef<void(PhysArbiter2D&)>& callback)
	{
		using CallbackType = std::remove_reference_t<decltype(callback)>;

		auto Trampoline = [](cpBody* /*body*/, cpArbiter* arbiter, void* data)
		{
			CallbackType& cb = *static_cast<CallbackType*>(data);

			PhysArbiter2D wrappedArbiter(arbiter);
			cb(wrappedArbiter);
		};

		cpBodyEachArbiter(m_handle, Trampoline, const_cast<void*>(static_cast<const void*>(&callback)));
	}

	void RigidBody2D::ForceSleep()
	{
		m_world->DeferBodyAction(*this, [](RigidBody2D* body)
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

	float RigidBody2D::GetElasticity(std::size_t shapeIndex) const
	{
		assert(shapeIndex < m_shapes.size());
		return float(cpShapeGetElasticity(m_shapes[shapeIndex]));
	}

	float RigidBody2D::GetFriction(std::size_t shapeIndex) const
	{
		assert(shapeIndex < m_shapes.size());
		return float(cpShapeGetFriction(m_shapes[shapeIndex]));
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

	Vector2f RigidBody2D::GetSurfaceVelocity(std::size_t shapeIndex) const
	{
		assert(shapeIndex < m_shapes.size());
		cpVect vel = cpShapeGetSurfaceVelocity(m_shapes[shapeIndex]);
		return Vector2f(static_cast<float>(vel.x), static_cast<float>(vel.y));
	}

	Vector2f RigidBody2D::GetVelocity() const
	{
		cpVect vel = cpBodyGetVelocity(m_handle);
		return Vector2f(static_cast<float>(vel.x), static_cast<float>(vel.y));
	}

	bool RigidBody2D::IsSleeping() const
	{
		return cpBodyIsSleeping(m_handle) != 0;
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

	void RigidBody2D::SetCollider(std::shared_ptr<Collider2D> collider, bool recomputeMoment, bool recomputeMassCenter)
	{
		// We have no public way of getting rid of an existing collider without removing the whole body
		// So let's save some attributes of the body, destroy it and rebuild it
		if (m_collider)
		{
			cpFloat mass = cpBodyGetMass(m_handle);
			cpFloat moment = cpBodyGetMoment(m_handle);

			cpBody* newHandle = cpBodyNew(1.f, 0.f);
			cpBodySetUserData(newHandle, this);

			CopyBodyData(m_handle, newHandle);

			if (cpBodyGetType(m_handle) == CP_BODY_TYPE_DYNAMIC)
			{
				cpBodySetMass(newHandle, mass);
				cpBodySetMoment(newHandle, moment);
			}

			DestroyBody();

			m_handle = newHandle;
		}

		if (collider)
			m_collider = std::move(collider);
		else
			m_collider = std::make_shared<NullCollider2D>();

		m_collider->GenerateShapes(m_handle, &m_shapes);

		for (cpShape* shape : m_shapes)
			cpShapeSetUserData(shape, this);

		if (m_isSimulationEnabled)
			RegisterToSpace();

		if (recomputeMoment)
		{
			if (!IsStatic() && !IsKinematic())
				cpBodySetMoment(m_handle, m_collider->ComputeMomentOfInertia(m_mass));
		}

		if (recomputeMassCenter)
			SetMassCenter(m_collider->ComputeCenterOfMass());
	}

	void RigidBody2D::SetMass(float mass, bool recomputeMoment)
	{
		if (m_mass > 0.f)
		{
			if (mass > 0.f)
			{
				m_world->DeferBodyAction(*this, [mass, recomputeMoment](RigidBody2D* body)
				{
					cpBodySetMass(body->GetHandle(), mass);

					if (recomputeMoment)
						cpBodySetMoment(body->GetHandle(), body->GetCollider()->ComputeMomentOfInertia(mass));
				});
			}
			else
				m_world->DeferBodyAction(*this, [](RigidBody2D* body) { cpBodySetType(body->GetHandle(), (body->IsStatic()) ? CP_BODY_TYPE_STATIC : CP_BODY_TYPE_KINEMATIC); } );
		}
		else if (mass > 0.f)
		{
			m_world->DeferBodyAction(*this, [mass, recomputeMoment](RigidBody2D* body)
			{
				if (cpBodyGetType(body->GetHandle()) != CP_BODY_TYPE_DYNAMIC)
				{
					cpBodySetType(body->GetHandle(), CP_BODY_TYPE_DYNAMIC);
					cpBodySetMass(body->GetHandle(), mass);

					if (recomputeMoment)
						cpBodySetMoment(body->GetHandle(), body->GetCollider()->ComputeMomentOfInertia(mass));
				}
			});
		}

		m_mass = mass;
	}

	void RigidBody2D::SetMassCenter(const Vector2f& center, CoordSys coordSys)
	{
		cpVect massCenter = ToChipmunk(center);

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
		m_world->DeferBodyAction(*this, [moment] (RigidBody2D* body)
		{
			cpBodySetMoment(body->GetHandle(), moment);
		});
	}

	void RigidBody2D::SetPosition(const Vector2f& position)
	{
		// Use cpTransformVect to rotate/scale the position offset
		cpBodySetPosition(m_handle, cpvadd(ToChipmunk(position), cpTransformVect(m_handle->transform, ToChipmunk(m_positionOffset))));
		if (m_isStatic)
		{
			m_world->DeferBodyAction(*this, [](RigidBody2D* body)
			{
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			});
		}
	}

	void RigidBody2D::SetPositionOffset(const Vector2f& offset)
	{
		Vector2f position = GetPosition();
		m_positionOffset = offset;
		SetPosition(position);
	}

	void RigidBody2D::SetRotation(const RadianAnglef& rotation)
	{
		cpBodySetAngle(m_handle, rotation.value);
		if (m_isStatic)
		{
			m_world->DeferBodyAction(*this, [](RigidBody2D* body)
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
		m_world->DeferBodyAction(*this, [](RigidBody2D* body)
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

	void RigidBody2D::SetVelocity(const Vector2f& velocity)
	{
		cpBodySetVelocity(m_handle, ToChipmunk(velocity));
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

				callback(*rigidBody, Vector2f(float(gravity.x), float(gravity.y)), float(damping), float(dt));
			};
		}
		else
			m_handle->velocity_func = cpBodyUpdateVelocity;
	}

	void RigidBody2D::TeleportTo(const Vector2f& position, const RadianAnglef& rotation)
	{
		// Use cpTransformVect to rotate/scale the position offset
		cpBodySetPosition(m_handle, cpvadd(ToChipmunk(position), cpTransformVect(m_handle->transform, ToChipmunk(m_positionOffset))));
		cpBodySetAngle(m_handle, rotation.value);
		if (m_isStatic)
		{
			m_world->DeferBodyAction(*this, [](RigidBody2D* body)
			{
				cpSpaceReindexShapesForBody(body->GetWorld()->GetHandle(), body->GetHandle());
			});
		}
	}

	RadianAnglef RigidBody2D::ToLocal(const RadianAnglef& worldRotation)
	{
		return worldRotation - GetRotation();
	}

	Vector2f RigidBody2D::ToLocal(const Vector2f& worldPosition)
	{
		return FromChipmunk(cpBodyWorldToLocal(m_handle, ToChipmunk(worldPosition)));
	}

	RadianAnglef RigidBody2D::ToWorld(const RadianAnglef& localRotation)
	{
		return GetRotation() + localRotation;
	}

	Vector2f RigidBody2D::ToWorld(const Vector2f& localPosition)
	{
		return FromChipmunk(cpBodyLocalToWorld(m_handle, ToChipmunk(localPosition)));
	}

	void RigidBody2D::UpdateVelocity(const Vector2f& gravity, float damping, float deltaTime)
	{
		cpBodyUpdateVelocity(m_handle, ToChipmunk(gravity), damping, deltaTime);
	}

	void RigidBody2D::Wakeup()
	{
		m_world->DeferBodyAction(*this, [](RigidBody2D* body)
		{
			if (cpBodyGetType(body->GetHandle()) != CP_BODY_TYPE_STATIC)
				cpBodyActivate(body->GetHandle());
			else
				cpBodyActivateStatic(body->GetHandle(), nullptr);
		});
	}

	RigidBody2D& RigidBody2D::operator=(const RigidBody2D& object)
	{
		return operator=(RigidBody2D(object));
	}

	RigidBody2D& RigidBody2D::operator=(RigidBody2D&& object) noexcept
	{
		Destroy();

		m_bodyIndex           = object.m_bodyIndex;
		m_handle              = object.m_handle;
		m_isRegistered        = object.m_isRegistered;
		m_isSimulationEnabled = object.m_isSimulationEnabled;
		m_isStatic            = object.m_isStatic;
		m_collider            = std::move(object.m_collider);
		m_gravityFactor       = object.m_gravityFactor;
		m_mass                = object.m_mass;
		m_positionOffset      = object.m_positionOffset;
		m_shapes              = std::move(object.m_shapes);
		m_velocityFunc        = std::move(object.m_velocityFunc);
		m_world               = object.m_world;

		if (m_handle)
		{
			cpBodySetUserData(m_handle, this);
			for (cpShape* shape : m_shapes)
				cpShapeSetUserData(shape, this);

			m_world->UpdateBodyPointer(*this);
		}

		object.m_bodyIndex = InvalidBodyIndex;
		object.m_handle = nullptr;

		return *this;
	}

	void RigidBody2D::Create(PhysWorld2D& world, const DynamicSettings& settings)
	{
		m_isRegistered = false;
		m_isSimulationEnabled = settings.isSimulationEnabled;
		m_isStatic = false;
		m_gravityFactor = settings.gravityFactor;
		m_mass = settings.mass;
		m_positionOffset = Vector2f::Zero();
		m_world = &world;

		m_bodyIndex = m_world->RegisterBody(*this);

		m_handle = (m_mass > 0.f) ? cpBodyNew(m_mass, 0.f) : cpBodyNewKinematic(); // moment will be recomputed by SetGeom
		cpBodySetUserData(m_handle, this);

		SetCollider(settings.collider);
		SetAngularVelocity(settings.angularVelocity);
		SetPosition(settings.position);
		SetRotation(settings.rotation);
		SetVelocity(settings.linearVelocity);
	}

	void RigidBody2D::Create(PhysWorld2D& world, const StaticSettings& settings)
	{
		m_gravityFactor = 1.f;
		m_isRegistered = false;
		m_isSimulationEnabled = settings.isSimulationEnabled;
		m_isStatic = true;
		m_mass = 0.f;
		m_positionOffset = Vector2f::Zero();
		m_world = &world;

		m_bodyIndex = m_world->RegisterBody(*this);

		m_handle = cpBodyNewStatic();
		cpBodySetUserData(m_handle, this);

		SetCollider(settings.collider);
		SetPosition(settings.position);
		SetRotation(settings.rotation);
	}

	void RigidBody2D::Destroy()
	{
		if (m_bodyIndex != InvalidBodyIndex)
		{
			m_world->UnregisterBody(m_bodyIndex);
			m_bodyIndex = InvalidBodyIndex;
		}

		DestroyBody();
	}

	void RigidBody2D::DestroyBody()
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
		cpBodySetType(to, cpBodyGetType(from));

		cpBodySetCenterOfGravity(to, cpBodyGetCenterOfGravity(from));

		cpBodySetAngle(to, cpBodyGetAngle(from));
		cpBodySetAngularVelocity(to, cpBodyGetAngularVelocity(from));
		cpBodySetForce(to, cpBodyGetForce(from));
		cpBodySetPosition(to, cpBodyGetPosition(from));
		cpBodySetTorque(to, cpBodyGetTorque(from));
		cpBodySetVelocity(to, cpBodyGetVelocity(from));

		to->velocity_func = from->velocity_func;
	}

	void RigidBody2D::CopyShapeData(cpShape* from, cpShape* to)
	{
		cpShapeSetElasticity(to, cpShapeGetElasticity(from));
		cpShapeSetFriction(to, cpShapeGetFriction(from));
		cpShapeSetSurfaceVelocity(to, cpShapeGetSurfaceVelocity(from));
	}
}
