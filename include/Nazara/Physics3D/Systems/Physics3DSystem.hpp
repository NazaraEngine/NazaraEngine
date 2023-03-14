// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP
#define NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/Components/RigidBody3DComponent.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3DSystem
	{
		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<RigidBody3DComponent, class NodeComponent>;

			struct RaycastHit;

			Physics3DSystem(entt::registry& registry);
			Physics3DSystem(const Physics3DSystem&) = delete;
			Physics3DSystem(Physics3DSystem&&) = delete;
			~Physics3DSystem();

			template<typename... Args> RigidBody3DComponent CreateRigidBody(Args&&... args);

			inline PhysWorld3D& GetPhysWorld();
			inline const PhysWorld3D& GetPhysWorld() const;

			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo = nullptr);

			void Update(Time elapsedTime);

			Physics3DSystem& operator=(const Physics3DSystem&) = delete;
			Physics3DSystem& operator=(Physics3DSystem&&) = delete;

			struct RaycastHit : PhysWorld3D::RaycastHit
			{
				entt::handle hitEntity;
			};

		private:
			void OnConstruct(entt::registry& registry, entt::entity entity);
			void OnDestruct(entt::registry& registry, entt::entity entity);

			std::vector<entt::entity> m_physicsEntities;
			entt::registry& m_registry;
			entt::observer m_physicsConstructObserver;
			entt::scoped_connection m_constructConnection;
			entt::scoped_connection m_destructConnection;
			PhysWorld3D m_physWorld;
	};
}

#include <Nazara/Physics3D/Systems/Physics3DSystem.inl>

#endif // NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP
