// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_TOSTRING_HPP
#define NAZARA_CORE_TOSTRING_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <filesystem>
#include <string>

namespace Nz
{
	template<typename... Args> std::string Format(std::string_view str, Args&&... args);

	template<typename T> decltype(auto) ToString(T&& value);

	template<typename T>
	struct ToStringFormatter
	{
		static_assert(AlwaysFalse<T>(), "ToStringFormatter is not implemented for this type");
	};

	template<>
	struct NAZARA_CORE_API ToStringFormatter<std::filesystem::path>
	{
		static std::string Format(const std::filesystem::path& path);
	};

	template<>
	struct ToStringFormatter<std::string>
	{
		static const std::string& Format(const std::string& value);
		static std::string Format(std::string&& value);
	};

	template<>
	struct ToStringFormatter<std::string_view>
	{
		static std::string Format(std::string_view value);
	};

	template<>
	struct ToStringFormatter<const char*>
	{
		static std::string_view Format(const char* value);
		template<std::size_t N> static  std::string_view Format(const char(&str)[N]);
	};

	// Specializations declared in .inl
#define NAZARA_TO_STRING_INLINE_SPEC(Type) \
	template<> \
	struct ToStringFormatter<Type> \
	{ \
		static std::string Format(Type value); \
	}

	NAZARA_TO_STRING_INLINE_SPEC(short);
	NAZARA_TO_STRING_INLINE_SPEC(int);
	NAZARA_TO_STRING_INLINE_SPEC(long);
	NAZARA_TO_STRING_INLINE_SPEC(long long);
	NAZARA_TO_STRING_INLINE_SPEC(unsigned short);
	NAZARA_TO_STRING_INLINE_SPEC(unsigned int);
	NAZARA_TO_STRING_INLINE_SPEC(unsigned long);
	NAZARA_TO_STRING_INLINE_SPEC(unsigned long long);

#undef NAZARA_TO_STRING_INLINE_SPEC

	// Specializations declared in .cpp
#define NAZARA_TO_STRING_CPP_SPEC(Type) \
	template<> \
	struct NAZARA_CORE_API ToStringFormatter<Type> \
	{ \
		static std::string Format(Type value); \
	}

	NAZARA_TO_STRING_CPP_SPEC(float);
	NAZARA_TO_STRING_CPP_SPEC(double);
	NAZARA_TO_STRING_CPP_SPEC(long double);

#undef NAZARA_TO_STRING_CPP_SPEC
}

#include <Nazara/Core/ToString.inl>

#endif // NAZARA_CORE_TOSTRING_HPP
