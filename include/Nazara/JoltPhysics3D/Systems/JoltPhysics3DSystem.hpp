// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_SYSTEMS_JOLTPHYSICS3DSYSTEM_HPP
#define NAZARA_JOLTPHYSICS3D_SYSTEMS_JOLTPHYSICS3DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Nazara/JoltPhysics3D/Components/JoltRigidBody3DComponent.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltPhysics3DSystem
	{
		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<JoltRigidBody3DComponent, class NodeComponent>;

			struct RaycastHit;

			JoltPhysics3DSystem(entt::registry& registry);
			JoltPhysics3DSystem(const JoltPhysics3DSystem&) = delete;
			JoltPhysics3DSystem(JoltPhysics3DSystem&&) = delete;
			~JoltPhysics3DSystem();

			template<typename... Args> JoltRigidBody3DComponent CreateRigidBody(Args&&... args);

			void Dump();

			inline JoltPhysWorld3D& GetPhysWorld();
			inline const JoltPhysWorld3D& GetPhysWorld() const;

			bool RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback);
			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback);

			void Update(Time elapsedTime);

			JoltPhysics3DSystem& operator=(const JoltPhysics3DSystem&) = delete;
			JoltPhysics3DSystem& operator=(JoltPhysics3DSystem&&) = delete;

			struct RaycastHit : JoltPhysWorld3D::RaycastHit
			{
				entt::handle hitEntity;
			};

		private:
			void OnConstruct(entt::registry& registry, entt::entity entity);
			void OnDestruct(entt::registry& registry, entt::entity entity);

			std::size_t m_stepCount;
			std::vector<entt::entity> m_physicsEntities;
			entt::registry& m_registry;
			entt::observer m_physicsConstructObserver;
			entt::scoped_connection m_constructConnection;
			entt::scoped_connection m_destructConnection;
			JoltPhysWorld3D m_physWorld;
			Time m_physicsTime;
			Time m_updateTime;
	};
}

#include <Nazara/JoltPhysics3D/Systems/JoltPhysics3DSystem.inl>

#endif // NAZARA_JOLTPHYSICS3D_SYSTEMS_JOLTPHYSICS3DSYSTEM_HPP
