// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_GRAPHICS_HPP
#define NAZARA_CONFIG_CHECK_GRAPHICS_HPP

/// This file is used to check the constant values defined in Config.hpp

#include <type_traits>
#define NazaraCheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// We fore the value of MANAGE_MEMORY in debug
#if defined(NAZARA_DEBUG) && !NAZARA_GRAPHICS_MANAGE_MEMORY
	#undef NAZARA_GRAPHICS_MANAGE_MEMORY
	#define NAZARA_GRAPHICS_MANAGE_MEMORY 0
#endif

NazaraCheckTypeAndVal(NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT, integral, >, 0, " shall be a strictly positive integer");
NazaraCheckTypeAndVal(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, integral, >, 0, " shall be a strictly positive integer");

#undef NazaraCheckTypeAndVal

#endif // NAZARA_CONFIG_CHECK_GRAPHICS_HPP
