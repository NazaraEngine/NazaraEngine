// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONSTRAINT2D_HPP
#define NAZARA_CONSTRAINT2D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <vector>

struct cpConstraint;

namespace Nz
{
	class Constraint2D;

	using Constraint2DHandle = ObjectHandle<Constraint2D>;

	class NAZARA_PHYSICS2D_API Constraint2D : public HandledObject<Constraint2D>
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
			float GetLastImpulse() const;
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
			Constraint2D(Nz::PhysWorld2D* world, cpConstraint* constraint);

			MovablePtr<cpConstraint> m_constraint;
	};

	class DampedSpringConstraint2D;

	using DampedSpringConstraint2DHandle = ObjectHandle<DampedSpringConstraint2D>;

	class NAZARA_PHYSICS2D_API DampedSpringConstraint2D : public Constraint2D
	{
		public:
			DampedSpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float restLength, float stiffness, float damping);
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

	using DampedRotarySpringConstraint2DHandle = ObjectHandle<DampedRotarySpringConstraint2D>;

	class NAZARA_PHYSICS2D_API DampedRotarySpringConstraint2D : public Constraint2D
	{
		public:
			DampedRotarySpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& restAngle, float stiffness, float damping);
			~DampedRotarySpringConstraint2D() = default;

			float GetDamping() const;
			RadianAnglef GetRestAngle() const;
			float GetStiffness() const;

			void SetDamping(float newDamping);
			void SetRestAngle(const RadianAnglef& newAngle);
			void SetStiffness(float newStiffness);
	};

	class GearConstraint2D;

	using GearConstraint2DHandle = ObjectHandle<GearConstraint2D>;
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

			template<typename... Args> static GearConstraint2DRef New(Args&&... args);
	};

	class MotorConstraint2D;

	using MotorConstraint2DHandle = ObjectHandle<MotorConstraint2D>;
	
	class NAZARA_PHYSICS2D_API MotorConstraint2D : public Constraint2D
	{
		public:
			MotorConstraint2D(RigidBody2D& first, RigidBody2D& second, float rate);
			~MotorConstraint2D() = default;

			float GetRate() const;
			void SetRate(float rate);
	};

	class PinConstraint2D;

	using PinConstraint2DHandle = ObjectHandle<PinConstraint2D>;

	class NAZARA_PHYSICS2D_API PinConstraint2D : public Constraint2D
	{
		public:
			PinConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor);
			~PinConstraint2D() = default;

			float GetDistance() const;
			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetDistance(float newDistance);
			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class PivotConstraint2D;

	using PivotConstraint2DHandle = ObjectHandle<PivotConstraint2D>;
	
	class NAZARA_PHYSICS2D_API PivotConstraint2D : public Constraint2D
	{
		public:
			PivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor);
			PivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor);
			~PivotConstraint2D() = default;

			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class RatchetConstraint2D;

	using RatchetConstraint2DHandle = ObjectHandle<RatchetConstraint2D>;
	
	class NAZARA_PHYSICS2D_API RatchetConstraint2D : public Constraint2D
	{
		public:
			RatchetConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratchet);
			~RatchetConstraint2D() = default;

			RadianAnglef GetAngle() const;
			float GetPhase() const;
			float GetRatchet() const;

			void SetAngle(const RadianAnglef& angle);
			void SetPhase(float phase);
			void SetRatchet(float ratchet);
	};

	class RotaryLimitConstraint2D;

	using RotaryLimitConstraint2DHandle = ObjectHandle<RotaryLimitConstraint2D>;

	class NAZARA_PHYSICS2D_API RotaryLimitConstraint2D : public Constraint2D
	{
		public:
			RotaryLimitConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& minAngle, const RadianAnglef& maxAngle);
			~RotaryLimitConstraint2D() = default;

			RadianAnglef GetMaxAngle() const;
			RadianAnglef GetMinAngle() const;

			void SetMaxAngle(const RadianAnglef& maxAngle);
			void SetMinAngle(const RadianAnglef& minAngle);
	};

	class SlideConstraint2D;

	using SlideConstraint2DHandle = ObjectHandle<SlideConstraint2D>;

	class NAZARA_PHYSICS2D_API SlideConstraint2D : public Constraint2D
	{
		public:
			SlideConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float min, float max);
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
