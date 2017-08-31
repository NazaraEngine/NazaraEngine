// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_UTILITY_HPP
#define NAZARA_CONFIG_CHECK_UTILITY_HPP

/// This file is used to check the constant values defined in Config.hpp

#include <type_traits>
#define NazaraCheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// We force the value of MANAGE_MEMORY in debug
#if defined(NAZARA_DEBUG) && !NAZARA_UTILITY_MANAGE_MEMORY
	#undef NAZARA_UTILITY_MANAGE_MEMORY
	#define NAZARA_UTILITY_MANAGE_MEMORY 0
#endif

NazaraCheckTypeAndVal(NAZARA_UTILITY_SKINNING_MAX_WEIGHTS, integral, >, 0, " shall be a strictly positive integer");

#undef NazaraCheckTypeAndVal

#endif // NAZARA_CONFIG_CHECK_UTILITY_HPP
