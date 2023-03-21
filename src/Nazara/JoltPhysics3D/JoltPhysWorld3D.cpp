// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Nazara/JoltPhysics3D/JoltCharacter.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysics3D.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <cassert>
#include <iostream>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace DitchMeAsap
{
	using namespace JPH;
	using std::cout;
	using std::endl;

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
		virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
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

		virtual uint					GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::NUM_LAYERS;
		}

		virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
		{
			JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
			return mObjectToBroadPhase[inLayer];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
		{
			switch ((BroadPhaseLayer::Type)inLayer)
			{
			case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
			case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
			default:													JPH_ASSERT(false); return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
	};
	/// Class that determines if an object layer can collide with a broadphase layer
	class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
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
	class MyContactListener : public ContactListener
	{
	public:
		// See: ContactListener
		virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
		{
			cout << "Contact validate callback" << endl;

			// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
			return ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void			OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
		{
			cout << "A contact was added" << endl;
		}

		virtual void			OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
		{
			cout << "A contact was persisted" << endl;
		}

		virtual void			OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
		{
			cout << "A contact was removed" << endl;
		}
	};
}

namespace Nz
{
	class JoltPhysWorld3D::BodyActivationListener : public JPH::BodyActivationListener
	{
		public:
			static constexpr UInt32 BodyPerBlock = 64;

			BodyActivationListener(JoltPhysWorld3D& physWorld) :
			m_physWorld(physWorld)
			{
			}

			void OnBodyActivated(const JPH::BodyID& inBodyID, UInt64 inBodyUserData) override
			{
				UInt32 bodyIndex = inBodyID.GetIndex();
				UInt32 blockIndex = bodyIndex / 64;
				UInt32 localIndex = bodyIndex % 64;

				m_physWorld.m_activeBodies[blockIndex] |= UInt64(1u) << localIndex;
			}

			void OnBodyDeactivated(const JPH::BodyID& inBodyID, UInt64 inBodyUserData) override
			{
				UInt32 bodyIndex = inBodyID.GetIndex();
				UInt32 blockIndex = bodyIndex / 64;
				UInt32 localIndex = bodyIndex % 64;

				m_physWorld.m_activeBodies[blockIndex] &= ~(UInt64(1u) << localIndex);
			}

		private:
			JoltPhysWorld3D& m_physWorld;
	};

	struct JoltPhysWorld3D::JoltWorld
	{
		JPH::TempAllocatorImpl tempAllocator;
		JPH::PhysicsSystem physicsSystem;

		JoltPhysWorld3D::BodyActivationListener bodyActivationListener;

		DitchMeAsap::BPLayerInterfaceImpl layerInterface;
		DitchMeAsap::ObjectLayerPairFilterImpl objectLayerFilter;
		DitchMeAsap::ObjectVsBroadPhaseLayerFilterImpl objectBroadphaseLayerFilter;

		JoltWorld(JoltPhysWorld3D& world, JPH::uint tempAllocatorSize) :
		tempAllocator(tempAllocatorSize),
		bodyActivationListener(world)
		{
		}

		JoltWorld(const JoltWorld&) = delete;
		JoltWorld(JoltWorld&&) = delete;

		JoltWorld& operator=(const JoltWorld&) = delete;
		JoltWorld& operator=(JoltWorld&&) = delete;
	};

	JoltPhysWorld3D::JoltPhysWorld3D() :
	m_maxStepCount(50),
	m_gravity(Vector3f::Zero()),
	m_stepSize(Time::TickDuration(120)),
	m_timestepAccumulator(Time::Zero())
	{
		m_world = std::make_unique<JoltWorld>(*this, 10 * 1024 * 1024);
		m_world->physicsSystem.Init(0xFFFF, 0, 0xFFFF, 10 * 1024, m_world->layerInterface, m_world->objectBroadphaseLayerFilter, m_world->objectLayerFilter);
		m_world->physicsSystem.SetBodyActivationListener(&m_world->bodyActivationListener);

		std::size_t blockCount = (m_world->physicsSystem.GetMaxBodies() - 1) / 64 + 1;
		m_activeBodies = std::make_unique<std::atomic_uint64_t[]>(blockCount);
		for (std::size_t i = 0; i < blockCount; ++i)
			m_activeBodies[i] = 0;
	}

	JoltPhysWorld3D::~JoltPhysWorld3D() = default;

	UInt32 JoltPhysWorld3D::GetActiveBodyCount() const
	{
		return m_world->physicsSystem.GetNumActiveBodies();
	}

	Vector3f JoltPhysWorld3D::GetGravity() const
	{
		return FromJolt(m_world->physicsSystem.GetGravity());
	}

	std::size_t JoltPhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	JPH::PhysicsSystem* JoltPhysWorld3D::GetPhysicsSystem()
	{
		return &m_world->physicsSystem;
	}

	Time JoltPhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	bool JoltPhysWorld3D::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo)
	{
		return false;
		/*
		btCollisionWorld::ClosestRayResultCallback callback(ToBullet(from), ToBullet(to));
		m_world->dynamicWorld.rayTest(ToBullet(from), ToBullet(to), callback);

		if (!callback.hasHit())
			return false;

		if (hitInfo)
		{
			hitInfo->fraction = callback.m_closestHitFraction;
			hitInfo->hitNormal = FromBullet(callback.m_hitNormalWorld);
			hitInfo->hitPosition = FromBullet(callback.m_hitPointWorld);

			if (const btRigidBody* body = btRigidBody::upcast(callback.m_collisionObject))
				hitInfo->hitBody = static_cast<BulletRigidBody3D*>(body->getUserPointer());
		}

		return true;*/
	}

	void JoltPhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_world->physicsSystem.SetGravity(ToJolt(gravity));
	}

	void JoltPhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void JoltPhysWorld3D::SetStepSize(Time stepSize)
	{
		m_stepSize = stepSize;
	}

	void JoltPhysWorld3D::Step(Time timestep)
	{
		JPH::JobSystem& jobSystem = JoltPhysics3D::Instance()->GetThreadPool();

		m_timestepAccumulator += timestep;

		float stepSize = m_stepSize.AsSeconds<float>();

		static bool firstStep = true;
		if (firstStep)
		{
			m_world->physicsSystem.OptimizeBroadPhase();
			firstStep = false;
		}

		std::size_t stepCount = 0;
		while (m_timestepAccumulator >= m_stepSize && stepCount < m_maxStepCount)
		{
			m_world->physicsSystem.Update(stepSize, 1, 1, &m_world->tempAllocator, &jobSystem);

			for (JoltCharacter* character : m_characters)
				character->PostSimulate();

			m_timestepAccumulator -= m_stepSize;
			stepCount++;
		}
	}
}
