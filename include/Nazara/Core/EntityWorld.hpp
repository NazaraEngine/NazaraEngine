// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ENTITYWORLD_HPP
#define NAZARA_CORE_ENTITYWORLD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class NAZARA_CORE_API EntityWorld
	{
		public:
			EntityWorld() = default;
			EntityWorld(const EntityWorld&) = default;
			EntityWorld(EntityWorld&&) = default;
			virtual ~EntityWorld();

			virtual void Update(Time elapsedTime) = 0;

			EntityWorld& operator=(const EntityWorld&) = default;
			EntityWorld& operator=(EntityWorld&&) = default;
	};
}

#include <Nazara/Core/EntityWorld.inl>

#endif // NAZARA_CORE_ENTITYWORLD_HPP
