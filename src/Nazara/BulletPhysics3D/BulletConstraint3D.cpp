// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/BulletConstraint3D.hpp>
#include <Nazara/BulletPhysics3D/BulletHelper.hpp>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	BulletConstraint3D::BulletConstraint3D(std::unique_ptr<btTypedConstraint> constraint, bool disableCollisions) :
	m_constraint(std::move(constraint)),
	m_bodyCollisionEnabled(!disableCollisions)
	{
		btDynamicsWorld* world = GetWorld().GetDynamicsWorld();
		world->addConstraint(m_constraint.get(), disableCollisions);
	}

	BulletConstraint3D::BulletConstraint3D(BulletConstraint3D&& constraint) noexcept :
	m_constraint(std::move(constraint.m_constraint)),
	m_bodyCollisionEnabled(constraint.m_bodyCollisionEnabled)
	{
		if (m_constraint)
			m_constraint->setUserConstraintPtr(this);
	}

	BulletConstraint3D::~BulletConstraint3D()
	{
		if (m_constraint)
		{
			btDynamicsWorld* world = GetWorld().GetDynamicsWorld();
			world->removeConstraint(m_constraint.get());
		}
	}

	BulletRigidBody3D& BulletConstraint3D::GetBodyA()
	{
		return *static_cast<BulletRigidBody3D*>(m_constraint->getRigidBodyA().getUserPointer());
	}

	const BulletRigidBody3D& BulletConstraint3D::GetBodyA() const
	{
		return *static_cast<BulletRigidBody3D*>(m_constraint->getRigidBodyA().getUserPointer());
	}

	BulletRigidBody3D& BulletConstraint3D::GetBodyB()
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<BulletRigidBody3D*>(m_constraint->getRigidBodyB().getUserPointer());
	}

	const BulletRigidBody3D& BulletConstraint3D::GetBodyB() const
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *static_cast<BulletRigidBody3D*>(m_constraint->getRigidBodyB().getUserPointer());
	}

	BulletPhysWorld3D& BulletConstraint3D::GetWorld()
	{
		return *GetBodyA().GetWorld();
	}

	const BulletPhysWorld3D& BulletConstraint3D::GetWorld() const
	{
		return *GetBodyA().GetWorld();
	}

	bool BulletConstraint3D::IsSingleBody() const
	{
		return &m_constraint->getRigidBodyB() == &btTypedConstraint::getFixedBody();
	}

	BulletConstraint3D& BulletConstraint3D::operator=(BulletConstraint3D&& constraint) noexcept
	{
		m_constraint.reset();

		m_constraint = std::move(constraint.m_constraint);
		m_bodyCollisionEnabled = constraint.m_bodyCollisionEnabled;

		if (m_constraint)
			m_constraint->setUserConstraintPtr(this);

		return *this;
	}


	BulletPivotConstraint3D::BulletPivotConstraint3D(BulletRigidBody3D& first, const Vector3f& pivot) :
	BulletConstraint3D(std::make_unique<btPoint2PointConstraint>(*first.GetRigidBody(), ToBullet(first.ToLocal(pivot))))
	{
	}

	BulletPivotConstraint3D::BulletPivotConstraint3D(BulletRigidBody3D& first, BulletRigidBody3D& second, const Vector3f& pivot, bool disableCollisions) :
	BulletConstraint3D(std::make_unique<btPoint2PointConstraint>(*first.GetRigidBody(), *second.GetRigidBody(), ToBullet(first.ToLocal(pivot)), ToBullet(second.ToLocal(pivot))), disableCollisions)
	{
	}

	BulletPivotConstraint3D::BulletPivotConstraint3D(BulletRigidBody3D& first, BulletRigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, bool disableCollisions) :
	BulletConstraint3D(std::make_unique<btPoint2PointConstraint>(*first.GetRigidBody(), *second.GetRigidBody(), ToBullet(firstAnchor), ToBullet(secondAnchor)), disableCollisions)
	{
	}

	Vector3f BulletPivotConstraint3D::GetFirstAnchor() const
	{
		return FromBullet(GetConstraint<btPoint2PointConstraint>()->getPivotInA());
	}

	Vector3f BulletPivotConstraint3D::GetSecondAnchor() const
	{
		return FromBullet(GetConstraint<btPoint2PointConstraint>()->getPivotInB());
	}

	void BulletPivotConstraint3D::SetFirstAnchor(const Vector3f& firstAnchor)
	{
		GetConstraint<btPoint2PointConstraint>()->setPivotA(ToBullet(firstAnchor));
	}

	void BulletPivotConstraint3D::SetSecondAnchor(const Vector3f& secondAnchor)
	{
		GetConstraint<btPoint2PointConstraint>()->setPivotB(ToBullet(secondAnchor));
	}
}
