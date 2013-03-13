// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWS_TIME_HPP
#define NAZARA_WINDOWS_TIME_HPP

#include <ctime>
#include <windows.h>

time_t NzFileTimeToTime(FILETIME* time);

#endif // NAZARA_WINDOWS_TIME_HPP
