// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_SYSTEMS_BULLETPHYSICS3DSYSTEM_HPP
#define NAZARA_BULLETPHYSICS3D_SYSTEMS_BULLETPHYSICS3DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/BulletPhysWorld3D.hpp>
#include <Nazara/BulletPhysics3D/Components/BulletRigidBody3DComponent.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Time.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_BULLETPHYSICS3D_API BulletPhysics3DSystem
	{
		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<BulletRigidBody3DComponent, class NodeComponent>;

			struct RaycastHit;

			BulletPhysics3DSystem(entt::registry& registry);
			BulletPhysics3DSystem(const BulletPhysics3DSystem&) = delete;
			BulletPhysics3DSystem(BulletPhysics3DSystem&&) = delete;
			~BulletPhysics3DSystem();

			template<typename... Args> BulletRigidBody3DComponent CreateRigidBody(Args&&... args);

			void Dump();

			inline BulletPhysWorld3D& GetPhysWorld();
			inline const BulletPhysWorld3D& GetPhysWorld() const;

			bool RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback);
			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo = nullptr);

			void Update(Time elapsedTime);

			BulletPhysics3DSystem& operator=(const BulletPhysics3DSystem&) = delete;
			BulletPhysics3DSystem& operator=(BulletPhysics3DSystem&&) = delete;

			struct RaycastHit : BulletPhysWorld3D::RaycastHit
			{
				entt::handle hitEntity;
			};

		private:
			void OnConstruct(entt::registry& registry, entt::entity entity);
			void OnDestruct(entt::registry& registry, entt::entity entity);

			std::size_t m_activeObjectCount;
			std::size_t m_stepCount;
			std::vector<entt::entity> m_physicsEntities;
			entt::registry& m_registry;
			entt::observer m_physicsConstructObserver;
			entt::scoped_connection m_constructConnection;
			entt::scoped_connection m_destructConnection;
			BulletPhysWorld3D m_physWorld;
			Time m_physicsTime;
			Time m_updateTime;
	};
}

#include <Nazara/BulletPhysics3D/Systems/BulletPhysics3DSystem.inl>

#endif // NAZARA_BULLETPHYSICS3D_SYSTEMS_BULLETPHYSICS3DSYSTEM_HPP
