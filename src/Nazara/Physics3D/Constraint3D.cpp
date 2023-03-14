// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Constraint3D.hpp>
#include <Nazara/Physics3D/BulletHelper.hpp>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Constraint3D::Constraint3D(std::unique_ptr<btTypedConstraint> constraint, bool disableCollisions) :
	m_constraint(std::move(constraint)),
	m_bodyCollisionEnabled(!disableCollisions)
	{
		btDynamicsWorld* world = GetWorld().GetDynamicsWorld();
		world->addConstraint(m_constraint.get(), disableCollisions);
	}

	Constraint3D::Constraint3D(Constraint3D&& constraint) noexcept :
	m_constraint(std::move(constraint.m_constraint)),
	m_bodyCollisionEnabled(constraint.m_bodyCollisionEnabled)
	{
		if (m_constraint)
			m_constraint->setUserConstraintPtr(this);
	}

	Constraint3D::~Constraint3D()
	{
		if (m_constraint)
		{
			btDynamicsWorld* world = GetWorld().GetDynamicsWorld();
			world->removeConstraint(m_constraint.get());
		}
	}

	RigidBody3D& Constraint3D::GetBodyA()
	{
		return *static_cast<RigidBody3D*>(m_constraint->getRigidBodyA().getUserPointer());
	}

	const RigidBody3D& Constraint3D::GetBodyA() const
	{
		return *static_cast<RigidBody3D*>(m_constraint->getRigidBodyA().getUserPointer());
	}

	RigidBody3D& Constraint3D::GetBodyB()
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<RigidBody3D*>(m_constraint->getRigidBodyB().getUserPointer());
	}

	const RigidBody3D& Constraint3D::GetBodyB() const
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<RigidBody3D*>(m_constraint->getRigidBodyB().getUserPointer());
	}

	PhysWorld3D& Constraint3D::GetWorld()
	{
		return *GetBodyA().GetWorld();
	}

	const PhysWorld3D& Constraint3D::GetWorld() const
	{
		return *GetBodyA().GetWorld();
	}

	bool Constraint3D::IsSingleBody() const
	{
		return &m_constraint->getRigidBodyB() == &btTypedConstraint::getFixedBody();
	}

	Constraint3D& Constraint3D::operator=(Constraint3D&& constraint) noexcept
	{
		m_constraint.reset();

		m_constraint = std::move(constraint.m_constraint);
		m_bodyCollisionEnabled = constraint.m_bodyCollisionEnabled;

		if (m_constraint)
			m_constraint->setUserConstraintPtr(this);

		return *this;
	}


	PivotConstraint3D::PivotConstraint3D(RigidBody3D& first, const Vector3f& pivot) :
	Constraint3D(std::make_unique<btPoint2PointConstraint>(*first.GetRigidBody(), ToBullet(first.ToLocal(pivot))))
	{
	}

	PivotConstraint3D::PivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& pivot, bool disableCollisions) :
	Constraint3D(std::make_unique<btPoint2PointConstraint>(*first.GetRigidBody(), *second.GetRigidBody(), ToBullet(first.ToLocal(pivot)), ToBullet(second.ToLocal(pivot))), disableCollisions)
	{
	}

	PivotConstraint3D::PivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, bool disableCollisions) :
	Constraint3D(std::make_unique<btPoint2PointConstraint>(*first.GetRigidBody(), *second.GetRigidBody(), ToBullet(firstAnchor), ToBullet(secondAnchor)), disableCollisions)
	{
	}

	Vector3f PivotConstraint3D::GetFirstAnchor() const
	{
		return FromBullet(GetConstraint<btPoint2PointConstraint>()->getPivotInA());
	}

	Vector3f PivotConstraint3D::GetSecondAnchor() const
	{
		return FromBullet(GetConstraint<btPoint2PointConstraint>()->getPivotInB());
	}

	void PivotConstraint3D::SetFirstAnchor(const Vector3f& firstAnchor)
	{
		GetConstraint<btPoint2PointConstraint>()->setPivotA(ToBullet(firstAnchor));
	}

	void PivotConstraint3D::SetSecondAnchor(const Vector3f& secondAnchor)
	{
		GetConstraint<btPoint2PointConstraint>()->setPivotB(ToBullet(secondAnchor));
	}
}
