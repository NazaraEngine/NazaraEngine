// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_SYSTEMS_CHIPMUNKPHYSICS2DSYSTEM_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_SYSTEMS_CHIPMUNKPHYSICS2DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkPhysWorld2D.hpp>
#include <Nazara/ChipmunkPhysics2D/Components/ChipmunkRigidBody2DComponent.hpp>
#include <Nazara/Core/Time.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkPhysics2DSystem
	{
		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<ChipmunkRigidBody2DComponent, class NodeComponent>;

			ChipmunkPhysics2DSystem(entt::registry& registry);
			ChipmunkPhysics2DSystem(const ChipmunkPhysics2DSystem&) = delete;
			ChipmunkPhysics2DSystem(ChipmunkPhysics2DSystem&&) = delete;
			~ChipmunkPhysics2DSystem();

			template<typename... Args> ChipmunkRigidBody2DComponent CreateRigidBody(Args&&... args);

			inline ChipmunkPhysWorld2D& GetPhysWorld();
			inline const ChipmunkPhysWorld2D& GetPhysWorld() const;

			void Update(Time elapsedTime);

			ChipmunkPhysics2DSystem& operator=(const ChipmunkPhysics2DSystem&) = delete;
			ChipmunkPhysics2DSystem& operator=(ChipmunkPhysics2DSystem&&) = delete;

		private:
			entt::registry& m_registry;
			entt::observer m_physicsConstructObserver;
			entt::scoped_connection m_constructConnection;
			ChipmunkPhysWorld2D m_physWorld;
	};
}

#include <Nazara/ChipmunkPhysics2D/Systems/ChipmunkPhysics2DSystem.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_SYSTEMS_CHIPMUNKPHYSICS2DSYSTEM_HPP
