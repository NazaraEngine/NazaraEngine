// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_PHYSCONSTRAINT2D_HPP
#define NAZARA_PHYSICS2D_PHYSCONSTRAINT2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Physics2D/Export.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NazaraUtils/MovablePtr.hpp>

struct cpConstraint;

namespace Nz
{
	class PhysConstraint2D;

	using PhysConstraint2DHandle = ObjectHandle<PhysConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysConstraint2D : public HandledObject<PhysConstraint2D>
	{
		public:
			PhysConstraint2D(const PhysConstraint2D&) = delete;
			PhysConstraint2D(PhysConstraint2D&& constraint) noexcept;
			virtual ~PhysConstraint2D();

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
			bool IsSingleBody() const;

			void SetErrorBias(float bias);
			void SetMaxBias(float bias);
			void SetMaxForce(float force);

			PhysConstraint2D& operator=(const PhysConstraint2D&) = delete;
			PhysConstraint2D& operator=(PhysConstraint2D&& constraint) noexcept;

		protected:
			PhysConstraint2D(PhysWorld2D* world, cpConstraint* constraint);

			MovablePtr<cpConstraint> m_constraint;

		private:
			void Destroy();
	};

	class PhysDampedSpringConstraint2D;

	using PhysDampedSpringConstraint2DHandle = ObjectHandle<PhysDampedSpringConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysDampedSpringConstraint2D : public PhysConstraint2D
	{
		public:
			PhysDampedSpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float restLength, float stiffness, float damping);
			~PhysDampedSpringConstraint2D() = default;

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

	class PhysDampedRotarySpringConstraint2D;

	using PhysDampedRotarySpringConstraint2DHandle = ObjectHandle<PhysDampedRotarySpringConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysDampedRotarySpringConstraint2D : public PhysConstraint2D
	{
		public:
			PhysDampedRotarySpringConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& restAngle, float stiffness, float damping);
			~PhysDampedRotarySpringConstraint2D() = default;

			float GetDamping() const;
			RadianAnglef GetRestAngle() const;
			float GetStiffness() const;

			void SetDamping(float newDamping);
			void SetRestAngle(const RadianAnglef& newAngle);
			void SetStiffness(float newStiffness);
	};

	class PhysGearConstraint2D;

	using PhysGearConstraint2DHandle = ObjectHandle<PhysGearConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysGearConstraint2D : public PhysConstraint2D
	{
		public:
			PhysGearConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratio);
			~PhysGearConstraint2D() = default;

			float GetPhase() const;
			float GetRatio() const;

			void SetPhase(float phase);
			void SetRatio(float ratio);
	};

	class PhysMotorConstraint2D;

	using PhysMotorConstraint2DHandle = ObjectHandle<PhysMotorConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysMotorConstraint2D : public PhysConstraint2D
	{
		public:
			PhysMotorConstraint2D(RigidBody2D& first, RigidBody2D& second, float rate);
			~PhysMotorConstraint2D() = default;

			float GetRate() const;
			void SetRate(float rate);
	};

	class PhysPinConstraint2D;

	using PhysPinConstraint2DHandle = ObjectHandle<PhysPinConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysPinConstraint2D : public PhysConstraint2D
	{
		public:
			PhysPinConstraint2D(RigidBody2D& body, const Vector2f& anchor);
			PhysPinConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor);
			~PhysPinConstraint2D() = default;

			float GetDistance() const;
			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetDistance(float newDistance);
			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class PhysPivotConstraint2D;

	using PhysPivotConstraint2DHandle = ObjectHandle<PhysPivotConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysPivotConstraint2D : public PhysConstraint2D
	{
		public:
			PhysPivotConstraint2D(RigidBody2D& body, const Vector2f& anchor);
			PhysPivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& anchor);
			PhysPivotConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor);
			~PhysPivotConstraint2D() = default;

			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class PhysRatchetConstraint2D;

	using PhysRatchetConstraint2DHandle = ObjectHandle<PhysRatchetConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysRatchetConstraint2D : public PhysConstraint2D
	{
		public:
			PhysRatchetConstraint2D(RigidBody2D& first, RigidBody2D& second, float phase, float ratchet);
			~PhysRatchetConstraint2D() = default;

			RadianAnglef GetAngle() const;
			float GetPhase() const;
			float GetRatchet() const;

			void SetAngle(const RadianAnglef& angle);
			void SetPhase(float phase);
			void SetRatchet(float ratchet);
	};

	class PhysRotaryLimitConstraint2D;

	using PhysRotaryLimitConstraint2DHandle = ObjectHandle<PhysRotaryLimitConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysRotaryLimitConstraint2D : public PhysConstraint2D
	{
		public:
			PhysRotaryLimitConstraint2D(RigidBody2D& first, RigidBody2D& second, const RadianAnglef& minAngle, const RadianAnglef& maxAngle);
			~PhysRotaryLimitConstraint2D() = default;

			RadianAnglef GetMaxAngle() const;
			RadianAnglef GetMinAngle() const;

			void SetMaxAngle(const RadianAnglef& maxAngle);
			void SetMinAngle(const RadianAnglef& minAngle);
	};

	class PhysSlideConstraint2D;

	using PhysSlideConstraint2DHandle = ObjectHandle<PhysSlideConstraint2D>;

	class NAZARA_PHYSICS2D_API PhysSlideConstraint2D : public PhysConstraint2D
	{
		public:
			PhysSlideConstraint2D(RigidBody2D& first, RigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float min, float max);
			~PhysSlideConstraint2D() = default;

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

#include <Nazara/Physics2D/PhysConstraint2D.inl>

#endif // NAZARA_PHYSICS2D_PHYSCONSTRAINT2D_HPP
