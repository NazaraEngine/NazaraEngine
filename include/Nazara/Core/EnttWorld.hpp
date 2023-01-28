// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ENTTWORLD_HPP
#define NAZARA_CORE_ENTTWORLD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/EntityWorld.hpp>
#include <Nazara/Core/EnttSystemGraph.hpp>

namespace Nz
{
	class NAZARA_CORE_API EnttWorld : public EntityWorld
	{
		public:
			EnttWorld();
			EnttWorld(const EnttWorld&) = default;
			EnttWorld(EnttWorld&&) = default;
			~EnttWorld() = default;

			template<typename T, typename... Args> T& AddSystem(Args&&... args);

			entt::handle CreateEntity();

			entt::registry& GetRegistry();
			const entt::registry& GetRegistry() const;
			template<typename T> T& GetSystem() const;

			void Update(Time elapsedTime) override;

			operator entt::registry&();
			operator const entt::registry&() const;

			EnttWorld& operator=(const EnttWorld&) = delete;
			EnttWorld& operator=(EnttWorld&&) = delete;

		private:
			entt::registry m_registry;
			EnttSystemGraph m_systemGraph;
	};
}

#include <Nazara/Core/EnttWorld.inl>

#endif // NAZARA_CORE_ENTTWORLD_HPP
