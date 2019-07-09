// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLOCKIMPL_WINDOWS_HPP
#define NAZARA_CLOCKIMPL_WINDOWS_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz
{
	bool ClockImplInitializeHighPrecision();
	UInt64 ClockImplGetElapsedMicroseconds();
	UInt64 ClockImplGetElapsedMilliseconds();
}

#endif // NAZARA_CLOCKIMPL_WINDOWS_HPP
