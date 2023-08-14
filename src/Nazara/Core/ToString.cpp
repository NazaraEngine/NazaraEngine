// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ToString.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		std::string FormatFallback(std::string_view str)
		{
			return fmt::format(str);
		}

		std::string FormatFallback(std::string_view str, std::string_view param1)
		{
			return fmt::format(str, param1);
		}

		std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2)
		{
			return fmt::format(str, param1, param2);
		}

		std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2, std::string_view param3)
		{
			return fmt::format(str, param1, param2, param3);
		}

		std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2, std::string_view param3, std::string_view param4)
		{
			return fmt::format(str, param1, param2, param3, param4);
		}

		std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2, std::string_view param3, std::string_view param4, std::string_view param5)
		{
			return fmt::format(str, param1, param2, param3, param4, param5);
		}
	}

	std::string ToStringFormatter<std::filesystem::path>::Format(const std::filesystem::path& path)
	{
		return path.generic_u8string();
	}

#define NAZARA_TO_STRING_FMT_SPEC(Type) \
	std::string ToStringFormatter<Type>::Format(Type value) \
	{ \
		return fmt::format("{}", value); \
	}

	NAZARA_TO_STRING_FMT_SPEC(float);
	NAZARA_TO_STRING_FMT_SPEC(double);
	NAZARA_TO_STRING_FMT_SPEC(long double);

#undef NAZARA_TO_STRING_CPP_SPEC
}
