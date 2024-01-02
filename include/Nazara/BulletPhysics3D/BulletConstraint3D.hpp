// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_BULLETCONSTRAINT3D_HPP
#define NAZARA_BULLETPHYSICS3D_BULLETCONSTRAINT3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/BulletPhysWorld3D.hpp>
#include <Nazara/BulletPhysics3D/BulletRigidBody3D.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>

class btTypedConstraint;

namespace Nz
{
	class BulletConstraint3D;

	using BulletConstraint3DHandle = ObjectHandle<BulletConstraint3D>;

	class NAZARA_BULLETPHYSICS3D_API BulletConstraint3D : public HandledObject<BulletConstraint3D>
	{
		public:
			BulletConstraint3D(const BulletConstraint3D&) = delete;
			BulletConstraint3D(BulletConstraint3D&& constraint) noexcept;
			virtual ~BulletConstraint3D();

			BulletRigidBody3D& GetBodyA();
			const BulletRigidBody3D& GetBodyA() const;
			BulletRigidBody3D& GetBodyB();
			const BulletRigidBody3D& GetBodyB() const;
			BulletPhysWorld3D& GetWorld();
			const BulletPhysWorld3D& GetWorld() const;

			inline bool IsBodyCollisionEnabled() const;
			bool IsSingleBody() const;

			BulletConstraint3D& operator=(const BulletConstraint3D&) = delete;
			BulletConstraint3D& operator=(BulletConstraint3D&& constraint) noexcept;

		protected:
			BulletConstraint3D(std::unique_ptr<btTypedConstraint> constraint, bool disableCollisions = false);

			template<typename T> T* GetConstraint();
			template<typename T> const T* GetConstraint() const;

		private:
			void Destroy();

			std::unique_ptr<btTypedConstraint> m_constraint;
			bool m_bodyCollisionEnabled;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletPivotConstraint3D : public BulletConstraint3D
	{
		public:
			BulletPivotConstraint3D(BulletRigidBody3D& first, const Vector3f& pivot);
			BulletPivotConstraint3D(BulletRigidBody3D& first, BulletRigidBody3D& second, const Vector3f& pivot, bool disableCollisions = false);
			BulletPivotConstraint3D(BulletRigidBody3D& first, BulletRigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, bool disableCollisions = false);
			~BulletPivotConstraint3D() = default;

			Vector3f GetFirstAnchor() const;
			Vector3f GetSecondAnchor() const;

			float GetDamping() const;
			float GetImpulseClamp() const;

			void SetFirstAnchor(const Vector3f& firstAnchor);
			void SetSecondAnchor(const Vector3f& secondAnchor);

			void SetDamping(float damping);
			void SetImpulseClamp(float impulseClamp);
	};
}

#include <Nazara/BulletPhysics3D/BulletConstraint3D.inl>

#endif // NAZARA_BULLETPHYSICS3D_BULLETCONSTRAINT3D_HPP
