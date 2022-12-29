// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_COMPONENTS_LIFETIMECOMPONENT_HPP
#define NAZARA_CORE_COMPONENTS_LIFETIMECOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class LifetimeComponent
	{
		public:
			inline LifetimeComponent(Time lifetime);
			LifetimeComponent(const LifetimeComponent&) = default;
			LifetimeComponent(LifetimeComponent&&) = default;
			~LifetimeComponent() = default;

			inline void DecreaseLifetime(Time elapsedTime);

			inline Time GetRemainingLifeTime() const;

			inline bool IsAlive() const;

			LifetimeComponent& operator=(const LifetimeComponent&) = default;
			LifetimeComponent& operator=(LifetimeComponent&&) = default;

		private:
			Time m_remainingLifetime;
	};
}

#include <Nazara/Core/Components/LifetimeComponent.inl>

#endif // NAZARA_CORE_COMPONENTS_LIFETIMECOMPONENT_HPP
