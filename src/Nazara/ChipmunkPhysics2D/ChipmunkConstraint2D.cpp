// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/ChipmunkConstraint2D.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkHelper.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	ChipmunkConstraint2D::ChipmunkConstraint2D(ChipmunkPhysWorld2D* world, cpConstraint* constraint) :
	m_constraint(constraint)
	{
		cpConstraintSetUserData(m_constraint, this);
		cpSpaceAddConstraint(world->GetHandle(), m_constraint);
	}

	ChipmunkConstraint2D::ChipmunkConstraint2D(ChipmunkConstraint2D&& constraint) noexcept :
	m_constraint(std::move(constraint.m_constraint))
	{
		if (m_constraint)
			cpConstraintSetUserData(m_constraint, this);
	}

	ChipmunkConstraint2D::~ChipmunkConstraint2D()
	{
		Destroy();
	}

	void ChipmunkConstraint2D::EnableBodyCollision(bool enable)
	{
		cpConstraintSetCollideBodies(m_constraint, (enable) ? cpTrue : cpFalse);
	}

	ChipmunkRigidBody2D& ChipmunkConstraint2D::GetBodyA()
	{
		return *static_cast<ChipmunkRigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyA(m_constraint)));
	}

	const ChipmunkRigidBody2D& ChipmunkConstraint2D::GetBodyA() const
	{
		return *static_cast<ChipmunkRigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyA(m_constraint)));
	}

	ChipmunkRigidBody2D& ChipmunkConstraint2D::GetBodyB()
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<ChipmunkRigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyB(m_constraint)));
	}

	const ChipmunkRigidBody2D& ChipmunkConstraint2D::GetBodyB() const
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<ChipmunkRigidBody2D*>(cpBodyGetUserData(cpConstraintGetBodyB(m_constraint)));
	}

	float ChipmunkConstraint2D::GetErrorBias() const
	{
		return float(cpConstraintGetErrorBias(m_constraint));
	}

	float ChipmunkConstraint2D::GetLastImpulse() const
	{
		return float(cpConstraintGetImpulse(m_constraint));
	}

	float ChipmunkConstraint2D::GetMaxBias() const
	{
		return float(cpConstraintGetMaxBias(m_constraint));
	}

	float ChipmunkConstraint2D::GetMaxForce() const
	{
		return float(cpConstraintGetMaxForce(m_constraint));
	}

	ChipmunkPhysWorld2D& ChipmunkConstraint2D::GetWorld()
	{
		return *static_cast<ChipmunkPhysWorld2D*>(cpSpaceGetUserData(cpConstraintGetSpace(m_constraint)));
	}

	const ChipmunkPhysWorld2D& ChipmunkConstraint2D::GetWorld() const
	{
		return *static_cast<ChipmunkPhysWorld2D*>(cpSpaceGetUserData(cpConstraintGetSpace(m_constraint)));
	}

	bool ChipmunkConstraint2D::IsBodyCollisionEnabled() const
	{
		return cpConstraintGetCollideBodies(m_constraint) == cpTrue;
	}

	bool ChipmunkConstraint2D::IsSingleBody() const
	{
		return cpConstraintGetBodyB(m_constraint) == cpSpaceGetStaticBody(cpConstraintGetSpace(m_constraint));
	}

	void ChipmunkConstraint2D::SetErrorBias(float bias)
	{
		cpConstraintSetErrorBias(m_constraint, bias);
	}

	void ChipmunkConstraint2D::SetMaxBias(float bias)
	{
		cpConstraintSetMaxBias(m_constraint, bias);
	}

	void ChipmunkConstraint2D::SetMaxForce(float force)
	{
		cpConstraintSetMaxForce(m_constraint, force);
	}

	ChipmunkConstraint2D& ChipmunkConstraint2D::operator=(ChipmunkConstraint2D&& rhs) noexcept
	{
		Destroy();

		m_constraint = std::move(rhs.m_constraint);
		if (m_constraint)
			cpConstraintSetUserData(m_constraint, this);

		return *this;
	}

	void ChipmunkConstraint2D::Destroy()
	{
		if (m_constraint)
		{
			cpSpaceRemoveConstraint(cpConstraintGetSpace(m_constraint), m_constraint);
			cpConstraintDestroy(m_constraint);

			m_constraint = nullptr;
		}
	}


	ChipmunkDampedSpringConstraint2D::ChipmunkDampedSpringConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float restLength, float stiffness, float damping) :
	ChipmunkConstraint2D(first.GetWorld(), cpDampedSpringNew(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor), restLength, stiffness, damping))
	{
	}

	float ChipmunkDampedSpringConstraint2D::GetDamping() const
	{
		return float(cpDampedSpringGetDamping(m_constraint));
	}

	Vector2f ChipmunkDampedSpringConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float ChipmunkDampedSpringConstraint2D::GetRestLength() const
	{
		return float(cpDampedSpringGetRestLength(m_constraint));
	}

	Vector2f ChipmunkDampedSpringConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpDampedSpringGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float ChipmunkDampedSpringConstraint2D::GetStiffness() const
	{
		return float(cpDampedSpringGetStiffness(m_constraint));
	}

	void ChipmunkDampedSpringConstraint2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void ChipmunkDampedSpringConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorA(m_constraint, ToChipmunk(firstAnchor));
	}

	void ChipmunkDampedSpringConstraint2D::SetRestLength(float newLength)
	{
		cpDampedSpringSetRestLength(m_constraint, newLength);
	}

	void ChipmunkDampedSpringConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpDampedSpringSetAnchorB(m_constraint, ToChipmunk(firstAnchor));
	}

	void ChipmunkDampedSpringConstraint2D::SetStiffness(float newStiffness)
	{
		cpDampedSpringSetStiffness(m_constraint, newStiffness);
	}

	
	ChipmunkDampedRotarySpringConstraint2D::ChipmunkDampedRotarySpringConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const RadianAnglef& restAngle, float stiffness, float damping) :
	ChipmunkConstraint2D(first.GetWorld(), cpDampedRotarySpringNew(first.GetHandle(), second.GetHandle(), restAngle.value, stiffness, damping))
	{
	}

	float ChipmunkDampedRotarySpringConstraint2D::GetDamping() const
	{
		return float(cpDampedRotarySpringGetDamping(m_constraint));
	}

	RadianAnglef ChipmunkDampedRotarySpringConstraint2D::GetRestAngle() const
	{
		return float(cpDampedRotarySpringGetRestAngle(m_constraint));
	}

	float ChipmunkDampedRotarySpringConstraint2D::GetStiffness() const
	{
		return float(cpDampedRotarySpringGetStiffness(m_constraint));
	}

	void ChipmunkDampedRotarySpringConstraint2D::SetDamping(float newDamping)
	{
		cpDampedSpringSetDamping(m_constraint, newDamping);
	}

	void ChipmunkDampedRotarySpringConstraint2D::SetRestAngle(const RadianAnglef& newAngle)
	{
		cpDampedRotarySpringSetRestAngle(m_constraint, newAngle.value);
	}

	void ChipmunkDampedRotarySpringConstraint2D::SetStiffness(float newStiffness)
	{
		cpDampedRotarySpringSetStiffness(m_constraint, newStiffness);
	}


	ChipmunkGearConstraint2D::ChipmunkGearConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, float phase, float ratio) :
	ChipmunkConstraint2D(first.GetWorld(), cpGearJointNew(first.GetHandle(), second.GetHandle(), phase, ratio))
	{
	}

	float ChipmunkGearConstraint2D::GetPhase() const
	{
		return float(cpGearJointGetPhase(m_constraint));
	}

	float ChipmunkGearConstraint2D::GetRatio() const
	{
		return float(cpGearJointGetRatio(m_constraint));
	}

	void ChipmunkGearConstraint2D::SetPhase(float phase)
	{
		cpGearJointSetPhase(m_constraint, phase);
	}

	void ChipmunkGearConstraint2D::SetRatio(float ratio)
	{
		cpGearJointSetRatio(m_constraint, ratio);
	}


	ChipmunkMotorConstraint2D::ChipmunkMotorConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, float rate) :
	ChipmunkConstraint2D(first.GetWorld(), cpSimpleMotorNew(first.GetHandle(), second.GetHandle(), rate))
	{
	}

	float ChipmunkMotorConstraint2D::GetRate() const
	{
		return float(cpSimpleMotorGetRate(m_constraint));
	}

	void ChipmunkMotorConstraint2D::SetRate(float rate)
	{
		cpSimpleMotorSetRate(m_constraint, rate);
	}


	ChipmunkPinConstraint2D::ChipmunkPinConstraint2D(ChipmunkRigidBody2D& body, const Vector2f& anchor) :
	ChipmunkConstraint2D(body.GetWorld(), cpPinJointNew(body.GetHandle(), cpSpaceGetStaticBody(body.GetWorld()->GetHandle()), ToChipmunk(body.ToLocal(anchor)), ToChipmunk(body.ToLocal(anchor))))
	{
	}

	ChipmunkPinConstraint2D::ChipmunkPinConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor) :
	ChipmunkConstraint2D(first.GetWorld(), cpPinJointNew(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor)))
	{
	}

	float ChipmunkPinConstraint2D::GetDistance() const
	{
		return float(cpPinJointGetDist(m_constraint));
	}

	Vector2f ChipmunkPinConstraint2D::GetFirstAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyA(m_constraint), cpPinJointGetAnchorA(m_constraint)));
	}

	Vector2f ChipmunkPinConstraint2D::GetSecondAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyB(m_constraint), cpPinJointGetAnchorB(m_constraint)));
	}

	void ChipmunkPinConstraint2D::SetDistance(float newDistance)
	{
		cpPinJointSetDist(m_constraint, newDistance);
	}

	void ChipmunkPinConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorA(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyA(m_constraint), ToChipmunk(firstAnchor)));
	}

	void ChipmunkPinConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPinJointSetAnchorB(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyB(m_constraint), ToChipmunk(firstAnchor)));
	}


	ChipmunkPivotConstraint2D::ChipmunkPivotConstraint2D(ChipmunkRigidBody2D& body, const Vector2f& anchor) :
	ChipmunkConstraint2D(body.GetWorld(), cpPivotJointNew(cpSpaceGetStaticBody(body.GetWorld()->GetHandle()), body.GetHandle(), ToChipmunk(anchor)))
	{
	}

	ChipmunkPivotConstraint2D::ChipmunkPivotConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& anchor) :
	ChipmunkConstraint2D(first.GetWorld(), cpPivotJointNew(first.GetHandle(), second.GetHandle(), ToChipmunk(anchor)))
	{
	}

	ChipmunkPivotConstraint2D::ChipmunkPivotConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor) :
	ChipmunkConstraint2D(first.GetWorld(), cpPivotJointNew2(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor)))
	{
	}

	Vector2f ChipmunkPivotConstraint2D::GetFirstAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyA(m_constraint), cpPivotJointGetAnchorA(m_constraint)));
	}

	Vector2f ChipmunkPivotConstraint2D::GetSecondAnchor() const
	{
		return FromChipmunk(cpBodyLocalToWorld(cpConstraintGetBodyB(m_constraint), cpPivotJointGetAnchorB(m_constraint)));
	}

	void ChipmunkPivotConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorA(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyA(m_constraint), ToChipmunk(firstAnchor)));
	}

	void ChipmunkPivotConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpPivotJointSetAnchorB(m_constraint, cpBodyWorldToLocal(cpConstraintGetBodyB(m_constraint), ToChipmunk(firstAnchor)));
	}


	ChipmunkRatchetConstraint2D::ChipmunkRatchetConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, float phase, float ratchet) :
	ChipmunkConstraint2D(first.GetWorld(), cpRatchetJointNew(first.GetHandle(), second.GetHandle(), phase, ratchet))
	{
	}

	RadianAnglef ChipmunkRatchetConstraint2D::GetAngle() const
	{
		return float(cpRatchetJointGetAngle(m_constraint));
	}

	float ChipmunkRatchetConstraint2D::GetPhase() const
	{
		return float(cpRatchetJointGetPhase(m_constraint));
	}

	float ChipmunkRatchetConstraint2D::GetRatchet() const
	{
		return float(cpRatchetJointGetRatchet(m_constraint));
	}

	void ChipmunkRatchetConstraint2D::SetAngle(const RadianAnglef& angle)
	{
		cpRatchetJointSetAngle(m_constraint, angle.value);
	}

	void ChipmunkRatchetConstraint2D::SetPhase(float phase)
	{
		cpRatchetJointSetPhase(m_constraint, phase);
	}

	void ChipmunkRatchetConstraint2D::SetRatchet(float ratchet)
	{
		cpRatchetJointSetRatchet(m_constraint, ratchet);
	}


	ChipmunkRotaryLimitConstraint2D::ChipmunkRotaryLimitConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const RadianAnglef& minAngle, const RadianAnglef& maxAngle) :
	ChipmunkConstraint2D(first.GetWorld(), cpRotaryLimitJointNew(first.GetHandle(), second.GetHandle(), minAngle.value, maxAngle.value))
	{
	}

	RadianAnglef ChipmunkRotaryLimitConstraint2D::GetMaxAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	RadianAnglef ChipmunkRotaryLimitConstraint2D::GetMinAngle() const
	{
		return float(cpRotaryLimitJointGetMax(m_constraint));
	}

	void ChipmunkRotaryLimitConstraint2D::SetMaxAngle(const RadianAnglef& maxAngle)
	{
		cpRotaryLimitJointSetMax(m_constraint, maxAngle.value);
	}

	void ChipmunkRotaryLimitConstraint2D::SetMinAngle(const RadianAnglef& minAngle)
	{
		cpRotaryLimitJointSetMin(m_constraint, minAngle.value);
	}


	ChipmunkSlideConstraint2D::ChipmunkSlideConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float min, float max) :
	ChipmunkConstraint2D(first.GetWorld(), cpSlideJointNew(first.GetHandle(), second.GetHandle(), ToChipmunk(firstAnchor), ToChipmunk(secondAnchor), min, max))
	{
	}

	Vector2f ChipmunkSlideConstraint2D::GetFirstAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorA(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	float ChipmunkSlideConstraint2D::GetMaxDistance() const
	{
		return float(cpSlideJointGetMax(m_constraint));
	}

	float ChipmunkSlideConstraint2D::GetMinDistance() const
	{
		return float(cpSlideJointGetMin(m_constraint));
	}

	Vector2f ChipmunkSlideConstraint2D::GetSecondAnchor() const
	{
		cpVect anchor = cpSlideJointGetAnchorB(m_constraint);
		return Vector2f(static_cast<float>(anchor.x), static_cast<float>(anchor.y));
	}

	void ChipmunkSlideConstraint2D::SetFirstAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorA(m_constraint, ToChipmunk(firstAnchor));
	}

	void ChipmunkSlideConstraint2D::SetMaxDistance(float newMaxDistance)
	{
		cpSlideJointSetMax(m_constraint, newMaxDistance);
	}

	void ChipmunkSlideConstraint2D::SetMinDistance(float newMinDistance)
	{
		cpSlideJointSetMin(m_constraint, newMinDistance);
	}

	void ChipmunkSlideConstraint2D::SetSecondAnchor(const Vector2f& firstAnchor)
	{
		cpSlideJointSetAnchorB(m_constraint, ToChipmunk(firstAnchor));
	}
}
