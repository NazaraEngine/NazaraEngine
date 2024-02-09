// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline PhysWorld2D& Physics2DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const PhysWorld2D& Physics2DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}

	inline entt::handle Physics2DSystem::GetRigidBodyEntity(UInt32 bodyIndex) const
	{
		return entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
	}

	inline bool Physics2DSystem::NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, entt::handle* nearestEntity)
	{
		RigidBody2D* nearestBody;
		if (!m_physWorld.NearestBodyQuery(from, maxDistance, collisionGroup, categoryMask, collisionMask, &nearestBody))
			return false;

		if (nearestEntity)
		{
			if (nearestBody)
				*nearestEntity = GetRigidBodyEntity(nearestBody->GetBodyIndex());
			else
				*nearestEntity = {};
		}

		return true;
	}

	inline bool Physics2DSystem::NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, NearestQueryResult* result)
	{
		if (!m_physWorld.NearestBodyQuery(from, maxDistance, collisionGroup, categoryMask, collisionMask, result))
			return false;

		if (result)
		{
			if (result->nearestBody)
				result->nearestEntity = GetRigidBodyEntity(result->nearestBody->GetBodyIndex());
			else
				result->nearestEntity = {};
		}

		return true;
	}

	inline void Physics2DSystem::RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(const RaycastHit&)>& callback)
	{
		return m_physWorld.RaycastQuery(from, to, radius, collisionGroup, categoryMask, collisionMask, [&](const PhysWorld2D::RaycastHit& hitInfo)
		{
			RaycastHit extendedHitInfo;
			static_cast<PhysWorld2D::RaycastHit&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.nearestBody)
				extendedHitInfo.nearestEntity = GetRigidBodyEntity(extendedHitInfo.nearestBody->GetBodyIndex());

			callback(extendedHitInfo);
		});
	}

	inline bool Physics2DSystem::RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<RaycastHit>* hitInfos)
	{
		NazaraAssert(hitInfos, "invalid output pointer");

		std::size_t originalSize = hitInfos->size();

		RaycastQuery(from, to, radius, collisionGroup, categoryMask, collisionMask, [&](const RaycastHit& hitInfo)
		{
			hitInfos->emplace_back(hitInfo);
		});

		return hitInfos->size() != originalSize;
	}

	inline bool Physics2DSystem::RaycastQueryFirst(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, RaycastHit* hitInfo)
	{
		if (!m_physWorld.RaycastQueryFirst(from, to, radius, collisionGroup, categoryMask, collisionMask, hitInfo))
			return false;

		if (hitInfo)
		{
			if (hitInfo->nearestBody)
				hitInfo->nearestEntity = GetRigidBodyEntity(hitInfo->nearestBody->GetBodyIndex());
			else
				hitInfo->nearestEntity = {};
		}

		return true;
	}

	inline void Physics2DSystem::RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(entt::handle)>& callback)
	{
		return m_physWorld.RegionQuery(boundingBox, collisionGroup, categoryMask, collisionMask, [&](RigidBody2D* body)
		{
			callback(GetRigidBodyEntity(body->GetBodyIndex()));
		});
	}

	inline void Physics2DSystem::RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<entt::handle>* bodies)
	{
		NazaraAssert(bodies, "invalid output pointer");

		return m_physWorld.RegionQuery(boundingBox, collisionGroup, categoryMask, collisionMask, [&](RigidBody2D* body)
		{
			bodies->emplace_back(GetRigidBodyEntity(body->GetBodyIndex()));
		});
	}

	inline void Physics2DSystem::RegisterCallbacks(unsigned int collisionId, ContactCallbacks callbacks)
	{
		return m_physWorld.RegisterCallbacks(collisionId, SetupContactCallbacks(std::move(callbacks)));
	}

	inline void Physics2DSystem::RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, ContactCallbacks callbacks)
	{
		return m_physWorld.RegisterCallbacks(collisionIdA, collisionIdB, SetupContactCallbacks(std::move(callbacks)));
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
