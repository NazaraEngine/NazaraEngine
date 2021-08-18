// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWS_TIME_HPP
#define NAZARA_WINDOWS_TIME_HPP

#include <Nazara/Prerequisites.hpp>
#include <ctime>
#include <windows.h>

namespace Nz
{
	time_t FileTimeToTime(FILETIME* time);
}

#endif // NAZARA_WINDOWS_TIME_HPP
