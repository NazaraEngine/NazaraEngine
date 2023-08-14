// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifdef NAZARA_BUILD
#include <fmt/format.h>
#include <fmt/std.h>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		NAZARA_CORE_API std::string FormatFallback(std::string_view str);
		NAZARA_CORE_API std::string FormatFallback(std::string_view str, std::string_view param1);
		NAZARA_CORE_API std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2);
		NAZARA_CORE_API std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2, std::string_view param3);
		NAZARA_CORE_API std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2, std::string_view param3, std::string_view param4);
		NAZARA_CORE_API std::string FormatFallback(std::string_view str, std::string_view param1, std::string_view param2, std::string_view param3, std::string_view param4, std::string_view param5);
	}

	template<typename ...Args>
	std::string Format(std::string_view str, Args&&... args)
	{
#ifdef NAZARA_BUILD
		return fmt::format(str, std::forward<Args>(args)...);
#else
		return Detail::FormatFallback(str, ToString(args)...);
#endif
	}


	template<typename T>
	decltype(auto) ToString(T&& value)
	{
		return ToStringFormatter<std::decay_t<T>>::Format(std::forward<T>(value));
	}


	inline const std::string& ToStringFormatter<std::string>::Format(const std::string& value)
	{
		return value;
	}

	inline std::string ToStringFormatter<std::string>::Format(std::string&& value)
	{
		return value;
	}


	inline std::string ToStringFormatter<std::string_view>::Format(std::string_view value)
	{
		return std::string(value);
	}


	inline std::string_view ToStringFormatter<const char*>::Format(const char* value)
	{
		return std::string_view(value);
	}

	template<std::size_t N>
	inline std::string_view ToStringFormatter<const char*>::Format(const char(&str)[N])
	{
		return std::string_view(str, N);
	}

#define NAZARA_TO_STRING_STD_SPEC(Type) \
	inline std::string ToStringFormatter<Type>::Format(Type value) \
	{ \
		return std::to_string(value); \
	}

	NAZARA_TO_STRING_STD_SPEC(short);
	NAZARA_TO_STRING_STD_SPEC(int);
	NAZARA_TO_STRING_STD_SPEC(long);
	NAZARA_TO_STRING_STD_SPEC(long long);
	NAZARA_TO_STRING_STD_SPEC(unsigned short);
	NAZARA_TO_STRING_STD_SPEC(unsigned int);
	NAZARA_TO_STRING_STD_SPEC(unsigned long);
	NAZARA_TO_STRING_STD_SPEC(unsigned long long);

#undef NAZARA_TO_STRING_STD_SPEC
}

#include <Nazara/Core/DebugOff.hpp>
