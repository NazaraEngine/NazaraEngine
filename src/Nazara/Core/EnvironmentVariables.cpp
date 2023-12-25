// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/EnvironmentVariables.hpp>
#include <array>
#include <cstdlib>
#include <string_view>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	const char* GetEnvironmentVariable(const char* envVar)
	{
		return std::getenv(envVar);
	}

	bool TestEnvironmentVariable(const char* envVar)
	{
		const char* value = GetEnvironmentVariable(envVar);
		if (!value)
			return false;

		// Environment variable exists, recognize some values as disabling it
		using namespace std::string_view_literals;
		constexpr std::array disabledValues = {
			"0"sv,
			"n"sv,
			"false"sv,
		};

		for (std::string_view disabledValue : disabledValues)
		{
			if (value == disabledValue)
				return false;
		}

		return true;
	}
}
