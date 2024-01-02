// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_SYSTEMS_VELOCITYSYSTEM_HPP
#define NAZARA_UTILITY_SYSTEMS_VELOCITYSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Utility/Config.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API VelocitySystem
	{
		public:
			using Components = TypeList<class NodeComponent, class VelocityComponent>;

			inline VelocitySystem(entt::registry& registry);
			VelocitySystem(const VelocitySystem&) = delete;
			VelocitySystem(VelocitySystem&&) = delete;
			~VelocitySystem() = default;

			void Update(Time elapsedTime);

			VelocitySystem& operator=(const VelocitySystem&) = delete;
			VelocitySystem& operator=(VelocitySystem&&) = delete;

		private:
			entt::registry& m_registry;
	};
}

#include <Nazara/Utility/Systems/VelocitySystem.inl>

#endif // NAZARA_UTILITY_SYSTEMS_VELOCITYSYSTEM_HPP
