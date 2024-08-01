// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/JoltHelper.hpp>
#include <Nazara/Physics3D/PhysCharacter3D.hpp>
#include <Nazara/Physics3D/PhysWorld3DStepListener.hpp>
#include <Nazara/Physics3D/Physics3D.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsStepListener.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <tsl/ordered_set.h>
#include <cassert>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		class BodyFilterBridge : public JPH::BodyFilter
		{
			public:
				BodyFilterBridge(const PhysBodyFilter3D* bodyFilter) :
				m_bodyFilter(bodyFilter)
				{
				}

				bool ShouldCollide(const JPH::BodyID& inBodyID) const override
				{
					return m_bodyFilter->ShouldCollide(inBodyID.GetIndex());
				}

				bool ShouldCollideLocked(const JPH::Body& inBody) const override
				{
					return m_bodyFilter->ShouldCollideLocked(*IntegerToPointer<PhysBody3D*>(inBody.GetUserData()));
				}

			private:
				const PhysBodyFilter3D* m_bodyFilter;
		};

		class BroadphaseLayerFilterBridge : public JPH::BroadPhaseLayerFilter
		{
			public:
				BroadphaseLayerFilterBridge(const PhysBroadphaseLayerFilter3D* broadphaseFilter) :
				m_broadphaseFilter(broadphaseFilter)
				{
				}

				bool ShouldCollide(JPH::BroadPhaseLayer inLayer) const override
				{
					return m_broadphaseFilter->ShouldCollide(inLayer.GetValue());
				}

			private:
				const PhysBroadphaseLayerFilter3D* m_broadphaseFilter;
		};
		
		class ObjectLayerFilterBridge : public JPH::ObjectLayerFilter
		{
			public:
				ObjectLayerFilterBridge(const PhysObjectLayerFilter3D* objectLayerFilter) :
				m_objectLayerFilter(objectLayerFilter)
				{
				}

				bool ShouldCollide(JPH::ObjectLayer inLayer) const override
				{
					return m_objectLayerFilter->ShouldCollide(inLayer);
				}

			private:
				const PhysObjectLayerFilter3D* m_objectLayerFilter;
		};
		
		class PointCallbackHitResult : public JPH::CollidePointCollector
		{
			public:
				PointCallbackHitResult(const JPH::BodyLockInterface& bodyLockInterface, const FunctionRef<std::optional<float>(const PhysWorld3D::PointCollisionInfo& hitInfo)>& callback) :
				m_bodyLockInterface(bodyLockInterface),
				m_callback(callback),
				m_didHit(false)
				{
				}

				void AddHit(const JPH::CollidePointResult& result) override
				{
					PhysWorld3D::PointCollisionInfo hitInfo;

					JPH::BodyLockWrite lock(m_bodyLockInterface, result.mBodyID);
					if (!lock.Succeeded())
						return; //< body was destroyed

					JPH::Body& body = lock.GetBody();

					hitInfo.hitBody = IntegerToPointer<PhysBody3D*>(body.GetUserData());

					if (auto fractionOpt = m_callback(hitInfo))
					{
						float fraction = fractionOpt.value();
						if (fraction > 0.f)
						{
							m_didHit = true;
							ResetEarlyOutFraction(fraction);
						}
						else
							ForceEarlyOut();
					}
				}

				bool DidHit() const
				{
					return m_didHit;
				}

			private:
				const JPH::BodyLockInterface& m_bodyLockInterface;
				const FunctionRef<std::optional<float>(const PhysWorld3D::PointCollisionInfo& hitInfo)>& m_callback;
				bool m_didHit;
		};

		class ShapeCallbackHitResult : public JPH::CollideShapeCollector
		{
			public:
				ShapeCallbackHitResult(const JPH::BodyLockInterface& bodyLockInterface, const FunctionRef<std::optional<float>(const PhysWorld3D::ShapeCollisionInfo& hitInfo)>& callback) :
				m_bodyLockInterface(bodyLockInterface),
				m_callback(callback),
				m_didHit(false)
				{
				}

				void AddHit(const JPH::CollideShapeResult& result) override
				{
					PhysWorld3D::ShapeCollisionInfo hitInfo;
					hitInfo.collisionPosition1 = FromJolt(result.mContactPointOn1);
					hitInfo.collisionPosition2 = FromJolt(result.mContactPointOn2);
					hitInfo.penetrationAxis = FromJolt(result.mPenetrationAxis);
					hitInfo.penetrationDepth = result.mPenetrationDepth;

					JPH::BodyLockWrite lock(m_bodyLockInterface, result.mBodyID2);
					if (!lock.Succeeded())
						return; //< body was destroyed

					JPH::Body& body = lock.GetBody();

					hitInfo.hitBody = IntegerToPointer<PhysBody3D*>(body.GetUserData());

					if (auto fractionOpt = m_callback(hitInfo))
					{
						float fraction = fractionOpt.value();
						if (fraction > 0.f)
						{
							m_didHit = true;
							ResetEarlyOutFraction(fraction);
						}
						else
							ForceEarlyOut();
					}
				}

				bool DidHit() const
				{
					return m_didHit;
				}

			private:
				const JPH::BodyLockInterface& m_bodyLockInterface;
				const FunctionRef<std::optional<float>(const PhysWorld3D::ShapeCollisionInfo& hitInfo)>& m_callback;
				bool m_didHit;
		};

		class RaycastCallbackHitResult : public JPH::CastRayCollector
		{
			public:
				RaycastCallbackHitResult(const JPH::BodyLockInterface& bodyLockInterface, const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const PhysWorld3D::RaycastHit& hitInfo)>& callback) :
				m_bodyLockInterface(bodyLockInterface),
				m_callback(callback),
				m_from(from),
				m_to(to),
				m_didHit(false)
				{
				}

				void AddHit(const JPH::RayCastResult& result) override
				{
					PhysWorld3D::RaycastHit hitInfo;
					hitInfo.fraction = result.mFraction;
					hitInfo.hitPosition = Lerp(m_from, m_to, result.mFraction);

					JPH::BodyLockWrite lock(m_bodyLockInterface, result.mBodyID);
					if (!lock.Succeeded())
						return; //< body was destroyed

					JPH::Body& body = lock.GetBody();

					hitInfo.hitBody = IntegerToPointer<PhysBody3D*>(body.GetUserData());
					hitInfo.hitNormal = FromJolt(body.GetWorldSpaceSurfaceNormal(result.mSubShapeID2, ToJolt(hitInfo.hitPosition)));

					if (auto fractionOpt = m_callback(hitInfo))
					{
						float fraction = fractionOpt.value();
						if (fraction > 0.f)
						{
							m_didHit = true;
							ResetEarlyOutFraction(fraction);
						}
						else
							ForceEarlyOut();
					}
				}

				bool DidHit() const
				{
					return m_didHit;
				}

			private:
				const JPH::BodyLockInterface& m_bodyLockInterface;
				const FunctionRef<std::optional<float>(const PhysWorld3D::RaycastHit& hitInfo)>& m_callback;
				Vector3f m_from;
				Vector3f m_to;
				bool m_didHit;
		};
	}

	class PhysWorld3D::BodyActivationListener : public JPH::BodyActivationListener
	{
		public:
			static constexpr UInt32 BodyPerBlock = 64;

			BodyActivationListener(PhysWorld3D& physWorld) :
			m_physWorld(physWorld)
			{
			}

			void OnBodyActivated(const JPH::BodyID& inBodyID, UInt64 /*inBodyUserData*/) override
			{
				UInt32 bodyIndex = inBodyID.GetIndex();
				UInt32 blockIndex = bodyIndex / 64;
				UInt32 localIndex = bodyIndex % 64;

				m_physWorld.m_activeBodies[blockIndex] |= UInt64(1u) << localIndex;
			}

			void OnBodyDeactivated(const JPH::BodyID& inBodyID, UInt64 /*inBodyUserData*/) override
			{
				UInt32 bodyIndex = inBodyID.GetIndex();
				UInt32 blockIndex = bodyIndex / 64;
				UInt32 localIndex = bodyIndex % 64;

				m_physWorld.m_activeBodies[blockIndex] &= ~(UInt64(1u) << localIndex);
			}

		private:
			PhysWorld3D& m_physWorld;
	};

	class PhysWorld3D::BroadphaseLayerInterfaceBridge : public JPH::BroadPhaseLayerInterface
	{
		public:
			BroadphaseLayerInterfaceBridge(std::unique_ptr<PhysBroadphaseLayerInterface3D> broadphaseLayerInterface) :
			m_broadphaseLayerInterface(std::move(broadphaseLayerInterface))
			{
			}

			JPH::uint GetNumBroadPhaseLayers() const override
			{
				return SafeCaster(m_broadphaseLayerInterface->GetBroadphaseLayerCount());
			}

			JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
			{
				return JPH::BroadPhaseLayer(m_broadphaseLayerInterface->GetBroadphaseLayer(inLayer));
			}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
			const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
			{
				return m_broadphaseLayerInterface->GetBroadphaseLayerName(inLayer.GetValue());
			}
#endif

		private:
			std::unique_ptr<PhysBroadphaseLayerInterface3D> m_broadphaseLayerInterface;
	};

	PhysWorld3D::ContactListener::~ContactListener() = default;

	class PhysWorld3D::ContactListenerBridge : public JPH::ContactListener
	{
		public:
			ContactListenerBridge(const JPH::BodyLockInterfaceNoLock& bodyLockInterface, std::unique_ptr<PhysWorld3D::ContactListener> contactListener) :
			m_contactListener(std::move(contactListener)),
			m_bodyLockInterface(bodyLockInterface)
			{
			}

			JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override
			{
				PhysBody3D* body1 = IntegerToPointer<PhysBody3D*>(inBody1.GetUserData());
				PhysBody3D* body2 = IntegerToPointer<PhysBody3D*>(inBody2.GetUserData());

				ShapeCollisionInfo shapeCollisionInfo;
				shapeCollisionInfo.collisionPosition1 = FromJolt(inCollisionResult.mContactPointOn1);
				shapeCollisionInfo.collisionPosition2 = FromJolt(inCollisionResult.mContactPointOn2);
				shapeCollisionInfo.penetrationAxis = FromJolt(inCollisionResult.mPenetrationAxis);
				shapeCollisionInfo.penetrationDepth = inCollisionResult.mPenetrationDepth;

				return static_cast<JPH::ValidateResult>(UnderlyingCast(m_contactListener->ValidateContact(body1, body2, FromJolt(inBaseOffset), shapeCollisionInfo)));
			}

			void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
			{
				PhysBody3D* body1 = IntegerToPointer<PhysBody3D*>(inBody1.GetUserData());
				PhysBody3D* body2 = IntegerToPointer<PhysBody3D*>(inBody2.GetUserData());

				m_contactListener->OnContactAdded(body1, body2);
			}

			void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
			{
				PhysBody3D* body1 = IntegerToPointer<PhysBody3D*>(inBody1.GetUserData());
				PhysBody3D* body2 = IntegerToPointer<PhysBody3D*>(inBody2.GetUserData());

				m_contactListener->OnContactPersisted(body1, body2);
			}

			void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
			{
				JPH::BodyLockRead lock1(m_bodyLockInterface, inSubShapePair.GetBody1ID());
				if (!lock1.Succeeded())
					return; //< body1 was destroyed

				JPH::BodyLockRead lock2(m_bodyLockInterface, inSubShapePair.GetBody2ID());
				if (!lock2.Succeeded())
					return; //< body2 was destroyed

				PhysBody3D* body1 = IntegerToPointer<PhysBody3D*>(lock1.GetBody().GetUserData());
				PhysBody3D* body2 = IntegerToPointer<PhysBody3D*>(lock2.GetBody().GetUserData());

				m_contactListener->OnContactRemoved(body1, body2);
			}

		private:
			std::unique_ptr<PhysWorld3D::ContactListener> m_contactListener;
			const JPH::BodyLockInterfaceNoLock& m_bodyLockInterface;
	};

	class PhysWorld3D::ObjectLayerPairFilterBridge : public JPH::ObjectLayerPairFilter
	{
		public:
			ObjectLayerPairFilterBridge(std::unique_ptr<PhysObjectLayerPairFilter3D> broadphaseLayerInterface) :
			m_objectLayerPairFilter(std::move(broadphaseLayerInterface))
			{
			}

			bool ShouldCollide(PhysObjectLayer3D layer1, PhysObjectLayer3D layer2) const override
			{
				return m_objectLayerPairFilter->ShouldCollide(layer1, layer2);
			}

		private:
			std::unique_ptr<PhysObjectLayerPairFilter3D> m_objectLayerPairFilter;
	};

	class PhysWorld3D::ObjectVsBroadphaseLayerFilterBridge : public JPH::ObjectVsBroadPhaseLayerFilter
	{
		public:
			ObjectVsBroadphaseLayerFilterBridge(std::unique_ptr<PhysObjectVsBroadphaseLayerFilter3D> broadphaseLayerInterface) :
			m_objectVsBroadphaseLayerFilter(std::move(broadphaseLayerInterface))
			{
			}

			bool ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const override
			{
				return m_objectVsBroadphaseLayerFilter->ShouldCollide(layer1, layer2.GetValue());
			}

		private:
			std::unique_ptr<PhysObjectVsBroadphaseLayerFilter3D> m_objectVsBroadphaseLayerFilter;
	};

	class PhysWorld3D::StepListener : public JPH::PhysicsStepListener
	{
		public:
			StepListener(PhysWorld3D& physWorld) :
			m_physWorld(physWorld)
			{
			}

			void OnStep(float inDeltaTime, JPH::PhysicsSystem& /*inPhysicsSystem*/) override
			{
				m_physWorld.OnPreStep(inDeltaTime);
			}

		private:
			PhysWorld3D& m_physWorld;
	};

	struct PhysWorld3D::JoltWorld
	{
		using BodySet = tsl::ordered_set<JPH::BodyID, std::hash<JPH::BodyID>, std::equal_to<JPH::BodyID>, std::allocator<JPH::BodyID>, std::vector<JPH::BodyID>>;

		JPH::PhysicsSystem physicsSystem;
		JPH::TempAllocatorImpl tempAllocator;
		BodySet pendingAdditionActivate;
		BodySet pendingAdditionNoActivate;
		BodySet pendingDeactivations;
		std::optional<ContactListenerBridge> contactListenerBridge;
		std::vector<JPH::BodyID> tempBodyIDVec;
		std::unique_ptr<JPH::SphereShape> nullShape;

		PhysWorld3D::BodyActivationListener bodyActivationListener;
		PhysWorld3D::BroadphaseLayerInterfaceBridge broadphaseLayerInterfaceBridge;
		PhysWorld3D::ObjectLayerPairFilterBridge objectLayerPairFilterBridge;
		PhysWorld3D::ObjectVsBroadphaseLayerFilterBridge objectVsBroadphaseLayerFilterBridge;
		PhysWorld3D::StepListener stepListener;

		JoltWorld(PhysWorld3D& world, std::unique_ptr<PhysBroadphaseLayerInterface3D> broadphaseLayerInterfaceImpl, std::unique_ptr<PhysObjectLayerPairFilter3D> objectLayerPairFilterImpl, std::unique_ptr<PhysObjectVsBroadphaseLayerFilter3D> objectVsBroadphaseLayerFilterImpl, JPH::uint tempAllocatorSize) :
		tempAllocator(tempAllocatorSize),
		bodyActivationListener(world),
		broadphaseLayerInterfaceBridge(std::move(broadphaseLayerInterfaceImpl)),
		objectLayerPairFilterBridge(std::move(objectLayerPairFilterImpl)),
		objectVsBroadphaseLayerFilterBridge(std::move(objectVsBroadphaseLayerFilterImpl)),
		stepListener(world)
		{
		}

		JoltWorld(const JoltWorld&) = delete;
		JoltWorld(JoltWorld&&) = delete;

		JoltWorld& operator=(const JoltWorld&) = delete;
		JoltWorld& operator=(JoltWorld&&) = delete;
	};

	PhysWorld3D::PhysWorld3D(Settings&& settings) :
	m_maxStepCount(settings.maxStepCount),
	m_stepSize(settings.stepSize),
	m_timestepAccumulator(Time::Zero())
	{
		m_world = std::make_unique<JoltWorld>(*this, std::move(settings.broadphaseLayerInterface), std::move(settings.objectLayerPairFilter), std::move(settings.objectVsBroadphaseLayerFilter), settings.tempAllocatorSize);
		m_world->physicsSystem.Init(settings.maxBodies, settings.numBodyMutexes, settings.maxBodyPairs, settings.maxContactConstraints, m_world->broadphaseLayerInterfaceBridge, m_world->objectVsBroadphaseLayerFilterBridge, m_world->objectLayerPairFilterBridge);
		m_world->physicsSystem.SetBodyActivationListener(&m_world->bodyActivationListener);
		m_world->physicsSystem.SetGravity(ToJolt(settings.gravity));

		if (settings.contactListener)
		{
			m_world->contactListenerBridge.emplace(m_world->physicsSystem.GetBodyLockInterfaceNoLock(), std::move(settings.contactListener));
			m_world->physicsSystem.SetContactListener(&*m_world->contactListenerBridge);
		}

		m_world->physicsSystem.AddStepListener(&m_world->stepListener);

		std::size_t blockCount = (m_world->physicsSystem.GetMaxBodies() - 1) / 64 + 1;

		m_activeBodies = std::make_unique<std::atomic_uint64_t[]>(blockCount);
		for (std::size_t i = 0; i < blockCount; ++i)
			m_activeBodies[i] = 0;

		m_registeredBodies = std::make_unique<std::uint64_t[]>(blockCount);
		for (std::size_t i = 0; i < blockCount; ++i)
			m_registeredBodies[i] = 0;
	}

	PhysWorld3D::~PhysWorld3D() = default;

	bool PhysWorld3D::CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		JPH::BodyFilter defaultBodyFilter;
		JPH::BroadPhaseLayerFilter defaultBroadphaseFilter;
		JPH::ObjectLayerFilter defaultObjectFilter;

		BodyFilterBridge bodyFilterBridge(bodyFilter);
		BroadphaseLayerFilterBridge broadphaseLayerFilterBridge(broadphaseFilter);
		ObjectLayerFilterBridge objectLayerFilterBridge(objectLayerFilter);

		PointCallbackHitResult collector(m_world->physicsSystem.GetBodyLockInterface(), callback);
		m_world->physicsSystem.GetNarrowPhaseQuery().CollidePoint(ToJolt(point), collector, (broadphaseFilter) ? broadphaseLayerFilterBridge : defaultBroadphaseFilter, (objectLayerFilter) ? objectLayerFilterBridge : defaultObjectFilter, (bodyFilter) ? bodyFilterBridge : defaultBodyFilter);

		return collector.DidHit();
	}

	bool PhysWorld3D::CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		return CollisionQuery(collider, colliderTransform, Vector3f::Unit(), callback, broadphaseFilter, objectLayerFilter, bodyFilter);
	}

	bool PhysWorld3D::CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		JPH::BodyFilter defaultBodyFilter;
		JPH::BroadPhaseLayerFilter defaultBroadphaseFilter;
		JPH::ObjectLayerFilter defaultObjectFilter;

		BodyFilterBridge bodyFilterBridge(bodyFilter);
		BroadphaseLayerFilterBridge broadphaseLayerFilterBridge(broadphaseFilter);
		ObjectLayerFilterBridge objectLayerFilterBridge(objectLayerFilter);

		JPH::Shape* shape = collider.GetShapeSettings()->Create().Get();

		JPH::CollideShapeSettings collideShapeSettings;

		ShapeCallbackHitResult collector(m_world->physicsSystem.GetBodyLockInterface(), callback);
		m_world->physicsSystem.GetNarrowPhaseQuery().CollideShape(shape, ToJolt(colliderScale), ToJolt(colliderTransform), collideShapeSettings, JPH::Vec3::sZero(), collector, (broadphaseFilter) ? broadphaseLayerFilterBridge : defaultBroadphaseFilter, (objectLayerFilter) ? objectLayerFilterBridge : defaultObjectFilter, (bodyFilter) ? bodyFilterBridge : defaultBodyFilter);

		return collector.DidHit();
	}

	UInt32 PhysWorld3D::GetActiveBodyCount() const
	{
		return m_world->physicsSystem.GetNumActiveBodies(JPH::EBodyType::RigidBody);
	}

	Boxf PhysWorld3D::GetBoundingBox() const
	{
		JPH::AABox bounds = m_world->physicsSystem.GetBounds();
		JPH::Vec3 pos = bounds.mMin;
		JPH::Vec3 size = bounds.GetSize();

		return Boxf(FromJolt(pos), FromJolt(size));
	}

	Vector3f PhysWorld3D::GetGravity() const
	{
		return FromJolt(m_world->physicsSystem.GetGravity());
	}

	std::size_t PhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	JPH::PhysicsSystem* PhysWorld3D::GetPhysicsSystem()
	{
		return &m_world->physicsSystem;
	}

	Time PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	bool PhysWorld3D::RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE
		
		JPH::BodyFilter defaultBodyFilter;
		JPH::BroadPhaseLayerFilter defaultBroadphaseFilter;
		JPH::ObjectLayerFilter defaultObjectFilter;

		BodyFilterBridge bodyFilterBridge(bodyFilter);
		BroadphaseLayerFilterBridge broadphaseLayerFilterBridge(broadphaseFilter);
		ObjectLayerFilterBridge objectLayerFilterBridge(objectLayerFilter);

		JPH::RRayCast rayCast;
		rayCast.mDirection = ToJolt(to - from);
		rayCast.mOrigin = ToJolt(from);

		JPH::RayCastSettings rayCastSettings;

		RaycastCallbackHitResult collector(m_world->physicsSystem.GetBodyLockInterface(), from, to, callback);
		m_world->physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, rayCastSettings, collector, (broadphaseFilter) ? broadphaseLayerFilterBridge : defaultBroadphaseFilter, (objectLayerFilter) ? objectLayerFilterBridge : defaultObjectFilter, (bodyFilter) ? bodyFilterBridge : defaultBodyFilter);

		return collector.DidHit();
	}

	bool PhysWorld3D::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		JPH::BodyFilter defaultBodyFilter;
		JPH::BroadPhaseLayerFilter defaultBroadphaseFilter;
		JPH::ObjectLayerFilter defaultObjectFilter;

		BodyFilterBridge bodyFilterBridge(bodyFilter);
		BroadphaseLayerFilterBridge broadphaseLayerFilterBridge(broadphaseFilter);
		ObjectLayerFilterBridge objectLayerFilterBridge(objectLayerFilter);

		JPH::RRayCast rayCast;
		rayCast.mDirection = ToJolt(to - from);
		rayCast.mOrigin = ToJolt(from);

		JPH::RayCastSettings rayCastSettings;

		JPH::ClosestHitCollisionCollector<JPH::CastRayCollector> collector;
		m_world->physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, rayCastSettings, collector, (broadphaseFilter) ? broadphaseLayerFilterBridge : defaultBroadphaseFilter, (objectLayerFilter) ? objectLayerFilterBridge : defaultObjectFilter, (bodyFilter) ? bodyFilterBridge : defaultBodyFilter);

		if (!collector.HadHit())
			return false;

		JPH::BodyLockWrite lock(m_world->physicsSystem.GetBodyLockInterface(), collector.mHit.mBodyID);
		if (!lock.Succeeded())
			return false; //< body was destroyed before lock

		JPH::Body& body = lock.GetBody();

		RaycastHit hitInfo;
		hitInfo.fraction = collector.mHit.GetEarlyOutFraction();
		hitInfo.hitPosition = Lerp(from, to, hitInfo.fraction);
		hitInfo.hitBody = IntegerToPointer<PhysBody3D*>(body.GetUserData());
		hitInfo.hitNormal = FromJolt(body.GetWorldSpaceSurfaceNormal(collector.mHit.mSubShapeID2, rayCast.GetPointOnRay(collector.mHit.GetEarlyOutFraction())));

		callback(hitInfo);
		return true;
	}

	void PhysWorld3D::RefreshBodies()
	{
		// Batch add bodies (keeps the broadphase efficient)
		JPH::BodyInterface& bodyInterface = m_world->physicsSystem.GetBodyInterfaceNoLock();
		auto AddBodies = [&](const JoltWorld::BodySet& bodies, JPH::EActivation activation)
		{
			for (const JPH::BodyID& bodyId : bodies)
			{
				UInt32 bodyIndex = bodyId.GetIndex();
				UInt32 blockIndex = bodyIndex / 64;
				UInt32 localIndex = bodyIndex % 64;

				m_registeredBodies[blockIndex] |= UInt64(1u) << localIndex;
			}

			if (bodies.size() == 1)
				bodyInterface.AddBody(bodies.front(), activation);
			else
			{
				m_world->tempBodyIDVec.resize(bodies.size());
				std::memcpy(&m_world->tempBodyIDVec[0], bodies.data(), bodies.size() * sizeof(JPH::BodyID));

				JPH::BodyInterface::AddState addState = bodyInterface.AddBodiesPrepare(m_world->tempBodyIDVec.data(), SafeCast<int>(m_world->tempBodyIDVec.size()));
				bodyInterface.AddBodiesFinalize(m_world->tempBodyIDVec.data(), SafeCast<int>(m_world->tempBodyIDVec.size()), addState, activation);
			}
		};

		// Handle pending register/unregister bodies
		if (!m_world->pendingAdditionActivate.empty())
		{
			AddBodies(m_world->pendingAdditionActivate, JPH::EActivation::Activate);
			m_world->pendingAdditionActivate.clear();
		}

		if (!m_world->pendingAdditionNoActivate.empty())
		{
			AddBodies(m_world->pendingAdditionNoActivate, JPH::EActivation::DontActivate);
			m_world->pendingAdditionNoActivate.clear();
		}

		if (!m_world->pendingDeactivations.empty())
		{
			bodyInterface.DeactivateBodies(m_world->pendingDeactivations.data(), SafeCast<int>(m_world->pendingDeactivations.size()));
			m_world->pendingDeactivations.clear();
		}
	}

	void PhysWorld3D::SetContactListener(std::unique_ptr<ContactListener> contactListener)
	{
		m_world->contactListenerBridge.emplace(m_world->physicsSystem.GetBodyLockInterfaceNoLock(), std::move(contactListener));
		m_world->physicsSystem.SetContactListener(&*m_world->contactListenerBridge);
	}

	void PhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_world->physicsSystem.SetGravity(ToJolt(gravity));
	}

	void PhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void PhysWorld3D::SetStepSize(Time stepSize)
	{
		m_stepSize = stepSize;
	}

	bool PhysWorld3D::Step(Time timestep)
	{
		m_timestepAccumulator += timestep;
		if (m_timestepAccumulator < m_stepSize)
			return false;

		RefreshBodies();

		JPH::JobSystem& jobSystem = Physics3D::Instance()->GetThreadPool();
		float stepSize = m_stepSize.AsSeconds<float>();

		std::size_t stepCount = 0;
		while (m_timestepAccumulator >= m_stepSize && stepCount < m_maxStepCount)
		{
			m_world->physicsSystem.Update(stepSize, 1, &m_world->tempAllocator, &jobSystem);

			for (PhysWorld3DStepListener* stepListener : m_stepListeners)
				stepListener->PostSimulate(stepSize);

			m_timestepAccumulator -= m_stepSize;
			stepCount++;
		}

		return true;
	}

	auto PhysWorld3D::BuildDefaultSettings() -> Settings
	{
		struct DefaultLayerInterface final : PhysBroadphaseLayerInterface3D
		{
			unsigned int GetBroadphaseLayerCount() const override
			{
				return 2;
			}

			PhysBroadphase3D GetBroadphaseLayer(PhysObjectLayer3D layer) const override
			{
				return SafeCaster(layer);
			}

			const char* GetBroadphaseLayerName(PhysBroadphase3D broadphaseLayer) const override
			{
				switch (broadphaseLayer)
				{
					case 0:  return "NON_MOVING";
					case 1:  return "MOVING";
					default: return "INVALID";
				}
			}
		};

		struct DefaultObjectLayerPairFilter final : PhysObjectLayerPairFilter3D
		{
			bool ShouldCollide(PhysObjectLayer3D object1, PhysObjectLayer3D object2) const override
			{
				switch (object1)
				{
					case 0:  return object2 == 1;
					case 1:  return true;
					default: return false;
				}
			}
		};

		struct DefaultObjectVsBroadphaseLayerFilter final : PhysObjectVsBroadphaseLayerFilter3D
		{
			bool ShouldCollide(PhysObjectLayer3D objectLayer, PhysBroadphase3D broadphaseLayer) const override
			{
				switch (objectLayer)
				{
					case 0:  return broadphaseLayer == 1;
					case 1:  return true;
					default: return false;
				}
			}
		};

		Settings settings;
		settings.broadphaseLayerInterface = std::make_unique<DefaultLayerInterface>();
		settings.objectLayerPairFilter = std::make_unique<DefaultObjectLayerPairFilter>();
		settings.objectVsBroadphaseLayerFilter = std::make_unique<DefaultObjectVsBroadphaseLayerFilter>();

		return settings;
	}

	void PhysWorld3D::RegisterBody(const JPH::BodyID& bodyID, bool activate, bool removeFromDeactivationList)
	{
		assert(removeFromDeactivationList || !m_world->pendingDeactivations.contains(bodyID));

		auto& activationSet = (activate) ? m_world->pendingAdditionActivate : m_world->pendingAdditionNoActivate;
		activationSet.insert(bodyID);

		if (removeFromDeactivationList)
		{
			auto& otherActivationSet = (activate) ? m_world->pendingAdditionNoActivate : m_world->pendingAdditionActivate;
			otherActivationSet.erase(bodyID);

			m_world->pendingDeactivations.erase(bodyID);
		}
	}

	void PhysWorld3D::UnregisterBody(const JPH::BodyID& bodyID, bool destroy, bool removeFromActivationList)
	{
		// Remove from list first as bodyID may be invalidated by destruction
		if (removeFromActivationList)
		{
			m_world->pendingAdditionActivate.erase(bodyID);
			m_world->pendingAdditionNoActivate.erase(bodyID);
		}

		if (destroy)
		{
			auto& bodyInterface = m_world->physicsSystem.GetBodyInterface();

			UInt32 bodyIndex = bodyID.GetIndex();
			if (IsBodyRegistered(bodyIndex))
			{
				UInt32 blockIndex = bodyIndex / 64;
				UInt32 localIndex = bodyIndex % 64;

				m_registeredBodies[blockIndex] &= ~(UInt64(1u) << localIndex);

				bodyInterface.RemoveBody(bodyID);
			}

			bodyInterface.DestroyBody(bodyID); //< this invalidate the bodyID reference!
		}
		else
			m_world->pendingDeactivations.insert(bodyID);
	}

	std::shared_ptr<PhysCharacter3DImpl> PhysWorld3D::GetDefaultCharacterImpl()
	{
		if (!m_defaultCharacterImpl)
			m_defaultCharacterImpl = std::make_shared<PhysCharacter3DImpl>();

		return m_defaultCharacterImpl;
	}

	const JPH::Shape* PhysWorld3D::GetNullShape() const
	{
		if (!m_world->nullShape)
		{
			m_world->nullShape = std::make_unique<JPH::SphereShape>(std::numeric_limits<float>::epsilon());
			m_world->nullShape->SetEmbedded();
		}

		return m_world->nullShape.get();
	}

	void PhysWorld3D::OnPreStep(float deltatime)
	{
		for (PhysWorld3DStepListener* stepListener : m_stepListeners)
			stepListener->PreSimulate(deltatime);
	}
}
