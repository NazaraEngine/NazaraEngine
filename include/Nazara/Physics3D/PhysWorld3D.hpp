// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSWORLD3D_HPP
#define NAZARA_PHYSICS3D_PHYSWORLD3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Export.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <atomic>
#include <optional>
#include <vector>

namespace JPH
{
	class BodyID;
	class PhysicsSystem;
	class Shape;
}

namespace Nz
{
	class PhysBody3D;
	class PhysCharacter3D;
	class PhysCharacter3DImpl;
	class Collider3D;
	class PhysWorld3DStepListener;
	class RigidBody3D;

	class NAZARA_PHYSICS3D_API PhysWorld3D
	{
		friend PhysCharacter3D;
		friend RigidBody3D;

		public:
			struct PointCollisionInfo;
			struct RaycastHit;
			struct ShapeCollisionInfo;

			PhysWorld3D();
			PhysWorld3D(const PhysWorld3D&) = delete;
			PhysWorld3D(PhysWorld3D&& ph) = delete;
			~PhysWorld3D();

			bool CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback);
			bool CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback);
			bool CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback);

			UInt32 GetActiveBodyCount() const;
			Boxf GetBoundingBox() const;
			Vector3f GetGravity() const;
			std::size_t GetMaxStepCount() const;
			JPH::PhysicsSystem* GetPhysicsSystem();
			Time GetStepSize() const;

			inline bool IsBodyActive(UInt32 bodyIndex) const;
			inline bool IsBodyRegistered(UInt32 bodyIndex) const;

			bool RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback);
			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback);

			void RefreshBodies();

			inline void RegisterStepListener(PhysWorld3DStepListener* stepListener);

			void SetGravity(const Vector3f& gravity);
			void SetMaxStepCount(std::size_t maxStepCount);
			void SetStepSize(Time stepSize);

			bool Step(Time timestep);

			inline void UnregisterStepListener(PhysWorld3DStepListener* stepListener);

			PhysWorld3D& operator=(const PhysWorld3D&) = delete;
			PhysWorld3D& operator=(PhysWorld3D&&) = delete;

			struct PointCollisionInfo
			{
				PhysBody3D* hitBody = nullptr;
			};

			struct RaycastHit
			{
				float fraction;
				PhysBody3D* hitBody = nullptr;
				Vector3f hitNormal;
				Vector3f hitPosition;
			};

			struct ShapeCollisionInfo
			{
				PhysBody3D* hitBody = nullptr;
				Vector3f collisionPosition1;
				Vector3f collisionPosition2;
				Vector3f penetrationAxis;
				float penetrationDepth;
			};

		private:
			class BodyActivationListener;
			friend BodyActivationListener;

			class StepListener;
			friend StepListener;

			struct JoltWorld;

			std::shared_ptr<PhysCharacter3DImpl> GetDefaultCharacterImpl();
			const JPH::Shape* GetNullShape() const;

			void OnPreStep(float deltatime);

			void RegisterBody(const JPH::BodyID& bodyID, bool activate, bool removeFromDeactivationList);

			void UnregisterBody(const JPH::BodyID& bodyID, bool destroy, bool removeFromRegisterList);

			std::size_t m_maxStepCount;
			std::shared_ptr<PhysCharacter3DImpl> m_defaultCharacterImpl;
			std::unique_ptr<std::atomic_uint64_t[]> m_activeBodies;
			std::unique_ptr<std::uint64_t[]> m_registeredBodies;
			std::unique_ptr<JoltWorld> m_world;
			std::vector<PhysWorld3DStepListener*> m_stepListeners;
			Vector3f m_gravity;
			Time m_stepSize;
			Time m_timestepAccumulator;
	};
}

#include <Nazara/Physics3D/PhysWorld3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSWORLD3D_HPP
