// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3DSYSTEM_HPP
#define NAZARA_PHYSICS3DSYSTEM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/Components/RigidBody3DComponent.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3DSystem
	{
		public:
			Physics3DSystem(entt::registry& registry);
			Physics3DSystem(const Physics3DSystem&) = delete;
			Physics3DSystem(Physics3DSystem&&) = delete;
			~Physics3DSystem();

			template<typename... Args> RigidBody3DComponent CreateRigidBody(Args&&... args);

			inline PhysWorld3D& GetPhysWorld();
			inline const PhysWorld3D& GetPhysWorld() const;

			void Update(entt::registry& registry, float elapsedTime);

			Physics3DSystem& operator=(const Physics3DSystem&) = delete;
			Physics3DSystem& operator=(Physics3DSystem&&) = delete;

		private:
			static void OnConstruct(entt::registry& registry, entt::entity entity);

			entt::connection m_constructConnection;
			PhysWorld3D m_physWorld;
	};
}

#include <Nazara/Physics3D/Systems/Physics3DSystem.inl>

#endif
