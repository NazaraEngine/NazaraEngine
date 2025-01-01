// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_COMPONENTS_DISABLEDCOMPONENT_HPP
#define NAZARA_CORE_COMPONENTS_DISABLEDCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class DisabledComponent
	{
		public:
			DisabledComponent() = default;
			DisabledComponent(const DisabledComponent&) = default;
			DisabledComponent(DisabledComponent&&) = default;
			~DisabledComponent() = default;

			DisabledComponent& operator=(const DisabledComponent&) = default;
			DisabledComponent& operator=(DisabledComponent&&) = default;
	};
}

#endif // NAZARA_CORE_COMPONENTS_DISABLEDCOMPONENT_HPP
