// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTCONSTRAINT3D_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTCONSTRAINT3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>

namespace JPH
{
	class TwoBodyConstraint;
}

namespace Nz
{
	class JoltConstraint3D;

	using JoltConstraint3DHandle = ObjectHandle<JoltConstraint3D>;

	class NAZARA_JOLTPHYSICS3D_API JoltConstraint3D : public HandledObject<JoltConstraint3D>
	{
		public:
			JoltConstraint3D(const JoltConstraint3D&) = delete;
			JoltConstraint3D(JoltConstraint3D&& constraint) noexcept;
			virtual ~JoltConstraint3D();

			JoltRigidBody3D& GetBodyA();
			const JoltRigidBody3D& GetBodyA() const;
			JoltRigidBody3D& GetBodyB();
			const JoltRigidBody3D& GetBodyB() const;
			JoltPhysWorld3D& GetWorld();
			const JoltPhysWorld3D& GetWorld() const;

			bool IsSingleBody() const;

			JoltConstraint3D& operator=(const JoltConstraint3D&) = delete;
			JoltConstraint3D& operator=(JoltConstraint3D&& constraint) noexcept;

		protected:
			JoltConstraint3D();

			template<typename T> T* GetConstraint();
			template<typename T> const T* GetConstraint() const;

			void SetupConstraint(std::unique_ptr<JPH::TwoBodyConstraint> constraint);

		private:
			void Destroy();

			std::unique_ptr<JPH::TwoBodyConstraint> m_constraint;
	};

	class NAZARA_JOLTPHYSICS3D_API JoltDistanceConstraint3D : public JoltConstraint3D
	{
		public:
			JoltDistanceConstraint3D(JoltRigidBody3D& first, const Vector3f& pivot, float maxDist = -1.f, float minDist = -1.f);
			JoltDistanceConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& pivot, float maxDist = -1.f, float minDist = -1.f);
			JoltDistanceConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, float maxDist = -1.f, float minDist = -1.f);
			~JoltDistanceConstraint3D() = default;

			float GetDamping() const;
			float GetFrequency() const;
			float GetMaxDistance() const;
			float GetMinDistance() const;

			void SetDamping(float damping);
			void SetDistance(float minDist, float maxDist);
			void SetFrequency(float frequency);
			void SetMaxDistance(float maxDist);
			void SetMinDistance(float minDist);
	};

	class NAZARA_JOLTPHYSICS3D_API JoltPivotConstraint3D : public JoltConstraint3D
	{
		public:
			JoltPivotConstraint3D(JoltRigidBody3D& first, const Vector3f& pivot);
			JoltPivotConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& pivot);
			JoltPivotConstraint3D(JoltRigidBody3D& first, JoltRigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor);
			~JoltPivotConstraint3D() = default;

			Vector3f GetFirstAnchor() const;
			Vector3f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector3f& firstAnchor);
			void SetSecondAnchor(const Vector3f& secondAnchor);
	};
}

#include <Nazara/JoltPhysics3D/JoltConstraint3D.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTCONSTRAINT3D_HPP
