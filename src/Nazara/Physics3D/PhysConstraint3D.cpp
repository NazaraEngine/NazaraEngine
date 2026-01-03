// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/PhysConstraint3D.hpp>
#include <Nazara/Physics3D/JoltHelper.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <cassert>

namespace Nz
{
	PhysConstraint3D::PhysConstraint3D() = default;

	PhysConstraint3D::PhysConstraint3D(PhysConstraint3D&& constraint) noexcept :
	m_constraint(std::move(constraint.m_constraint))
	{
		if (m_constraint)
			m_constraint->SetUserData(PointerToInteger<UInt64>(this));
	}

	PhysConstraint3D::~PhysConstraint3D()
	{
		Destroy();
	}

	RigidBody3D& PhysConstraint3D::GetBodyA()
	{
		return *IntegerToPointer<RigidBody3D*>(m_constraint->GetBody1()->GetUserData());
	}

	const RigidBody3D& PhysConstraint3D::GetBodyA() const
	{
		return *IntegerToPointer<RigidBody3D*>(m_constraint->GetBody1()->GetUserData());
	}

	RigidBody3D& PhysConstraint3D::GetBodyB()
	{
		NazaraAssertMsg(!IsSingleBody(), "constraint is not attached to a second body");
		return *IntegerToPointer<RigidBody3D*>(m_constraint->GetBody2()->GetUserData());
	}

	const RigidBody3D& PhysConstraint3D::GetBodyB() const
	{
		NazaraAssertMsg(!IsSingleBody(), "constraint is not attached to a second body");
		return *IntegerToPointer<RigidBody3D*>(m_constraint->GetBody2()->GetUserData());
	}

	PhysWorld3D& PhysConstraint3D::GetWorld()
	{
		return GetBodyA().GetWorld();
	}

	const PhysWorld3D& PhysConstraint3D::GetWorld() const
	{
		return GetBodyA().GetWorld();
	}

	bool PhysConstraint3D::IsSingleBody() const
	{
		return m_constraint->GetBody2() == &JPH::Body::sFixedToWorld;
	}

	PhysConstraint3D& PhysConstraint3D::operator=(PhysConstraint3D&& constraint) noexcept
	{
		Destroy();

		m_constraint = std::move(constraint.m_constraint);

		if (m_constraint)
			m_constraint->SetUserData(PointerToInteger<UInt64>(this));

		return *this;
	}

	void PhysConstraint3D::Destroy()
	{
		if (m_constraint)
		{
			JPH::PhysicsSystem* physicsSystem = GetWorld().GetPhysicsSystem();
			physicsSystem->RemoveConstraint(m_constraint.get());

			m_constraint.reset();
		}
	}

	void PhysConstraint3D::SetupConstraint(std::unique_ptr<JPH::TwoBodyConstraint> constraint)
	{
		assert(!m_constraint);
		m_constraint = std::move(constraint);
		m_constraint->SetEmbedded();
		m_constraint->SetUserData(PointerToInteger<UInt64>(this));

		JPH::PhysicsSystem* physicsSystem = GetWorld().GetPhysicsSystem();
		physicsSystem->AddConstraint(m_constraint.get());
	}


	PhysDistanceConstraint3D::PhysDistanceConstraint3D(RigidBody3D& first, const Vector3f& pivot, float maxDist, float minDist)
	{
		JPH::DistanceConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;
		settings.mMaxDistance = maxDist;
		settings.mMinDistance = minDist;

		SetupConstraint(std::make_unique<JPH::DistanceConstraint>(*first.GetBody(), JPH::Body::sFixedToWorld, settings));
	}

	PhysDistanceConstraint3D::PhysDistanceConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& pivot, float maxDist, float minDist)
	{
		JPH::DistanceConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;
		settings.mMaxDistance = maxDist;
		settings.mMinDistance = minDist;

		SetupConstraint(std::make_unique<JPH::DistanceConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	PhysDistanceConstraint3D::PhysDistanceConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, float maxDist, float minDist)
	{
		JPH::DistanceConstraintSettings settings;
		settings.mPoint1 = ToJolt(firstAnchor);
		settings.mPoint2 = ToJolt(secondAnchor);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;
		settings.mMaxDistance = maxDist;
		settings.mMinDistance = minDist;

		SetupConstraint(std::make_unique<JPH::DistanceConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	float PhysDistanceConstraint3D::GetDamping() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mDamping;
	}

	float PhysDistanceConstraint3D::GetFrequency() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mFrequency;
	}

	float PhysDistanceConstraint3D::GetMaxDistance() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetMaxDistance();
	}

	float PhysDistanceConstraint3D::GetMinDistance() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetMinDistance();
	}

	void PhysDistanceConstraint3D::SetDamping(float damping)
	{
		GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mDamping = damping;
	}

	void PhysDistanceConstraint3D::SetDistance(float minDist, float maxDist)
	{
		GetConstraint<JPH::DistanceConstraint>()->SetDistance(minDist, maxDist);
	}

	void PhysDistanceConstraint3D::SetFrequency(float frequency)
	{
		GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mFrequency = frequency;
	}

	void PhysDistanceConstraint3D::SetMaxDistance(float maxDist)
	{
		JPH::DistanceConstraint* constraint = GetConstraint<JPH::DistanceConstraint>();

		constraint->SetDistance(constraint->GetMinDistance(), maxDist);
	}

	void PhysDistanceConstraint3D::SetMinDistance(float minDist)
	{
		JPH::DistanceConstraint* constraint = GetConstraint<JPH::DistanceConstraint>();

		constraint->SetDistance(minDist, constraint->GetMaxDistance());
	}


	PhysPivotConstraint3D::PhysPivotConstraint3D(RigidBody3D& first, const Vector3f& pivot)
	{
		JPH::PointConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;

		SetupConstraint(std::make_unique<JPH::PointConstraint>(*first.GetBody(), JPH::Body::sFixedToWorld, settings));
	}

	PhysPivotConstraint3D::PhysPivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& pivot)
	{
		JPH::PointConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;

		SetupConstraint(std::make_unique<JPH::PointConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	PhysPivotConstraint3D::PhysPivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor)
	{
		JPH::PointConstraintSettings settings;
		settings.mPoint1 = ToJolt(firstAnchor);
		settings.mPoint2 = ToJolt(secondAnchor);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;

		SetupConstraint(std::make_unique<JPH::PointConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	Vector3f PhysPivotConstraint3D::GetFirstAnchor() const
	{
		const JPH::PointConstraint* constraint = GetConstraint<JPH::PointConstraint>();

		return FromJolt(constraint->GetBody1()->GetCenterOfMassTransform() * constraint->GetLocalSpacePoint1());
	}

	Vector3f PhysPivotConstraint3D::GetSecondAnchor() const
	{
		const JPH::PointConstraint* constraint = GetConstraint<JPH::PointConstraint>();

		return FromJolt(constraint->GetBody2()->GetCenterOfMassTransform() * constraint->GetLocalSpacePoint2());
	}

	void PhysPivotConstraint3D::SetFirstAnchor(const Vector3f& firstAnchor)
	{
		GetConstraint<JPH::PointConstraint>()->SetPoint1(JPH::EConstraintSpace::WorldSpace, ToJolt(firstAnchor));
		GetConstraint<JPH::PointConstraint>()->SetPoint2(JPH::EConstraintSpace::WorldSpace, ToJolt(firstAnchor));
	}

	void PhysPivotConstraint3D::SetSecondAnchor(const Vector3f& secondAnchor)
	{
		GetConstraint<JPH::PointConstraint>()->SetPoint2(JPH::EConstraintSpace::WorldSpace, ToJolt(secondAnchor));
	}
}
