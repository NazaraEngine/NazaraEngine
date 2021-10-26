// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_CLOCKIMPL_HPP
#define NAZARA_CORE_WIN32_CLOCKIMPL_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz
{
	bool ClockImplInitializeHighPrecision();
	UInt64 ClockImplGetElapsedMicroseconds();
	UInt64 ClockImplGetElapsedMilliseconds();
}

#endif // NAZARA_CORE_WIN32_CLOCKIMPL_HPP
