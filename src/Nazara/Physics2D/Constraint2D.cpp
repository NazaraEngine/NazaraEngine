// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Constraint2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	Constraint2D::Constraint2D(Nz::PhysWorld2D* world, cpConstraint* constraint) :
	m_constraint(constraint)
	{
		cpConstraintSetUserData(m_constraint, this);
		cpSpaceAddConstraint(world->GetHandle(), m_constraint);
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


	DampedSpringConstraint2D::DampedSpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float restLength, float stiffness, float damping) :
	Constraint2D(first.GetWorld(), cpDampedSpringNew(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y), restLength, stiffness, damping))
	{
	}

	float DampedSpringConstraint2D::GetDamping() const
	{
		return float(cpDampedSpringGetDamping(m_constraint));
	}

	Vector2f DampedSpringConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float DampedSpringConstraint2D::GetRestLength() const
	{
		return float(cpDampedSpringGetRestLength(m_constraint));
	}

	Vector2f DampedSpringConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float DampedSpringConstraint2D::GetStiffness() const
	{
		return float(cpDampedSpringGetStiffness(m_constraint));
	}

	void DampedSpringConstraint2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void DampedSpringConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void DampedSpringConstraint2D::SetRestLength(float newLength)
	{
		cpDampedSpringSetRestLength(m_constraint, newLength);
	}

	void DampedSpringConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void DampedSpringConstraint2D::SetStiffness(float newStiffness)
	{
		cpDampedSpringSetStiffness(m_constraint, newStiffness);
	}

	
	DampedRotarySpringConstraint2D::DampedRotarySpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& restAngle, float stiffness, float damping) :
	Constraint2D(first.GetWorld(), cpDampedRotarySpringNew(first.GetHandle(), second.GetHandle(), restAngle.value, stiffness, damping))
	{
	}

	float DampedRotarySpringConstraint2D::GetDamping() const
	{
		return float(cpDampedRotarySpringGetDamping(m_constraint));
	}

	RadianAnglef DampedRotarySpringConstraint2D::GetRestAngle() const
	{
		return float(cpDampedRotarySpringGetRestAngle(m_constraint));
	}

	float DampedRotarySpringConstraint2D::GetStiffness() const
	{
		return float(cpDampedRotarySpringGetStiffness(m_constraint));
	}

	void DampedRotarySpringConstraint2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void DampedRotarySpringConstraint2D::SetRestAngle(const RadianAnglef& newAngle)
	{
		cpDampedRotarySpringSetRestAngle(m_constraint, newAngle.value);
	}

	void DampedRotarySpringConstraint2D::SetStiffness(float newStiffness)
	{
		cpDampedRotarySpringSetStiffness(m_constraint, newStiffness);
	}


	GearConstraint2D::GearConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratio) :
	Constraint2D(first.GetWorld(), cpGearJointNew(first.GetHandle(), second.GetHandle(), phase, ratio))
	{
	}

	float GearConstraint2D::GetPhase() const
	{
		return float(cpGearJointGetPhase(m_constraint));
	}

	float GearConstraint2D::GetRatio() const
	{
		return float(cpGearJointGetRatio(m_constraint));
	}

	void GearConstraint2D::SetPhase(float phase)
	{
		cpGearJointSetPhase(m_constraint, phase);
	}

	void GearConstraint2D::SetRatio(float ratio)
	{
		cpGearJointSetRatio(m_constraint, ratio);
	}


	MotorConstraint2D::MotorConstraint2D(RigidBody2D& first, RigidBody2D& second, float rate) :
	Constraint2D(first.GetWorld(), cpSimpleMotorNew(first.GetHandle(), second.GetHandle(), rate))
	{
	}

	float MotorConstraint2D::GetRate() const
	{
		return float(cpSimpleMotorGetRate(m_constraint));
	}

	void MotorConstraint2D::SetRate(float rate)
	{
		cpSimpleMotorSetRate(m_constraint, rate);
	}


	PinConstraint2D::PinConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor) :
	Constraint2D(first.GetWorld(), cpPinJointNew(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y)))
	{
	}

	float PinConstraint2D::GetDistance() const
	{
		return float(cpPinJointGetDist(m_constraint));
	}

	Vector2f PinConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpPinJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	Vector2f PinConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpPinJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void PinConstraint2D::SetDistance(float newDistance)
	{
		cpPinJointSetDist(m_constraint, newDistance);
	}

	void PinConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void PinConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}


	PivotConstraint2D::PivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor) :
	Constraint2D(first.GetWorld(), cpPivotJointNew(first.GetHandle(), second.GetHandle(), cpv(anchor.x, anchor.y)))
	{
	}

	PivotConstraint2D::PivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor) :
	Constraint2D(first.GetWorld(), cpPivotJointNew2(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y)))
	{
	}

	Vector2f PivotConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpPivotJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	Vector2f PivotConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpPivotJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void PivotConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void PivotConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}


	RatchetConstraint2D::RatchetConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratchet) :
	Constraint2D(first.GetWorld(), cpRatchetJointNew(first.GetHandle(), second.GetHandle(), phase, ratchet))
	{
	}

	RadianAnglef RatchetConstraint2D::GetAngle() const
	{
		return float(cpRatchetJointGetAngle(m_constraint));
	}

	float RatchetConstraint2D::GetPhase() const
	{
		return float(cpRatchetJointGetPhase(m_constraint));
	}

	float RatchetConstraint2D::GetRatchet() const
	{
		return float(cpRatchetJointGetRatchet(m_constraint));
	}

	void RatchetConstraint2D::SetAngle(const RadianAnglef& angle)
	{
		cpRatchetJointSetAngle(m_constraint, angle.value);
	}

	void RatchetConstraint2D::SetPhase(float phase)
	{
		cpRatchetJointSetPhase(m_constraint, phase);
	}

	void RatchetConstraint2D::SetRatchet(float ratchet)
	{
		cpRatchetJointSetRatchet(m_constraint, ratchet);
	}


	RotaryLimitConstraint2D::RotaryLimitConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& minAngle, const RadianAnglef& maxAngle) :
	Constraint2D(first.GetWorld(), cpRotaryLimitJointNew(first.GetHandle(), second.GetHandle(), minAngle.value, maxAngle.value))
	{
	}

	RadianAnglef RotaryLimitConstraint2D::GetMaxAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	RadianAnglef RotaryLimitConstraint2D::GetMinAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	void RotaryLimitConstraint2D::SetMaxAngle(const RadianAnglef& maxAngle)
	{
		cpRotaryLimitJointSetMax(m_constraint, maxAngle.value);
	}

	void RotaryLimitConstraint2D::SetMinAngle(const RadianAnglef& minAngle)
	{
		cpRotaryLimitJointSetMin(m_constraint, minAngle.value);
	}


	SlideConstraint2D::SlideConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float min, float max) :
	Constraint2D(first.GetWorld(), cpSlideJointNew(first.GetHandle(), second.GetHandle(), cpv(firstAnchor.x, firstAnchor.y), cpv(secondAnchor.x, secondAnchor.y), min, max))
	{
	}

	Vector2f SlideConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float SlideConstraint2D::GetMaxDistance() const
	{
		return float(cpSlideJointGetMax(m_constraint));
	}

	float SlideConstraint2D::GetMinDistance() const
	{
		return float(cpSlideJointGetMin(m_constraint));
	}

	Vector2f SlideConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void SlideConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorA(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}

	void SlideConstraint2D::SetMaxDistance(float newMaxDistance)
	{
		cpSlideJointSetMax(m_constraint, newMaxDistance);
	}

	void SlideConstraint2D::SetMinDistance(float newMinDistance)
	{
		cpSlideJointSetMin(m_constraint, newMinDistance);
	}

	void SlideConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorB(m_constraint, cpv(firstAnchor.x, firstAnchor.y));
	}
}
