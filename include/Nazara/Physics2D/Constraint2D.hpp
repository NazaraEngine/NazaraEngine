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
	class Constraint2D;

	using Constraint2DConstRef = ObjectRef<const Constraint2D>;
	using Constraint2DLibrary = ObjectLibrary<Constraint2D>;
	using Constraint2DRef = ObjectRef<Constraint2D>;

	class NAZARA_PHYSICS2D_API Constraint2D : public RefCounted
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
			Constraint2D(cpConstraint* constraint);

			MovablePtr<cpConstraint> m_constraint;
	};

	class DampedSpringConstraint2D;

	using DampedSpringConstraint2DConstRef = ObjectRef<const DampedSpringConstraint2D>;
	using DampedSpringConstraint2DLibrary = ObjectLibrary<DampedSpringConstraint2D>;
	using DampedSpringConstraint2DRef = ObjectRef<DampedSpringConstraint2D>;
	
	class NAZARA_PHYSICS2D_API DampedSpringConstraint2D : public Constraint2D
	{
		public:
			DampedSpringConstraint2D(RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor, float restLength, float stiffness, float damping);
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

	class DampedRotarySpringConstraint2D;

	using DampedRotarySpringConstraint2DConstRef = ObjectRef<const DampedRotarySpringConstraint2D>;
	using DampedRotarySpringConstraint2DLibrary = ObjectLibrary<DampedRotarySpringConstraint2D>;
	using DampedRotarySpringConstraint2DRef = ObjectRef<DampedRotarySpringConstraint2D>;

	class NAZARA_PHYSICS2D_API DampedRotarySpringConstraint2D : public Constraint2D
	{
		public:
			DampedRotarySpringConstraint2D(RigidBody2D& first, RigidBody2D& second, float restAngle, float stiffness, float damping);
			~DampedRotarySpringConstraint2D() = default;

			float GetDamping() const;
			float GetRestAngle() const;
			float GetStiffness() const;

			void SetDamping(float newDamping);
			void SetRestAngle(float newAngle);
			void SetStiffness(float newStiffness);
	};

	class GearConstraint2D;

	using GearConstraint2DConstRef = ObjectRef<const GearConstraint2D>;
	using GearConstraint2DLibrary = ObjectLibrary<GearConstraint2D>;
	using GearConstraint2DRef = ObjectRef<GearConstraint2D>;
	
	class NAZARA_PHYSICS2D_API GearConstraint2D : public Constraint2D
	{
		public:
			GearConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratio);
			~GearConstraint2D() = default;

			float GetPhase() const;
			float GetRatio() const;

			void SetPhase(float phase);
			void SetRatio(float ratio);
	};

	class MotorConstraint2D;

	using MotorConstraint2DConstRef = ObjectRef<const MotorConstraint2D>;
	using MotorConstraint2DLibrary = ObjectLibrary<MotorConstraint2D>;
	using MotorConstraint2DRef = ObjectRef<MotorConstraint2D>;
	
	class NAZARA_PHYSICS2D_API MotorConstraint2D : public Constraint2D
	{
		public:
			MotorConstraint2D(RigidBody2D& first, RigidBody2D& second, float rate);
			~MotorConstraint2D() = default;

			float GetRate() const;
			void SetRate(float rate);
	};

	class PinConstraint2D;

	using PinConstraint2DConstRef = ObjectRef<const PinConstraint2D>;
	using PinConstraint2DLibrary = ObjectLibrary<PinConstraint2D>;
	using PinConstraint2DRef = ObjectRef<PinConstraint2D>;

	class NAZARA_PHYSICS2D_API PinConstraint2D : public Constraint2D
	{
		public:
			PinConstraint2D(RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor);
			~PinConstraint2D() = default;

			float GetDistance() const;
			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetDistance(float newDistance);
			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class PivotConstraint2D;

	using PivotConstraint2DConstRef = ObjectRef<const PivotConstraint2D>;
	using PivotConstraint2DLibrary = ObjectLibrary<PivotConstraint2D>;
	using PivotConstraint2DRef = ObjectRef<PivotConstraint2D>;
	
	class NAZARA_PHYSICS2D_API PivotConstraint2D : public Constraint2D
	{
		public:
			PivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor);
			PivotConstraint2D(RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor);
			~PivotConstraint2D() = default;

			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class RatchetConstraint2D;

	using RatchetConstraint2DConstRef = ObjectRef<const RatchetConstraint2D>;
	using RatchetConstraint2DLibrary = ObjectLibrary<RatchetConstraint2D>;
	using RatchetConstraint2DRef = ObjectRef<RatchetConstraint2D>;
	
	class NAZARA_PHYSICS2D_API RatchetConstraint2D : public Constraint2D
	{
		public:
			RatchetConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratchet);
			~RatchetConstraint2D() = default;

			float GetAngle() const;
			float GetPhase() const;
			float GetRatchet() const;

			void SetAngle(float angle);
			void SetPhase(float phase);
			void SetRatchet(float ratchet);
	};

	class RotaryLimitConstraint2D;

	using RotaryLimitConstraint2DConstRef = ObjectRef<const RotaryLimitConstraint2D>;
	using RotaryLimitConstraint2DLibrary = ObjectLibrary<RotaryLimitConstraint2D>;
	using RotaryLimitConstraint2DRef = ObjectRef<RotaryLimitConstraint2D>;

	class NAZARA_PHYSICS2D_API RotaryLimitConstraint2D : public Constraint2D
	{
		public:
			RotaryLimitConstraint2D(RigidBody2D& first, RigidBody2D& second, float minAngle, float maxAngle);
			~RotaryLimitConstraint2D() = default;

			float GetMaxAngle() const;
			float GetMinAngle() const;

			void SetMaxAngle(float maxAngle);
			void SetMinAngle(float minAngle);
	};

	class RotaryLimitConstraint2D;

	using RotaryLimitConstraint2DConstRef = ObjectRef<const RotaryLimitConstraint2D>;
	using RotaryLimitConstraint2DLibrary = ObjectLibrary<RotaryLimitConstraint2D>;
	using RotaryLimitConstraint2DRef = ObjectRef<RotaryLimitConstraint2D>;

	class NAZARA_PHYSICS2D_API SlideConstraint2D : public Constraint2D
	{
		public:
			SlideConstraint2D(RigidBody2D& first, const Vector2f& firstAnchor, RigidBody2D& second, const Vector2f& secondAnchor, float min, float max);
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
