// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_CONFIGCHECK_HPP
#define NAZARA_PLATFORM_CONFIGCHECK_HPP

/// This file is used to check the constant values defined in Config.hpp

#include <type_traits>
#define NazaraCheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// We force the value of MANAGE_MEMORY in debug
#if defined(NAZARA_DEBUG) && !NAZARA_PLATFORM_MANAGE_MEMORY
	#undef NAZARA_PLATFORM_MANAGE_MEMORY
	#define NAZARA_PLATFORM_MANAGE_MEMORY 0
#endif

#undef NazaraCheckTypeAndVal

#endif // NAZARA_PLATFORM_CONFIGCHECK_HPP
