// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_UTILS_HPP
#define NAZARA_CORE_WIN32_UTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <ctime>
#include <Windows.h>

namespace Nz
{
	time_t FileTimeToTime(FILETIME* time);
}

#endif // NAZARA_CORE_WIN32_UTILS_HPP
