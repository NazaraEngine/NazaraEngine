// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics2D/PhysConstraint2D.hpp>
#include <Nazara/Physics2D/ChipmunkHelper.hpp>
#include <chipmunk/chipmunk.h>

namespace Nz
{
	PhysConstraint2D::PhysConstraint2D(PhysWorld2D* world, cpConstraint* constraint) :
	m_constraint(constraint)
	{
		cpConstraintSetUserData(m_constraint, this);
		cpSpaceAddConstraint(world->GetHandle(), m_constraint);
	}

	PhysConstraint2D::PhysConstraint2D(PhysConstraint2D&& constraint) noexcept :
	m_constraint(std::move(constraint.m_constraint))
	{
		if (m_constraint)
			cpConstraintSetUserData(m_constraint, this);
	}

	PhysConstraint2D::~PhysConstraint2D()
	{
		Destroy();
	}

	void PhysConstraint2D::EnableBodyCollision(bool enable)
	{
		cpConstraintSetCollideBodies(m_constraint, (enable) ? cpTrue : cpFalse);
	}

	RigidBody2D& PhysConstraint2D::GetBodyA()
	{
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyA(m_constraint)));
	}

	const RigidBody2D& PhysConstraint2D::GetBodyA() const
	{
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyA(m_constraint)));
	}

	RigidBody2D& PhysConstraint2D::GetBodyB()
	{
		NazaraAssertMsg(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyB(m_constraint)));
	}

	const RigidBody2D& PhysConstraint2D::GetBodyB() const
	{
		NazaraAssertMsg(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyB(m_constraint)));
	}

	float PhysConstraint2D::GetErrorBias() const
	{
		return float(cpConstraintGetErrorBias(m_constraint));
	}

	float PhysConstraint2D::GetLastImpulse() const
	{
		return float(cpConstraintGetImpulse(m_constraint));
	}

	float PhysConstraint2D::GetMaxBias() const
	{
		return float(cpConstraintGetMaxBias(m_constraint));
	}

	float PhysConstraint2D::GetMaxForce() const
	{
		return float(cpConstraintGetMaxForce(m_constraint));
	}

	PhysWorld2D& PhysConstraint2D::GetWorld()
	{
		return *static_cast<PhysWorld2D*>(cpSpaceGetUserData(cpConstraintGetSpace(m_constraint)));
	}

	const PhysWorld2D& PhysConstraint2D::GetWorld() const
	{
		return *static_cast<PhysWorld2D*>(cpSpaceGetUserData(cpConstraintGetSpace(m_constraint)));
	}

	bool PhysConstraint2D::IsBodyCollisionEnabled() const
	{
		return cpConstraintGetCollideBodies(m_constraint) == cpTrue;
	}

	bool PhysConstraint2D::IsSingleBody() const
	{
		return cpConstraintGetBodyB(m_constraint) == cpSpaceGetStaticBody(cpConstraintGetSpace(m_constraint));
	}

	void PhysConstraint2D::SetErrorBias(float bias)
	{
		cpConstraintSetErrorBias(m_constraint, bias);
	}

	void PhysConstraint2D::SetMaxBias(float bias)
	{
		cpConstraintSetMaxBias(m_constraint, bias);
	}

	void PhysConstraint2D::SetMaxForce(float force)
	{
		cpConstraintSetMaxForce(m_constraint, force);
	}

	PhysConstraint2D& PhysConstraint2D::operator=(PhysConstraint2D&& rhs) noexcept
	{
		Destroy();

		m_constraint = std::move(rhs.m_constraint);
		if (m_constraint)
			cpConstraintSetUserData(m_constraint, this);

		return *this;
	}

	void PhysConstraint2D::Destroy()
	{
		if (m_constraint)
		{
			cpSpaceRemoveConstraint(cpConstraintGetSpace(m_constraint), m_constraint);
			cpConstraintDestroy(m_constraint);

			m_constraint = nullptr;
		}
	}


	PhysDampedSpringConstraint2D::PhysDampedSpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float restLength, float stiffness, float damping) :
	PhysConstraint2D(first.GetWorld(), cpDampedSpringNew(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor), restLength, stiffness, damping))
	{
	}

	float PhysDampedSpringConstraint2D::GetDamping() const
	{
		return float(cpDampedSpringGetDamping(m_constraint));
	}

	Vector2f PhysDampedSpringConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float PhysDampedSpringConstraint2D::GetRestLength() const
	{
		return float(cpDampedSpringGetRestLength(m_constraint));
	}

	Vector2f PhysDampedSpringConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float PhysDampedSpringConstraint2D::GetStiffness() const
	{
		return float(cpDampedSpringGetStiffness(m_constraint));
	}

	void PhysDampedSpringConstraint2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void PhysDampedSpringConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorA(m_constraint, ToChipmunk(firstAnchor));
	}

	void PhysDampedSpringConstraint2D::SetRestLength(float newLength)
	{
		cpDampedSpringSetRestLength(m_constraint, newLength);
	}

	void PhysDampedSpringConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorB(m_constraint, ToChipmunk(firstAnchor));
	}

	void PhysDampedSpringConstraint2D::SetStiffness(float newStiffness)
	{
		cpDampedSpringSetStiffness(m_constraint, newStiffness);
	}


	PhysDampedRotarySpringConstraint2D::PhysDampedRotarySpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& restAngle, float stiffness, float damping) :
	PhysConstraint2D(first.GetWorld(), cpDampedRotarySpringNew(first.GetHandle(), second.GetHandle(), restAngle.value, stiffness, damping))
	{
	}

	float PhysDampedRotarySpringConstraint2D::GetDamping() const
	{
		return float(cpDampedRotarySpringGetDamping(m_constraint));
	}

	RadianAnglef PhysDampedRotarySpringConstraint2D::GetRestAngle() const
	{
		return float(cpDampedRotarySpringGetRestAngle(m_constraint));
	}

	float PhysDampedRotarySpringConstraint2D::GetStiffness() const
	{
		return float(cpDampedRotarySpringGetStiffness(m_constraint));
	}

	void PhysDampedRotarySpringConstraint2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void PhysDampedRotarySpringConstraint2D::SetRestAngle(const RadianAnglef& newAngle)
	{
		cpDampedRotarySpringSetRestAngle(m_constraint, newAngle.value);
	}

	void PhysDampedRotarySpringConstraint2D::SetStiffness(float newStiffness)
	{
		cpDampedRotarySpringSetStiffness(m_constraint, newStiffness);
	}


	PhysGearConstraint2D::PhysGearConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratio) :
	PhysConstraint2D(first.GetWorld(), cpGearJointNew(first.GetHandle(), second.GetHandle(), phase, ratio))
	{
	}

	float PhysGearConstraint2D::GetPhase() const
	{
		return float(cpGearJointGetPhase(m_constraint));
	}

	float PhysGearConstraint2D::GetRatio() const
	{
		return float(cpGearJointGetRatio(m_constraint));
	}

	void PhysGearConstraint2D::SetPhase(float phase)
	{
		cpGearJointSetPhase(m_constraint, phase);
	}

	void PhysGearConstraint2D::SetRatio(float ratio)
	{
		cpGearJointSetRatio(m_constraint, ratio);
	}


	PhysMotorConstraint2D::PhysMotorConstraint2D(RigidBody2D& first, RigidBody2D& second, float rate) :
	PhysConstraint2D(first.GetWorld(), cpSimpleMotorNew(first.GetHandle(), second.GetHandle(), rate))
	{
	}

	float PhysMotorConstraint2D::GetRate() const
	{
		return float(cpSimpleMotorGetRate(m_constraint));
	}

	void PhysMotorConstraint2D::SetRate(float rate)
	{
		cpSimpleMotorSetRate(m_constraint, rate);
	}


	PhysPinConstraint2D::PhysPinConstraint2D(RigidBody2D& body, const Vector2f& anchor) :
	PhysConstraint2D(body.GetWorld(), cpPinJointNew(body.GetHandle(), cpSpaceGetStaticBody(body.GetWorld()->GetHandle()), ToChipmunk(body.ToLocal(anchor)), ToChipmunk(body.ToLocal(anchor))))
	{
	}

	PhysPinConstraint2D::PhysPinConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor) :
	PhysConstraint2D(first.GetWorld(), cpPinJointNew(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor)))
	{
	}

	float PhysPinConstraint2D::GetDistance() const
	{
		return float(cpPinJointGetDist(m_constraint));
	}

	Vector2f PhysPinConstraint2D::GetFirstAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyA(m_constraint), cpPinJointGetAnchorA(m_constraint)));
	}

	Vector2f PhysPinConstraint2D::GetSecondAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyB(m_constraint), cpPinJointGetAnchorB(m_constraint)));
	}

	void PhysPinConstraint2D::SetDistance(float newDistance)
	{
		cpPinJointSetDist(m_constraint, newDistance);
	}

	void PhysPinConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorA(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyA(m_constraint), ToChipmunk(firstAnchor)));
	}

	void PhysPinConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorB(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyB(m_constraint), ToChipmunk(firstAnchor)));
	}


	PhysPivotConstraint2D::PhysPivotConstraint2D(RigidBody2D& body, const Vector2f& anchor) :
	PhysConstraint2D(body.GetWorld(), cpPivotJointNew(cpSpaceGetStaticBody(body.GetWorld()->GetHandle()), body.GetHandle(), ToChipmunk(anchor)))
	{
	}

	PhysPivotConstraint2D::PhysPivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor) :
	PhysConstraint2D(first.GetWorld(), cpPivotJointNew(first.GetHandle(), second.GetHandle(), ToChipmunk(anchor)))
	{
	}

	PhysPivotConstraint2D::PhysPivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor) :
	PhysConstraint2D(first.GetWorld(), cpPivotJointNew2(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor)))
	{
	}

	Vector2f PhysPivotConstraint2D::GetFirstAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyA(m_constraint), cpPivotJointGetAnchorA(m_constraint)));
	}

	Vector2f PhysPivotConstraint2D::GetSecondAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyB(m_constraint), cpPivotJointGetAnchorB(m_constraint)));
	}

	void PhysPivotConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorA(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyA(m_constraint), ToChipmunk(firstAnchor)));
	}

	void PhysPivotConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorB(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyB(m_constraint), ToChipmunk(firstAnchor)));
	}


	PhysRatchetConstraint2D::PhysRatchetConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratchet) :
	PhysConstraint2D(first.GetWorld(), cpRatchetJointNew(first.GetHandle(), second.GetHandle(), phase, ratchet))
	{
	}

	RadianAnglef PhysRatchetConstraint2D::GetAngle() const
	{
		return float(cpRatchetJointGetAngle(m_constraint));
	}

	float PhysRatchetConstraint2D::GetPhase() const
	{
		return float(cpRatchetJointGetPhase(m_constraint));
	}

	float PhysRatchetConstraint2D::GetRatchet() const
	{
		return float(cpRatchetJointGetRatchet(m_constraint));
	}

	void PhysRatchetConstraint2D::SetAngle(const RadianAnglef& angle)
	{
		cpRatchetJointSetAngle(m_constraint, angle.value);
	}

	void PhysRatchetConstraint2D::SetPhase(float phase)
	{
		cpRatchetJointSetPhase(m_constraint, phase);
	}

	void PhysRatchetConstraint2D::SetRatchet(float ratchet)
	{
		cpRatchetJointSetRatchet(m_constraint, ratchet);
	}


	PhysRotaryLimitConstraint2D::PhysRotaryLimitConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& minAngle, const RadianAnglef& maxAngle) :
	PhysConstraint2D(first.GetWorld(), cpRotaryLimitJointNew(first.GetHandle(), second.GetHandle(), minAngle.value, maxAngle.value))
	{
	}

	RadianAnglef PhysRotaryLimitConstraint2D::GetMaxAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	RadianAnglef PhysRotaryLimitConstraint2D::GetMinAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	void PhysRotaryLimitConstraint2D::SetMaxAngle(const RadianAnglef& maxAngle)
	{
		cpRotaryLimitJointSetMax(m_constraint, maxAngle.value);
	}

	void PhysRotaryLimitConstraint2D::SetMinAngle(const RadianAnglef& minAngle)
	{
		cpRotaryLimitJointSetMin(m_constraint, minAngle.value);
	}


	PhysSlideConstraint2D::PhysSlideConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float min, float max) :
	PhysConstraint2D(first.GetWorld(), cpSlideJointNew(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor), min, max))
	{
	}

	Vector2f PhysSlideConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float PhysSlideConstraint2D::GetMaxDistance() const
	{
		return float(cpSlideJointGetMax(m_constraint));
	}

	float PhysSlideConstraint2D::GetMinDistance() const
	{
		return float(cpSlideJointGetMin(m_constraint));
	}

	Vector2f PhysSlideConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void PhysSlideConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorA(m_constraint, ToChipmunk(firstAnchor));
	}

	void PhysSlideConstraint2D::SetMaxDistance(float newMaxDistance)
	{
		cpSlideJointSetMax(m_constraint, newMaxDistance);
	}

	void PhysSlideConstraint2D::SetMinDistance(float newMinDistance)
	{
		cpSlideJointSetMin(m_constraint, newMinDistance);
	}

	void PhysSlideConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorB(m_constraint, ToChipmunk(firstAnchor));
	}
}
