// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_CONSTRAINT3D_HPP
#define NAZARA_PHYSICS3D_CONSTRAINT3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>

class btTypedConstraint;

namespace Nz
{
	class Constraint3D;

	using Constraint3DHandle = ObjectHandle<Constraint3D>;

	class NAZARA_PHYSICS3D_API Constraint3D : public HandledObject<Constraint3D>
	{
		public:
			Constraint3D(const Constraint3D&) = delete;
			Constraint3D(Constraint3D&& constraint) noexcept;
			virtual ~Constraint3D();

			RigidBody3D& GetBodyA();
			const RigidBody3D& GetBodyA() const;
			RigidBody3D& GetBodyB();
			const RigidBody3D& GetBodyB() const;
			PhysWorld3D& GetWorld();
			const PhysWorld3D& GetWorld() const;

			inline bool IsBodyCollisionEnabled() const;
			bool IsSingleBody() const;

			Constraint3D& operator=(const Constraint3D&) = delete;
			Constraint3D& operator=(Constraint3D&& constraint) noexcept;

		protected:
			Constraint3D(std::unique_ptr<btTypedConstraint> constraint, bool disableCollisions = false);

			template<typename T> T* GetConstraint();
			template<typename T> const T* GetConstraint() const;

		private:
			std::unique_ptr<btTypedConstraint> m_constraint;
			bool m_bodyCollisionEnabled;
	};

	class NAZARA_PHYSICS3D_API PivotConstraint3D : public Constraint3D
	{
		public:
			PivotConstraint3D(RigidBody3D& first, const Vector3f& pivot);
			PivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& pivot, bool disableCollisions = false);
			PivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, bool disableCollisions = false);
			~PivotConstraint3D() = default;

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

#include <Nazara/Physics3D/Constraint3D.inl>

#endif // NAZARA_PHYSICS3D_CONSTRAINT3D_HPP
