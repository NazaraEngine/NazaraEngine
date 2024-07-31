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
#include <Nazara/Physics3D/PhysObjectLayer3D.hpp>
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
	class Collider3D;
	class PhysBody3D;
	class PhysCharacter3D;
	class PhysCharacter3DImpl;
	class PhysWorld3DStepListener;
	class RigidBody3D;

	class NAZARA_PHYSICS3D_API PhysWorld3D
	{
		friend PhysCharacter3D;
		friend RigidBody3D;

		public:
			struct BroadphaseLayerInterface;
			struct ContactListener;
			enum class ContactValidateResult;
			struct ObjectLayerPairFilter;
			struct ObjectVsBroadphaseLayerFilter;
			struct PointCollisionInfo;
			struct RaycastHit;
			struct Settings;
			struct ShapeCollisionInfo;

			PhysWorld3D(Settings&& settings = BuildDefaultSettings());
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

			static Settings BuildDefaultSettings();

			struct BroadphaseLayerInterface
			{
				virtual PhysBroadphase3D GetBroadphaseLayer(PhysObjectLayer3D layer) const = 0;
				virtual unsigned int GetBroadphaseLayerCount() const = 0;
				virtual const char* GetBroadphaseLayerName(PhysBroadphase3D broadphaseLayer) const = 0;
			};

			struct ContactListener
			{
				virtual ContactValidateResult ValidateContact(const PhysBody3D* body1, const PhysBody3D* body2, const Vector3f& baseOffset, const ShapeCollisionInfo& collisionResult) = 0;

				virtual void OnContactAdded(const PhysBody3D* body1, const PhysBody3D* body2) = 0; //< TODO: Add ContactManifold and ContactSettings
				virtual void OnContactPersisted(const PhysBody3D* body1, const PhysBody3D* body2) = 0; //< TODO: Add ContactManifold and ContactSettings
				virtual void OnContactRemoved(const PhysBody3D* body1, const PhysBody3D* body2) = 0; //< TODO: Add subshape id
			};

			enum class ContactValidateResult
			{
				AcceptAllContactsForThisBodyPair, ///< Accept this and any further contact points for this body pair
				AcceptContact,                    ///< Accept this contact only (and continue calling this callback for every contact manifold for the same body pair)
				RejectContact,                    ///< Reject this contact only (but process any other contact manifolds for the same body pair)
				RejectAllContactsForThisBodyPair  ///< Rejects this and any further contact points for this body pair
			};

			struct ObjectLayerPairFilter
			{
				virtual bool ShouldCollide(PhysObjectLayer3D object1, PhysObjectLayer3D object2) const = 0;
			};

			struct ObjectVsBroadphaseLayerFilter
			{
				virtual bool ShouldCollide(PhysObjectLayer3D objectLayer, PhysBroadphase3D broadphaseLayer) const = 0;
			};

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

			struct Settings
			{
				std::unique_ptr<BroadphaseLayerInterface> broadphaseLayerInterface; //< mandatory
				std::unique_ptr<ContactListener> contactListener;
				std::unique_ptr<ObjectLayerPairFilter> objectLayerPairFilter; //< mandatory
				std::unique_ptr<ObjectVsBroadphaseLayerFilter> objectVsBroadphaseLayerFilter; //< mandatory
				Time stepDuration = Time::TickDuration(120);
				Vector3f gravity = Vector3f::Zero();
				unsigned int maxStepCount = 50;
				unsigned int maxBodies = 0xFFFF;
				unsigned int maxBodyPairs = 0xFFFF;
				unsigned int maxContactConstraints = 10 * 1024;
				unsigned int numBodyMutexes = 0; //< 0 = auto-determined
				unsigned int tempAllocatorSize = 10 * 1024 * 1024;
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

			class BroadphaseLayerInterfaceBridge;
			friend BroadphaseLayerInterfaceBridge;

			class ContactListenerBridge;
			friend ContactListenerBridge;

			class ObjectLayerPairFilterBridge;
			friend ObjectLayerPairFilterBridge;

			class ObjectVsBroadphaseLayerFilterBridge;
			friend ObjectVsBroadphaseLayerFilterBridge;

			struct JoltWorld;

			class StepListener;
			friend StepListener;

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
			Time m_stepDuration;
			Time m_timestepAccumulator;
	};
}

#include <Nazara/Physics3D/PhysWorld3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSWORLD3D_HPP
