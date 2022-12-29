// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_SYSTEMS_LIFETIMESYSTEM_HPP
#define NAZARA_CORE_SYSTEMS_LIFETIMESYSTEM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Utils/TypeList.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_CORE_API LifetimeSystem
	{
		public:
			static constexpr bool AllowConcurrent = false;
			static constexpr Int64 ExecutionOrder = 1'000'000;
			using Components = TypeList<class LifetimeComponent>;

			inline LifetimeSystem(entt::registry& registry);
			LifetimeSystem(const LifetimeSystem&) = delete;
			LifetimeSystem(LifetimeSystem&&) = delete;
			~LifetimeSystem() = default;

			void Update(Time elapsedTime);

			LifetimeSystem& operator=(const LifetimeSystem&) = delete;
			LifetimeSystem& operator=(LifetimeSystem&&) = delete;

		private:
			entt::registry& m_registry;
	};
}

#include <Nazara/Core/Systems/LifetimeSystem.inl>

#endif // NAZARA_CORE_SYSTEMS_LIFETIMESYSTEM_HPP
