// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONCOMPONENT_HPP
#define NAZARA_CORE_APPLICATIONCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class NAZARA_CORE_API ApplicationComponent
	{
		public:
			ApplicationComponent() = default;
			ApplicationComponent(const ApplicationComponent&) = delete;
			ApplicationComponent(ApplicationComponent&&) = delete;
			virtual ~ApplicationComponent();

			virtual void Update(Time elapsedTime);

			ApplicationComponent& operator=(const ApplicationComponent&) = delete;
			ApplicationComponent& operator=(ApplicationComponent&&) = delete;
	};
}

#include <Nazara/Core/ApplicationComponent.inl>

#endif // NAZARA_CORE_APPLICATIONCOMPONENT_HPP
