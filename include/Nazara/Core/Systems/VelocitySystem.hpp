// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SYSTEMS_VELOCITYSYSTEM_HPP
#define NAZARA_CORE_SYSTEMS_VELOCITYSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <flecs.h>

namespace Nz
{
	class NAZARA_CORE_API VelocitySystem
	{
		public:
			using Components = TypeList<class NodeComponent, class VelocityComponent>;

			inline VelocitySystem(flecs::world& world);
			VelocitySystem(const VelocitySystem&) = delete;
			VelocitySystem(VelocitySystem&&) = delete;
			~VelocitySystem() = default;

			void Update(Time elapsedTime);

			VelocitySystem& operator=(const VelocitySystem&) = delete;
			VelocitySystem& operator=(VelocitySystem&&) = delete;

		private:
			flecs::world& m_world;
	};
}

#include <Nazara/Core/Systems/VelocitySystem.inl>

#endif // NAZARA_CORE_SYSTEMS_VELOCITYSYSTEM_HPP
