// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FORMAT_HPP
#define NAZARA_CORE_FORMAT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <string>

#ifdef NAZARA_BUILD

#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/std.h>

#define NAZARA_FORMAT(s) FMT_STRING(s)

#else

#define NAZARA_FORMAT(s) s

#endif

namespace Nz
{
#ifdef NAZARA_BUILD
	template<typename... Args> using FormatString = fmt::format_string<Args...>;
#else
	template<typename... Args> using FormatString = std::string_view;
#endif

	NAZARA_FORCEINLINE std::string Format(FormatString<> str);
	template<typename... Args> std::string Format(FormatString<Args...> str, Args&&... args);
}

#include <Nazara/Core/Format.inl>

#endif // NAZARA_CORE_FORMAT_HPP
