// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Systems/PhysicsSystem2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NazaraSDK/World.hpp>
#include <NazaraSDK/Components/CollisionComponent2D.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Components/PhysicsComponent2D.hpp>
#include <NazaraSDK/Components/PhysicsComponent3D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::PhysicsSystem2D
	* \brief NDK class that represents a two-dimensional physics system
	*
	* \remark This system is enabled if the entity has the trait: NodeComponent and any of these two: CollisionComponent3D or PhysicsComponent3D
	* \remark Static objects do not have a velocity specified by the physical engine
	*/

	/*!
	* \brief Constructs an PhysicsSystem object by default
	*/

	PhysicsSystem2D::PhysicsSystem2D()
	{
		Requires<NodeComponent>();
		RequiresAny<CollisionComponent2D, PhysicsComponent2D>();
		Excludes<PhysicsComponent3D>();
	}

	void PhysicsSystem2D::CreatePhysWorld() const
	{
		NazaraAssert(!m_physWorld, "Physics world should not be created twice");

		m_physWorld = std::make_unique<Nz::PhysWorld2D>();
	}

	void PhysicsSystem2D::DebugDraw(const DebugDrawOptions& options, bool drawShapes, bool drawConstraints, bool drawCollisions)
	{
		Nz::PhysWorld2D::DebugDrawOptions worldOptions{ options.constraintColor, options.collisionPointColor, options.shapeOutlineColor };

		if (options.colorCallback)
		{
			worldOptions.colorCallback = [&options, this](Nz::RigidBody2D& body, std::size_t shapeIndex, void* userdata)
			{
				return options.colorCallback(GetEntityFromBody(body), shapeIndex, userdata);
			};
		}

		worldOptions.circleCallback = options.circleCallback;
		worldOptions.dotCallback = options.dotCallback;
		worldOptions.polygonCallback = options.polygonCallback;
		worldOptions.segmentCallback = options.segmentCallback;
		worldOptions.thickSegmentCallback = options.thickSegmentCallback;

		worldOptions.userdata = options.userdata;

		GetPhysWorld().DebugDraw(worldOptions, drawShapes, drawConstraints, drawCollisions);
	}

	const EntityHandle& PhysicsSystem2D::GetEntityFromBody(const Nz::RigidBody2D& body) const
	{
		auto entityId = static_cast<EntityId>(reinterpret_cast<std::uintptr_t>(body.GetUserdata()));

		auto& world = GetWorld();

		NazaraAssert(world.IsEntityIdValid(entityId), "All Bodies of this world must be part of the physics world by using PhysicsComponent");

		return world.GetEntity(entityId);
	}

	bool PhysicsSystem2D::NearestBodyQuery(const Nz::Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, EntityHandle* nearestBody)
	{
		Nz::RigidBody2D* body;
		bool res = GetPhysWorld().NearestBodyQuery(from, maxDistance, collisionGroup, categoryMask, collisionMask, &body);

		(*nearestBody) = GetEntityFromBody(*body);

		return res;
	}

	bool PhysicsSystem2D::NearestBodyQuery(const Nz::Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, NearestQueryResult* result)
	{
		Nz::PhysWorld2D::NearestQueryResult queryResult;
		if (GetPhysWorld().NearestBodyQuery(from, maxDistance, collisionGroup, categoryMask, collisionMask, &queryResult))
		{
			result->nearestBody = GetEntityFromBody(*queryResult.nearestBody);
			result->closestPoint = std::move(queryResult.closestPoint);
			result->fraction = std::move(queryResult.fraction);
			result->distance = queryResult.distance;

			return true;
		}
		else
			return false;
	}

	void PhysicsSystem2D::RaycastQuery(const Nz::Vector2f & from, const Nz::Vector2f & to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, const std::function<void(const RaycastHit&)>& callback)
	{
		return GetPhysWorld().RaycastQuery(from, to, radius, collisionGroup, categoryMask, collisionMask, [this, &callback](const Nz::PhysWorld2D::RaycastHit& hitInfo)
		{
			callback({
				GetEntityFromBody(*hitInfo.nearestBody),
				hitInfo.hitPos,
				hitInfo.hitNormal,
				hitInfo.fraction
			});
		});
	}

	bool PhysicsSystem2D::RaycastQuery(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<RaycastHit>* hitInfos)
	{
		std::vector<Nz::PhysWorld2D::RaycastHit> queryResult;
		bool res = GetPhysWorld().RaycastQuery(from, to, radius, collisionGroup, categoryMask, collisionMask, &queryResult);

		for (auto& hitResult : queryResult)
		{
			hitInfos->push_back({
				GetEntityFromBody(*hitResult.nearestBody),
				std::move(hitResult.hitPos),
				std::move(hitResult.hitNormal),
				hitResult.fraction
			});
		}

		return res;
	}

	bool PhysicsSystem2D::RaycastQueryFirst(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, RaycastHit* hitInfo)
	{
		Nz::PhysWorld2D::RaycastHit queryResult;
		if (GetPhysWorld().RaycastQueryFirst(from, to, radius, collisionGroup, categoryMask, collisionMask, &queryResult))
		{
			hitInfo->body = GetEntityFromBody(*queryResult.nearestBody);
			hitInfo->hitPos = std::move(queryResult.hitPos);
			hitInfo->hitNormal = std::move(queryResult.hitNormal);
			hitInfo->fraction = queryResult.fraction;

			return true;
		}
		else
			return false;
	}

	void PhysicsSystem2D::RegionQuery(const Nz::Rectf& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, const std::function<void(const EntityHandle&)>& callback)
	{
		return GetPhysWorld().RegionQuery(boundingBox, collisionGroup, categoryMask, collisionMask, [this, &callback](Nz::RigidBody2D* body)
		{
			callback(GetEntityFromBody(*body));
		});
	}

	void PhysicsSystem2D::RegionQuery(const Nz::Rectf& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<EntityHandle>* bodies)
	{
		std::vector<Nz::RigidBody2D*> queryResult;
		GetPhysWorld().RegionQuery(boundingBox, collisionGroup, categoryMask, collisionMask, &queryResult);

		for (auto& body : queryResult)
		{
			bodies->emplace_back(GetEntityFromBody(*body));
		}
	}

	/*!
	* \brief Operation to perform when entity is validated for the system
	*
	* \param entity Pointer to the entity
	* \param justAdded Is the entity newly added
	*/

	void PhysicsSystem2D::OnEntityValidation(Entity* entity, bool justAdded)
	{
		if (entity->HasComponent<PhysicsComponent2D>())
		{
			if (entity->GetComponent<PhysicsComponent2D>().IsNodeSynchronizationEnabled())
				m_dynamicObjects.Insert(entity);
			else
				m_dynamicObjects.Remove(entity);

			m_staticObjects.Remove(entity);
		}
		else
		{
			m_dynamicObjects.Remove(entity);
			m_staticObjects.Insert(entity);

			// If entities just got added to the system, teleport them to their NodeComponent position/rotation
			// This will prevent the physics engine to mess with the scene while correcting position/rotation
			if (justAdded)
			{
				auto& collision = entity->GetComponent<CollisionComponent2D>();
				auto& node = entity->GetComponent<NodeComponent>();

				Nz::RigidBody2D* physObj = collision.GetStaticBody();
				physObj->SetPosition(Nz::Vector2f(node.GetPosition(Nz::CoordSys_Global)));
				//physObj->SetRotation(node.GetRotation());
			}
		}

		if (!m_physWorld)
			CreatePhysWorld();
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void PhysicsSystem2D::OnUpdate(float elapsedTime)
	{
		if (!m_physWorld)
			return;

		m_physWorld->Step(elapsedTime);

		for (const Ndk::EntityHandle& entity : m_dynamicObjects)
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			PhysicsComponent2D& phys = entity->GetComponent<PhysicsComponent2D>();

			Nz::RigidBody2D* body = phys.GetRigidBody();
			node.SetRotation(body->GetRotation(), Nz::CoordSys_Global);
			node.SetPosition(Nz::Vector3f(body->GetPosition(), node.GetPosition(Nz::CoordSys_Global).z), Nz::CoordSys_Global);
		}

		float invElapsedTime = 1.f / elapsedTime;
		for (const Ndk::EntityHandle& entity : m_staticObjects)
		{
			CollisionComponent2D& collision = entity->GetComponent<CollisionComponent2D>();
			NodeComponent& node = entity->GetComponent<NodeComponent>();

			Nz::RigidBody2D* body = collision.GetStaticBody();

			Nz::Vector2f oldPosition = body->GetPosition();
			Nz::Vector2f newPosition = Nz::Vector2f(node.GetPosition(Nz::CoordSys_Global));

			// To move static objects and ensure their collisions, we have to specify them a velocity
			// (/!\: the physical engine does not apply the speed on static objects)
			if (newPosition != oldPosition)
			{
				body->SetPosition(newPosition);
				body->SetVelocity((newPosition - oldPosition) * invElapsedTime);
			}
			else
				body->SetVelocity(Nz::Vector2f::Zero());

			Nz::RadianAnglef oldRotation = body->GetRotation();
			Nz::RadianAnglef newRotation = node.GetRotation(Nz::CoordSys_Global).To2DAngle();

			if (newRotation != oldRotation)
			{
				body->SetRotation(oldRotation);
				body->SetAngularVelocity((newRotation - oldRotation) * invElapsedTime);
			}
			else
				body->SetAngularVelocity(Nz::RadianAnglef::Zero());
		}
	}

	void PhysicsSystem2D::RegisterCallbacks(unsigned int collisionId, Callback callbacks)
	{
		Nz::PhysWorld2D::Callback worldCallbacks;

		if (callbacks.endCallback)
		{
			worldCallbacks.endCallback = [this, cb = std::move(callbacks.endCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		if (callbacks.preSolveCallback)
		{
			worldCallbacks.preSolveCallback = [this, cb = std::move(callbacks.preSolveCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				return cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		if (callbacks.postSolveCallback)
		{
			worldCallbacks.postSolveCallback = [this, cb = std::move(callbacks.postSolveCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		if (callbacks.startCallback)
		{
			worldCallbacks.startCallback = [this, cb = std::move(callbacks.startCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				return cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		worldCallbacks.userdata = callbacks.userdata;

		m_physWorld->RegisterCallbacks(collisionId, worldCallbacks);
	}

	void PhysicsSystem2D::RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, Callback callbacks)
	{
		Nz::PhysWorld2D::Callback worldCallbacks;

		if (callbacks.endCallback)
		{
			worldCallbacks.endCallback = [this, cb = std::move(callbacks.endCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		if (callbacks.preSolveCallback)
		{
			worldCallbacks.preSolveCallback = [this, cb = std::move(callbacks.preSolveCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				return cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		if (callbacks.postSolveCallback)
		{
			worldCallbacks.postSolveCallback = [this, cb = std::move(callbacks.postSolveCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		if (callbacks.startCallback)
		{
			worldCallbacks.startCallback = [this, cb = std::move(callbacks.startCallback)](Nz::PhysWorld2D& world, Nz::Arbiter2D& arbiter, Nz::RigidBody2D& bodyA, Nz::RigidBody2D& bodyB, void* userdata)
			{
				return cb(*this, arbiter, GetEntityFromBody(bodyA), GetEntityFromBody(bodyB), userdata);
			};
		}

		worldCallbacks.userdata = callbacks.userdata;

		m_physWorld->RegisterCallbacks(collisionIdA, collisionIdB, worldCallbacks);
	}

	SystemIndex PhysicsSystem2D::systemIndex;
}
