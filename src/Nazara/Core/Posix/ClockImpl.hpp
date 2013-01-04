// Copyright (C) 2012 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLOCKIMPL_POSIX_HPP
#define NAZARA_CLOCKIMPL_POSIX_HPP

#include <Nazara/Prerequesites.hpp>

bool NzClockImplInitializeHighPrecision();
nzUInt64 NzClockImplGetMicroseconds();
nzUInt64 NzClockImplGetMilliseconds();

#endif // NAZARA_CLOCKIMPL_POSIX_HPP
