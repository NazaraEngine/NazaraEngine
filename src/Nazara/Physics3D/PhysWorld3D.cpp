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

namespace DitchMeAsap
{
	using namespace JPH;

	// Layer that objects can be in, determines which other objects it can collide with
	// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
	// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
	// but only if you do collision testing).
	namespace Layers
	{
		static constexpr uint8 NON_MOVING = 0;
		static constexpr uint8 MOVING = 1;
		static constexpr uint8 NUM_LAYERS = 2;
	};

	/// Class that determines if two object layers can collide
	class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
		{
			switch (inObject1)
			{
			case Layers::NON_MOVING:
				return inObject2 == Layers::MOVING; // Non moving only collides with moving
			case Layers::MOVING:
				return true; // Moving collides with everything
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	};

	// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
	// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
	// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
	// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
	// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
	namespace BroadPhaseLayers
	{
		static constexpr BroadPhaseLayer NON_MOVING(0);
		static constexpr BroadPhaseLayer MOVING(1);
		static constexpr uint NUM_LAYERS(2);
	};

	// BroadPhaseLayerInterface implementation
	// This defines a mapping between object and broadphase layers.
	class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
	{
	public:
		BPLayerInterfaceImpl()
		{
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		}

		virtual uint GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::NUM_LAYERS;
		}

		virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
		{
			JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
			return mObjectToBroadPhase[inLayer];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
		{
			switch ((BroadPhaseLayer::Type)inLayer)
			{
			case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:   return "NON_MOVING";
			case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:       return "MOVING";
			default:                                                    JPH_ASSERT(false); return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
	};
	/// Class that determines if an object layer can collide with a broadphase layer
	class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
		{
			switch (inLayer1)
			{
			case Layers::NON_MOVING:
				return inLayer2 == BroadPhaseLayers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	};

	// An example contact listener
	/*class MyContactListener : public ContactListener
	{
	public:
		// See: ContactListener
		virtual ValidateResult  OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
		{
			cout << "Contact validate callback" << endl;

			// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
			return ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
		{
			cout << "A contact was added" << endl;
		}

		virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
		{
			cout << "A contact was persisted" << endl;
		}

		virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
		{
			cout << "A contact was removed" << endl;
		}
	};*/
}

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
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

					hitInfo.hitBody = BitCast<PhysBody3D*>(static_cast<std::uintptr_t>(body.GetUserData()));

					if (auto fractionOpt = m_callback(hitInfo))
					{
						float fraction = fractionOpt.value();
						if (fraction > 0.f)
						{
							m_didHit = true;
							UpdateEarlyOutFraction(fraction);
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

					hitInfo.hitBody = BitCast<PhysBody3D*>(static_cast<std::uintptr_t>(body.GetUserData()));

					if (auto fractionOpt = m_callback(hitInfo))
					{
						float fraction = fractionOpt.value();
						if (fraction > 0.f)
						{
							m_didHit = true;
							UpdateEarlyOutFraction(fraction);
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

					hitInfo.hitBody = BitCast<PhysBody3D*>(static_cast<std::uintptr_t>(body.GetUserData()));
					hitInfo.hitNormal = FromJolt(body.GetWorldSpaceSurfaceNormal(result.mSubShapeID2, ToJolt(hitInfo.hitPosition)));

					if (auto fractionOpt = m_callback(hitInfo))
					{
						float fraction = fractionOpt.value();
						if (fraction > 0.f)
						{
							m_didHit = true;
							UpdateEarlyOutFraction(fraction);
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

		JPH::TempAllocatorImpl tempAllocator;
		JPH::PhysicsSystem physicsSystem;
		BodySet pendingAdditionActivate;
		BodySet pendingAdditionNoActivate;
		BodySet pendingDeactivations;
		std::vector<JPH::BodyID> tempBodyIDVec;
		std::unique_ptr<JPH::SphereShape> nullShape;

		PhysWorld3D::BodyActivationListener bodyActivationListener;
		PhysWorld3D::StepListener stepListener;

		DitchMeAsap::BPLayerInterfaceImpl layerInterface;
		DitchMeAsap::ObjectLayerPairFilterImpl objectLayerFilter;
		DitchMeAsap::ObjectVsBroadPhaseLayerFilterImpl objectBroadphaseLayerFilter;

		JoltWorld(PhysWorld3D& world, JPH::uint tempAllocatorSize) :
		tempAllocator(tempAllocatorSize),
		bodyActivationListener(world),
		stepListener(world)
		{
		}

		JoltWorld(const JoltWorld&) = delete;
		JoltWorld(JoltWorld&&) = delete;

		JoltWorld& operator=(const JoltWorld&) = delete;
		JoltWorld& operator=(JoltWorld&&) = delete;
	};

	PhysWorld3D::PhysWorld3D() :
	m_maxStepCount(50),
	m_gravity(Vector3f::Zero()),
	m_stepSize(Time::TickDuration(120)),
	m_timestepAccumulator(Time::Zero())
	{
		m_world = std::make_unique<JoltWorld>(*this, 10 * 1024 * 1024);
		m_world->physicsSystem.Init(0xFFFF, 0, 0xFFFF, 10 * 1024, m_world->layerInterface, m_world->objectBroadphaseLayerFilter, m_world->objectLayerFilter);
		m_world->physicsSystem.SetBodyActivationListener(&m_world->bodyActivationListener);

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

	bool PhysWorld3D::CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		PointCallbackHitResult collector(m_world->physicsSystem.GetBodyLockInterface(), callback);
		m_world->physicsSystem.GetNarrowPhaseQuery().CollidePoint(ToJolt(point), collector);

		return collector.DidHit();
	}

	bool PhysWorld3D::CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback)
	{
		return CollisionQuery(collider, colliderTransform, Vector3f::Unit(), callback);
	}

	bool PhysWorld3D::CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		JPH::Shape* shape = collider.GetShapeSettings()->Create().Get();

		JPH::CollideShapeSettings collideShapeSettings;

		ShapeCallbackHitResult collector(m_world->physicsSystem.GetBodyLockInterface(), callback);
		m_world->physicsSystem.GetNarrowPhaseQuery().CollideShape(shape, ToJolt(colliderScale), ToJolt(colliderTransform), collideShapeSettings, JPH::Vec3::sZero(), collector);

		return collector.DidHit();
	}

	UInt32 PhysWorld3D::GetActiveBodyCount() const
	{
		return m_world->physicsSystem.GetNumActiveBodies(JPH::EBodyType::RigidBody);
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

	bool PhysWorld3D::RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		JPH::RRayCast rayCast;
		rayCast.mDirection = ToJolt(to - from);
		rayCast.mOrigin = ToJolt(from);

		JPH::RayCastSettings rayCastSettings;

		RaycastCallbackHitResult collector(m_world->physicsSystem.GetBodyLockInterface(), from, to, callback);
		m_world->physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, rayCastSettings, collector);

		return collector.DidHit();
	}

	bool PhysWorld3D::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback)
	{
		JPH::RRayCast rayCast;
		rayCast.mDirection = ToJolt(to - from);
		rayCast.mOrigin = ToJolt(from);

		JPH::RayCastSettings rayCastSettings;

		JPH::ClosestHitCollisionCollector<JPH::CastRayCollector> collector;
		m_world->physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, rayCastSettings, collector);

		if (!collector.HadHit())
			return false;

		JPH::BodyLockWrite lock(m_world->physicsSystem.GetBodyLockInterface(), collector.mHit.mBodyID);
		if (!lock.Succeeded())
			return false; //< body was destroyed before lock

		JPH::Body& body = lock.GetBody();

		RaycastHit hitInfo;
		hitInfo.fraction = collector.mHit.GetEarlyOutFraction();
		hitInfo.hitPosition = Lerp(from, to, hitInfo.fraction);
		hitInfo.hitBody = BitCast<PhysBody3D*>(static_cast<std::uintptr_t>(body.GetUserData()));
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
