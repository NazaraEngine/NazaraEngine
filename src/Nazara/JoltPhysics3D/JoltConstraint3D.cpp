// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltConstraint3D.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <cassert>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltConstraint3D::JoltConstraint3D() = default;

	JoltConstraint3D::JoltConstraint3D(JoltConstraint3D&& constraint) noexcept :
	m_constraint(std::move(constraint.m_constraint))
	{
		if (m_constraint)
			m_constraint->SetUserData(SafeCast<UInt64>(BitCast<std::uintptr_t>(this)));
	}

	JoltConstraint3D::~JoltConstraint3D()
	{
		Destroy();
	}

	JoltRigidBody3D& JoltConstraint3D::GetBodyA()
	{
		return *BitCast<JoltRigidBody3D*>(static_cast<std::uintptr_t>(m_constraint->GetBody1()->GetUserData()));
	}

	const JoltRigidBody3D& JoltConstraint3D::GetBodyA() const
	{
		return *BitCast<JoltRigidBody3D*>(static_cast<std::uintptr_t>(m_constraint->GetBody1()->GetUserData()));
	}

	JoltRigidBody3D& JoltConstraint3D::GetBodyB()
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *BitCast<JoltRigidBody3D*>(static_cast<std::uintptr_t>(m_constraint->GetBody2()->GetUserData()));
	}

	const JoltRigidBody3D& JoltConstraint3D::GetBodyB() const
	{
		NazaraAssert(!IsSingleBody(), "constraint is not attached to a second body");
		return *BitCast<JoltRigidBody3D*>(static_cast<std::uintptr_t>(m_constraint->GetBody2()->GetUserData()));
	}

	JoltPhysWorld3D& JoltConstraint3D::GetWorld()
	{
		return GetBodyA().GetWorld();
	}

	const JoltPhysWorld3D& JoltConstraint3D::GetWorld() const
	{
		return GetBodyA().GetWorld();
	}

	bool JoltConstraint3D::IsSingleBody() const
	{
		return m_constraint->GetBody2() == &JPH::Body::sFixedToWorld;
	}

	JoltConstraint3D& JoltConstraint3D::operator=(JoltConstraint3D&& constraint) noexcept
	{
		Destroy();

		m_constraint = std::move(constraint.m_constraint);

		if (m_constraint)
			m_constraint->SetUserData(SafeCast<UInt64>(BitCast<std::uintptr_t>(this)));

		return *this;
	}

	void JoltConstraint3D::Destroy()
	{
		if (m_constraint)
		{
			JPH::PhysicsSystem* physicsSystem = GetWorld().GetPhysicsSystem();
			physicsSystem->RemoveConstraint(m_constraint.get());

			m_constraint.reset();
		}
	}

	void JoltConstraint3D::SetupConstraint(std::unique_ptr<JPH::TwoBodyConstraint> constraint)
	{
		assert(!m_constraint);
		m_constraint = std::move(constraint);
		m_constraint->SetEmbedded();
		m_constraint->SetUserData(SafeCast<UInt64>(BitCast<std::uintptr_t>(this)));

		JPH::PhysicsSystem* physicsSystem = GetWorld().GetPhysicsSystem();
		physicsSystem->AddConstraint(m_constraint.get());
	}


	JoltDistanceConstraint3D::JoltDistanceConstraint3D(JoltRigidBody3D& first, const Vector3f& pivot, float maxDist, float minDist)
	{
		JPH::DistanceConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;
		settings.mMaxDistance = maxDist;
		settings.mMinDistance = minDist;

		SetupConstraint(std::make_unique<JPH::DistanceConstraint>(*first.GetBody(), JPH::Body::sFixedToWorld, settings));
	}

	JoltDistanceConstraint3D::JoltDistanceConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& pivot, float maxDist, float minDist)
	{
		JPH::DistanceConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;
		settings.mMaxDistance = maxDist;
		settings.mMinDistance = minDist;

		SetupConstraint(std::make_unique<JPH::DistanceConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	JoltDistanceConstraint3D::JoltDistanceConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, float maxDist, float minDist)
	{
		JPH::DistanceConstraintSettings settings;
		settings.mPoint1 = ToJolt(firstAnchor);
		settings.mPoint2 = ToJolt(secondAnchor);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;
		settings.mMaxDistance = maxDist;
		settings.mMinDistance = minDist;

		SetupConstraint(std::make_unique<JPH::DistanceConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	float JoltDistanceConstraint3D::GetDamping() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mDamping;
	}

	float JoltDistanceConstraint3D::GetFrequency() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mFrequency;
	}

	float JoltDistanceConstraint3D::GetMaxDistance() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetMaxDistance();
	}

	float JoltDistanceConstraint3D::GetMinDistance() const
	{
		return GetConstraint<JPH::DistanceConstraint>()->GetMinDistance();
	}

	void JoltDistanceConstraint3D::SetDamping(float damping)
	{
		GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mDamping = damping;
	}

	void JoltDistanceConstraint3D::SetDistance(float minDist, float maxDist)
	{
		GetConstraint<JPH::DistanceConstraint>()->SetDistance(minDist, maxDist);
	}

	void JoltDistanceConstraint3D::SetFrequency(float frequency)
	{
		GetConstraint<JPH::DistanceConstraint>()->GetLimitsSpringSettings().mFrequency = frequency;
	}

	void JoltDistanceConstraint3D::SetMaxDistance(float maxDist)
	{
		JPH::DistanceConstraint* constraint = GetConstraint<JPH::DistanceConstraint>();

		constraint->SetDistance(constraint->GetMinDistance(), maxDist);
	}

	void JoltDistanceConstraint3D::SetMinDistance(float minDist)
	{
		JPH::DistanceConstraint* constraint = GetConstraint<JPH::DistanceConstraint>();

		constraint->SetDistance(minDist, constraint->GetMaxDistance());
	}


	JoltPivotConstraint3D::JoltPivotConstraint3D(JoltRigidBody3D& first, const Vector3f& pivot)
	{
		JPH::PointConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;

		SetupConstraint(std::make_unique<JPH::PointConstraint>(*first.GetBody(), JPH::Body::sFixedToWorld, settings));
	}

	JoltPivotConstraint3D::JoltPivotConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& pivot)
	{
		JPH::PointConstraintSettings settings;
		settings.mPoint1 = ToJolt(pivot);
		settings.mPoint2 = ToJolt(pivot);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;

		SetupConstraint(std::make_unique<JPH::PointConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	JoltPivotConstraint3D::JoltPivotConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor)
	{
		JPH::PointConstraintSettings settings;
		settings.mPoint1 = ToJolt(firstAnchor);
		settings.mPoint2 = ToJolt(secondAnchor);
		settings.mSpace = JPH::EConstraintSpace::WorldSpace;

		SetupConstraint(std::make_unique<JPH::PointConstraint>(*first.GetBody(), *second.GetBody(), settings));
	}

	Vector3f JoltPivotConstraint3D::GetFirstAnchor() const
	{
		const JPH::PointConstraint* constraint = GetConstraint<JPH::PointConstraint>();

		return FromJolt(constraint->GetBody1()->GetCenterOfMassTransform() * constraint->GetLocalSpacePoint1());
	}

	Vector3f JoltPivotConstraint3D::GetSecondAnchor() const
	{
		const JPH::PointConstraint* constraint = GetConstraint<JPH::PointConstraint>();

		return FromJolt(constraint->GetBody2()->GetCenterOfMassTransform() * constraint->GetLocalSpacePoint2());
	}

	void JoltPivotConstraint3D::SetFirstAnchor(const Vector3f& firstAnchor)
	{
		GetConstraint<JPH::PointConstraint>()->SetPoint1(JPH::EConstraintSpace::WorldSpace, ToJolt(firstAnchor));
		GetConstraint<JPH::PointConstraint>()->SetPoint2(JPH::EConstraintSpace::WorldSpace, ToJolt(firstAnchor));
	}

	void JoltPivotConstraint3D::SetSecondAnchor(const Vector3f& secondAnchor)
	{
		GetConstraint<JPH::PointConstraint>()->SetPoint2(JPH::EConstraintSpace::WorldSpace, ToJolt(secondAnchor));
	}
}
