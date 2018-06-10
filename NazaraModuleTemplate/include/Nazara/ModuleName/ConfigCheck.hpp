// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_MODULENAME_HPP
#define NAZARA_CONFIG_CHECK_MODULENAME_HPP

/// This file is used to check the constant values defined in Config.hpp

#include <type_traits>
#define CheckType(name, type, err) static_assert(std::is_ ##type <decltype(name)>::value, #type err)
#define CheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// We force the value of MANAGE_MEMORY in debug
#if defined(NAZARA_DEBUG) && !NAZARA_MODULENAME_MANAGE_MEMORY
	#undef NAZARA_MODULENAME_MANAGE_MEMORY
	#define NAZARA_MODULENAME_MANAGE_MEMORY 0
#endif

#endif // NAZARA_CONFIG_CHECK_MODULENAME_HPP
