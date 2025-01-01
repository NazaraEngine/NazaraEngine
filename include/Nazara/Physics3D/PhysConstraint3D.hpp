// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSCONSTRAINT3D_HPP
#define NAZARA_PHYSICS3D_PHYSCONSTRAINT3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Physics3D/Export.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>

namespace JPH
{
	class TwoBodyConstraint;
}

namespace Nz
{
	class PhysConstraint3D;

	using PhysConstraint3DHandle = ObjectHandle<PhysConstraint3D>;

	class NAZARA_PHYSICS3D_API PhysConstraint3D : public HandledObject<PhysConstraint3D>
	{
		public:
			PhysConstraint3D(const PhysConstraint3D&) = delete;
			PhysConstraint3D(PhysConstraint3D&& constraint) noexcept;
			virtual ~PhysConstraint3D();

			RigidBody3D& GetBodyA();
			const RigidBody3D& GetBodyA() const;
			RigidBody3D& GetBodyB();
			const RigidBody3D& GetBodyB() const;
			PhysWorld3D& GetWorld();
			const PhysWorld3D& GetWorld() const;

			bool IsSingleBody() const;

			PhysConstraint3D& operator=(const PhysConstraint3D&) = delete;
			PhysConstraint3D& operator=(PhysConstraint3D&& constraint) noexcept;

		protected:
			PhysConstraint3D();

			template<typename T> T* GetConstraint();
			template<typename T> const T* GetConstraint() const;

			void SetupConstraint(std::unique_ptr<JPH::TwoBodyConstraint> constraint);

		private:
			void Destroy();

			std::unique_ptr<JPH::TwoBodyConstraint> m_constraint;
	};

	class NAZARA_PHYSICS3D_API PhysDistanceConstraint3D : public PhysConstraint3D
	{
		public:
			PhysDistanceConstraint3D(RigidBody3D& first, const Vector3f& pivot, float maxDist = -1.f, float minDist = -1.f);
			PhysDistanceConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& pivot, float maxDist = -1.f, float minDist = -1.f);
			PhysDistanceConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor, float maxDist = -1.f, float minDist = -1.f);
			~PhysDistanceConstraint3D() = default;

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

	class NAZARA_PHYSICS3D_API PhysPivotConstraint3D : public PhysConstraint3D
	{
		public:
			PhysPivotConstraint3D(RigidBody3D& first, const Vector3f& pivot);
			PhysPivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& pivot);
			PhysPivotConstraint3D(RigidBody3D& first, RigidBody3D& second, const Vector3f& firstAnchor, const Vector3f& secondAnchor);
			~PhysPivotConstraint3D() = default;

			Vector3f GetFirstAnchor() const;
			Vector3f GetSecondAnchor() const;

			void SetFirstAnchor(const Vector3f& firstAnchor);
			void SetSecondAnchor(const Vector3f& secondAnchor);
	};
}

#include <Nazara/Physics3D/PhysConstraint3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSCONSTRAINT3D_HPP
