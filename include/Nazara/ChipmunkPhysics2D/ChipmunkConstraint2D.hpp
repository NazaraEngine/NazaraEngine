// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKCONSTRAINT2D_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKCONSTRAINT2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkPhysWorld2D.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkRigidBody2D.hpp>
#include <Nazara/ChipmunkPhysics2D/Config.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Math/Angle.hpp>
#include <NazaraUtils/MovablePtr.hpp>

struct cpConstraint;

namespace Nz
{
	class ChipmunkConstraint2D;

	using ChipmunkConstraint2DHandle = ObjectHandle<ChipmunkConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkConstraint2D : public HandledObject<ChipmunkConstraint2D>
	{
		public:
			ChipmunkConstraint2D(const ChipmunkConstraint2D&) = delete;
			ChipmunkConstraint2D(ChipmunkConstraint2D&& constraint) noexcept;
			virtual ~ChipmunkConstraint2D();

			void EnableBodyCollision(bool enable);

			ChipmunkRigidBody2D& GetBodyA();
			const ChipmunkRigidBody2D& GetBodyA() const;
			ChipmunkRigidBody2D& GetBodyB();
			const ChipmunkRigidBody2D& GetBodyB() const;
			float GetErrorBias() const;
			float GetLastImpulse() const;
			float GetMaxBias() const;
			float GetMaxForce() const;
			ChipmunkPhysWorld2D& GetWorld();
			const ChipmunkPhysWorld2D& GetWorld() const;

			bool IsBodyCollisionEnabled() const;
			bool IsSingleBody() const;

			void SetErrorBias(float bias);
			void SetMaxBias(float bias);
			void SetMaxForce(float force);

			ChipmunkConstraint2D& operator=(const ChipmunkConstraint2D&) = delete;
			ChipmunkConstraint2D& operator=(ChipmunkConstraint2D&& constraint) noexcept;

		protected:
			ChipmunkConstraint2D(ChipmunkPhysWorld2D* world, cpConstraint* constraint);

			MovablePtr<cpConstraint> m_constraint;

		private:
			void Destroy();
	};

	class ChipmunkDampedSpringConstraint2D;

	using ChipmunkDampedSpringConstraint2DHandle = ObjectHandle<ChipmunkDampedSpringConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkDampedSpringConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkDampedSpringConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float restLength, float stiffness, float damping);
			~ChipmunkDampedSpringConstraint2D() = default;

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

	class ChipmunkDampedRotarySpringConstraint2D;

	using ChipmunkDampedRotarySpringConstraint2DHandle = ObjectHandle<ChipmunkDampedRotarySpringConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkDampedRotarySpringConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkDampedRotarySpringConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const RadianAnglef& restAngle, float stiffness, float damping);
			~ChipmunkDampedRotarySpringConstraint2D() = default;

			float GetDamping() const;
			RadianAnglef GetRestAngle() const;
			float GetStiffness() const;

			void SetDamping(float newDamping);
			void SetRestAngle(const RadianAnglef& newAngle);
			void SetStiffness(float newStiffness);
	};

	class ChipmunkGearConstraint2D;

	using ChipmunkGearConstraint2DHandle = ObjectHandle<ChipmunkGearConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkGearConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkGearConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, float phase, float ratio);
			~ChipmunkGearConstraint2D() = default;

			float GetPhase() const;
			float GetRatio() const;

			void SetPhase(float phase);
			void SetRatio(float ratio);
	};

	class ChipmunkMotorConstraint2D;

	using ChipmunkMotorConstraint2DHandle = ObjectHandle<ChipmunkMotorConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkMotorConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkMotorConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, float rate);
			~ChipmunkMotorConstraint2D() = default;

			float GetRate() const;
			void SetRate(float rate);
	};

	class ChipmunkPinConstraint2D;

	using ChipmunkPinConstraint2DHandle = ObjectHandle<ChipmunkPinConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkPinConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkPinConstraint2D(ChipmunkRigidBody2D& body, const Vector2f& anchor);
			ChipmunkPinConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor);
			~ChipmunkPinConstraint2D() = default;

			float GetDistance() const;
			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetDistance(float newDistance);
			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class ChipmunkPivotConstraint2D;

	using ChipmunkPivotConstraint2DHandle = ObjectHandle<ChipmunkPivotConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkPivotConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkPivotConstraint2D(ChipmunkRigidBody2D& body, const Vector2f& anchor);
			ChipmunkPivotConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& anchor);
			ChipmunkPivotConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor);
			~ChipmunkPivotConstraint2D() = default;

			Vector2f GetFirstAnchor() const;
			Vector2f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector2f& firstAnchor);
			void SetSecondAnchor(const Vector2f& firstAnchor);
	};

	class ChipmunkRatchetConstraint2D;

	using ChipmunkRatchetConstraint2DHandle = ObjectHandle<ChipmunkRatchetConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkRatchetConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkRatchetConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, float phase, float ratchet);
			~ChipmunkRatchetConstraint2D() = default;

			RadianAnglef GetAngle() const;
			float GetPhase() const;
			float GetRatchet() const;

			void SetAngle(const RadianAnglef& angle);
			void SetPhase(float phase);
			void SetRatchet(float ratchet);
	};

	class ChipmunkRotaryLimitConstraint2D;

	using ChipmunkRotaryLimitConstraint2DHandle = ObjectHandle<ChipmunkRotaryLimitConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkRotaryLimitConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkRotaryLimitConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const RadianAnglef& minAngle, const RadianAnglef& maxAngle);
			~ChipmunkRotaryLimitConstraint2D() = default;

			RadianAnglef GetMaxAngle() const;
			RadianAnglef GetMinAngle() const;

			void SetMaxAngle(const RadianAnglef& maxAngle);
			void SetMinAngle(const RadianAnglef& minAngle);
	};

	class ChipmunkSlideConstraint2D;

	using ChipmunkSlideConstraint2DHandle = ObjectHandle<ChipmunkSlideConstraint2D>;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkSlideConstraint2D : public ChipmunkConstraint2D
	{
		public:
			ChipmunkSlideConstraint2D(ChipmunkRigidBody2D& first, ChipmunkRigidBody2D& second, const Vector2f& firstAnchor, const Vector2f& secondAnchor, float min, float max);
			~ChipmunkSlideConstraint2D() = default;

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

#include <Nazara/ChipmunkPhysics2D/ChipmunkConstraint2D.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKCONSTRAINT2D_HPP
