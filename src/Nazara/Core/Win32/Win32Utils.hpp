// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_WIN32UTILS_HPP
#define NAZARA_CORE_WIN32_WIN32UTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <ctime>
#include <filesystem>
#include <Windows.h>

namespace Nz::PlatformImpl
{
	constexpr bool ArePathWide = std::is_same_v<std::filesystem::path::value_type, wchar_t>;

	using WidePathHolder = std::conditional_t<ArePathWide, /*null-terminated*/ std::wstring_view, std::wstring>;

	inline WidePathHolder PathToWideTemp(const std::filesystem::path& path);
	time_t FileTimeToTime(FILETIME* time);
}

#include <Nazara/Core/Win32/Win32Utils.inl>

#endif // NAZARA_CORE_WIN32_WIN32UTILS_HPP
