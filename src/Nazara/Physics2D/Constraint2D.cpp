// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Constraint2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	Constraint2D::Constraint2D(PhysWorld2D& world, cpConstraint* constraint) :
	m_constraint(constraint)
	{
		cpConstraintSetUserData(m_constraint, this);
		cpSpaceAddConstraint(world.GetHandle(), m_constraint);
	}

	Constraint2D::Constraint2D(Constraint2D&& rhs) :
	m_constraint(std::move(rhs.m_constraint))
	{
		cpConstraintSetUserData(m_constraint, this);
	}

	Constraint2D::~Constraint2D()
	{
		cpSpaceRemoveConstraint(cpConstraintGetSpace(m_constraint), m_constraint);
	}

	void Constraint2D::EnableBodyCollision(bool enable)
	{
		cpConstraintSetCollideBodies(m_constraint, (enable) ? cpTrue : cpFalse);
	}

	RigidBody2D& Constraint2D::GetBodyA()
	{
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyA(m_constraint)));
	}

	const RigidBody2D& Constraint2D::GetBodyA() const
	{
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyA(m_constraint)));
	}

	RigidBody2D& Constraint2D::GetBodyB()
	{
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyB(m_constraint)));
	}

	const RigidBody2D& Constraint2D::GetBodyB() const
	{
		return *static_cast<RigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyB(m_constraint)));
	}

	float Constraint2D::GetErrorBias() const
	{
		return float(cpConstraintGetErrorBias(m_constraint));
	}

	float Constraint2D::GetMaxBias() const
	{
		return float(cpConstraintGetMaxBias(m_constraint));
	}

	float Constraint2D::GetMaxForce() const
	{
		return float(cpConstraintGetMaxForce(m_constraint));
	}

	PhysWorld2D& Constraint2D::GetWorld()
	{
		return *static_cast<PhysWorld2D*>(cpSpaceGetUserData(cpConstraintGetSpace(m_constraint)));
	}

	const PhysWorld2D& Constraint2D::GetWorld() const
	{
		return *static_cast<PhysWorld2D*>(cpSpaceGetUserData(cpConstraintGetSpace(m_constraint)));
	}

	bool Constraint2D::IsBodyCollisionEnabled() const
	{
		return cpConstraintGetCollideBodies(m_constraint) == cpTrue;
	}

	void Constraint2D::SetErrorBias(float bias)
	{
		cpConstraintSetErrorBias(m_constraint, bias);
	}

	void Constraint2D::SetMaxBias(float bias)
	{
		cpConstraintSetMaxBias(m_constraint, bias);
	}

	void Constraint2D::SetMaxForce(float force)
	{
		cpConstraintSetMaxForce(m_constraint, force);
	}

	Constraint2D& Constraint2D::operator=(Constraint2D && rhs)
	{
		m_constraint = std::move(rhs.m_constraint);
		cpConstraintSetUserData(m_constraint, this);

		return *this;
	}


	DampedSpring2D::DampedSpring2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor, float restLength, float stiffness, float damping) :
	Constraint2D(world, cpDampedSpringNew(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y), restLength, stiffness, damping))
	{
	}

	float DampedSpring2D::GetDamping() const
	{
		return float(cpDampedSpringGetDamping(m_constraint));
	}

	Vector2f DampedSpring2D::GetFirstAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float DampedSpring2D::GetRestLength() const
	{
		return float(cpDampedSpringGetRestLength(m_constraint));
	}

	Vector2f DampedSpring2D::GetSecondAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float DampedSpring2D::GetStiffness() const
	{
		return float(cpDampedSpringGetStiffness(m_constraint));
	}

	void DampedSpring2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void DampedSpring2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void DampedSpring2D::SetRestLength(float newLength)
	{
		cpDampedSpringSetRestLength(m_constraint, newLength);
	}

	void DampedSpring2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void DampedSpring2D::SetStiffness(float newStiffness)
	{
		cpDampedSpringSetStiffness(m_constraint, newStiffness);
	}

	
	DampedRotarySpring2D::DampedRotarySpring2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float restAngle, float stiffness, float damping) :
	Constraint2D(world, cpDampedRotarySpringNew(first.GetHandle(), second.GetHandle(), restAngle, stiffness, damping))
	{
	}

	float DampedRotarySpring2D::GetDamping() const
	{
		return float(cpDampedRotarySpringGetDamping(m_constraint));
	}

	float DampedRotarySpring2D::GetRestAngle() const
	{
		return float(cpDampedRotarySpringGetRestAngle(m_constraint));
	}

	float DampedRotarySpring2D::GetStiffness() const
	{
		return float(cpDampedRotarySpringGetStiffness(m_constraint));
	}

	void DampedRotarySpring2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void DampedRotarySpring2D::SetRestAngle(float newAngle)
	{
		cpDampedRotarySpringSetRestAngle(m_constraint, newAngle);
	}

	void DampedRotarySpring2D::SetStiffness(float newStiffness)
	{
		cpDampedRotarySpringSetStiffness(m_constraint, newStiffness);
	}


	GearJoint2D::GearJoint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float phase, float ratio) :
	Constraint2D(world, cpGearJointNew(first.GetHandle(), second.GetHandle(), phase, ratio))
	{
	}

	float GearJoint2D::GetPhase() const
	{
		return float(cpGearJointGetPhase(m_constraint));
	}

	float GearJoint2D::GetRatio() const
	{
		return float(cpGearJointGetRatio(m_constraint));
	}

	void GearJoint2D::SetPhase(float phase)
	{
		cpGearJointSetPhase(m_constraint, phase);
	}

	void GearJoint2D::SetRatio(float ratio)
	{
		cpGearJointSetRatio(m_constraint, ratio);
	}


	MotorJoint2D::MotorJoint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float rate) :
	Constraint2D(world, cpSimpleMotorNew(first.GetHandle(), second.GetHandle(), rate))
	{
	}

	float MotorJoint2D::GetRate() const
	{
		return float(cpSimpleMotorGetRate(m_constraint));
	}

	void MotorJoint2D::SetRate(float rate)
	{
		cpSimpleMotorSetRate(m_constraint, rate);
	}


	PinJoint2D::PinJoint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor) :
	Constraint2D(world, cpPinJointNew(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y)))
	{
	}

	float PinJoint2D::GetDistance() const
	{
		return float(cpPinJointGetDist(m_constraint));
	}

	Vector2f PinJoint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpPinJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	Vector2f PinJoint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpPinJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void PinJoint2D::SetDistance(float newDistance)
	{
		cpPinJointSetDist(m_constraint, newDistance);
	}

	void PinJoint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void PinJoint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}


	PivotJoint2D::PivotJoint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor) :
	Constraint2D(world, cpPivotJointNew(first.GetHandle(), second.GetHandle(), cpv(anchor.x, anchor.y)))
	{
	}

	PivotJoint2D::PivotJoint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor) :
	Constraint2D(world, cpPivotJointNew2(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y)))
	{
	}

	Vector2f PivotJoint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpPivotJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	Vector2f PivotJoint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpPivotJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void PivotJoint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void PivotJoint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}


	RatchetJoint2D::RatchetJoint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float phase, float ratchet) :
	Constraint2D(world, cpRatchetJointNew(first.GetHandle(), second.GetHandle(), phase, ratchet))
	{
	}

	float RatchetJoint2D::GetAngle() const
	{
		return float(cpRatchetJointGetAngle(m_constraint));
	}

	float RatchetJoint2D::GetPhase() const
	{
		return float(cpRatchetJointGetPhase(m_constraint));
	}

	float RatchetJoint2D::GetRatchet() const
	{
		return float(cpRatchetJointGetRatchet(m_constraint));
	}

	void RatchetJoint2D::SetAngle(float angle)
	{
		cpRatchetJointSetAngle(m_constraint, angle);
	}

	void RatchetJoint2D::SetPhase(float phase)
	{
		cpRatchetJointSetPhase(m_constraint, phase);
	}

	void RatchetJoint2D::SetRatchet(float ratchet)
	{
		cpRatchetJointSetRatchet(m_constraint, ratchet);
	}


	RotaryLimitJoint2D::RotaryLimitJoint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float minAngle, float maxAngle) :
	Constraint2D(world, cpRotaryLimitJointNew(first.GetHandle(), second.GetHandle(), minAngle, maxAngle))
	{
	}

	float RotaryLimitJoint2D::GetMaxAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	float RotaryLimitJoint2D::GetMinAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	void RotaryLimitJoint2D::SetMaxAngle(float maxAngle)
	{
		cpRotaryLimitJointSetMax(m_constraint, maxAngle);
	}

	void RotaryLimitJoint2D::SetMinAngle(float minAngle)
	{
		cpRotaryLimitJointSetMin(m_constraint, minAngle);
	}


	SlideJoint2D::SlideJoint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor, float min, float max) :
	Constraint2D(world, cpSlideJointNew(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y), min, max))
	{
	}

	Vector2f SlideJoint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float SlideJoint2D::GetMaxDistance() const
	{
		return float(cpSlideJointGetMax(m_constraint));
	}

	float SlideJoint2D::GetMinDistance() const
	{
		return float(cpSlideJointGetMin(m_constraint));
	}

	Vector2f SlideJoint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void SlideJoint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void SlideJoint2D::SetMaxDistance(float newMaxDistance)
	{
		cpSlideJointSetMax(m_constraint, newMaxDistance);
	}

	void SlideJoint2D::SetMinDistance(float newMinDistance)
	{
		cpSlideJointSetMin(m_constraint, newMinDistance);
	}

	void SlideJoint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

}
