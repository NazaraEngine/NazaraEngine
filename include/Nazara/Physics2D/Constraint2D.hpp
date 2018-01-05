// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONSTRAINT2D_HPP
#define NAZARA_CONSTRAINT2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <vector>

struct cpConstraint;

namespace Nz
{
	class NAZARA_PHYSICS2D_API Constraint2D
	{
		public:
			Constraint2D(const Constraint2D&) = delete;
			Constraint2D(Constraint2D&& rhs);
			virtual ~Constraint2D();

			void EnableBodyCollision(bool enable);

			RigidBody2D& GetBodyA();
			const RigidBody2D& GetBodyA() const;
			RigidBody2D& GetBodyB();
			const RigidBody2D& GetBodyB() const;
			float GetErrorBias() const;
			float GetMaxBias() const;
			float GetMaxForce() const;
			PhysWorld2D& GetWorld();
			const PhysWorld2D& GetWorld() const;

			bool IsBodyCollisionEnabled() const;

			void SetErrorBias(float bias);
			void SetMaxBias(float bias);
			void SetMaxForce(float force);

			Constraint2D& operator=(const Constraint2D&) = delete;
			Constraint2D& operator=(Constraint2D&& rhs);

		protected:
			Constraint2D(PhysWorld2D& world, cpConstraint* constraint);

			MovablePtr<cpConstraint> m_constraint;
	};
	
	class NAZARA_PHYSICS2D_API DampedSpringConstraint2D : public Constraint2D
	{
		public:
			DampedSpringConstraint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor, float restLength, float stiffness, float damping);
			~DampedSpringConstraint2D() = default;

			float GetDamping() const;
			Vector2f GetFirstAnchor() const;
			float GetRestLength() const;
			Vector2f GetSecondAnchor() const;
			float GetStiffness() const;

			void SetDamping(float newDamping);
			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetRestLength(float newLength);
			void SetSecondAnchor(const Vector2f& firstAnchor);
			void SetStiffness(float newStiffness);
	};

	class NAZARA_PHYSICS2D_API DampedRotarySpringConstraint2D : public Constraint2D
	{
		public:
			DampedRotarySpringConstraint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float restAngle, float stiffness, float damping);
			~DampedRotarySpringConstraint2D() = default;

			float GetDamping() const;
			float GetRestAngle() const;
			float GetStiffness() const;

			void SetDamping(float newDamping);
			void SetRestAngle(float newAngle);
			void SetStiffness(float newStiffness);
	};
	
	class NAZARA_PHYSICS2D_API GearConstraint2D : public Constraint2D
	{
		public:
			GearConstraint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float phase, float ratio);
			~GearConstraint2D() = default;

			float GetPhase() const;
			float GetRatio() const;

			void SetPhase(float phase);
			void SetRatio(float ratio);
	};
	
	class NAZARA_PHYSICS2D_API MotorConstraint2D : public Constraint2D
	{
		public:
			MotorConstraint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float rate);
			~MotorConstraint2D() = default;

			float GetRate() const;
			void SetRate(float rate);
	};

	class NAZARA_PHYSICS2D_API PinConstraint2D : public Constraint2D
	{
		public:
			PinConstraint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor);
			~PinConstraint2D() = default;

			float GetDistance() const;
			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetDistance(float newDistance);
			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};
	
	class NAZARA_PHYSICS2D_API PivotConstraint2D : public Constraint2D
	{
		public:
			PivotConstraint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor);
			PivotConstraint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor);
			~PivotConstraint2D() = default;

			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};
	
	class NAZARA_PHYSICS2D_API RatchetConstraint2D : public Constraint2D
	{
		public:
			RatchetConstraint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float phase, float ratchet);
			~RatchetConstraint2D() = default;

			float GetAngle() const;
			float GetPhase() const;
			float GetRatchet() const;

			void SetAngle(float angle);
			void SetPhase(float phase);
			void SetRatchet(float ratchet);
	};

	class NAZARA_PHYSICS2D_API RotaryLimitConstraint2D : public Constraint2D
	{
		public:
			RotaryLimitConstraint2D(PhysWorld2D& world, RigidBody2D& first, RigidBody2D& second, float minAngle, float maxAngle);
			~RotaryLimitConstraint2D() = default;

			float GetMaxAngle() const;
			float GetMinAngle() const;

			void SetMaxAngle(float maxAngle);
			void SetMinAngle(float minAngle);
	};

	class NAZARA_PHYSICS2D_API SlideConstraint2D : public Constraint2D
	{
		public:
			SlideConstraint2D(PhysWorld2D& world, RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor, float min, float max);
			~SlideConstraint2D() = default;

			Vector2f GetFirstAnchor() const;
			float GetMaxDistance() const;
			float GetMinDistance() const;
			Vector2f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetMaxDistance(float newMaxDistance);
			void SetMinDistance(float newMinDistance);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};
}

#include <Nazara/Physics2D/Constraint2D.inl>

#endif // NAZARA_CONSTRAINT2D_HPP
