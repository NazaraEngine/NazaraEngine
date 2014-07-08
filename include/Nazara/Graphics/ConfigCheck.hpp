// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_GRAPHICS_HPP
#define NAZARA_CONFIG_CHECK_GRAPHICS_HPP

/// Ce fichier sert à vérifier la valeur des constantes du fichier Config.hpp

#include <type_traits>
#define CheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// On force la valeur de MANAGE_MEMORY en mode debug
#if defined(NAZARA_DEBUG) && !NAZARA_GRAPHICS_MANAGE_MEMORY
	#undef NAZARA_GRAPHICS_MANAGE_MEMORY
	#define NAZARA_GRAPHICS_MANAGE_MEMORY 1
#endif

CheckTypeAndVal(NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT, integral, >, 0, " shall be a strictly positive integer");
CheckTypeAndVal(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, integral, >, 0, " shall be a strictly positive integer");

#endif // NAZARA_CONFIG_CHECK_GRAPHICS_HPP
