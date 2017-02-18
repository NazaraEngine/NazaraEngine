// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_RENDERER_HPP
#define NAZARA_CONFIG_CHECK_RENDERER_HPP

/// Ce fichier sert à vérifier la valeur des constantes du fichier Config.hpp

#include <type_traits>
#define NazaraCheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// On force la valeur de MANAGE_MEMORY en mode debug
#if defined(NAZARA_DEBUG) && !NAZARA_RENDERER_MANAGE_MEMORY
	#undef NAZARA_RENDERER_MANAGE_MEMORY
	#define NAZARA_RENDERER_MANAGE_MEMORY 0
#endif

NazaraCheckTypeAndVal(NAZARA_RENDERER_INSTANCE_BUFFER_SIZE, integral, >, 0, " shall be a strictly positive integer");

#undef NazaraCheckTypeAndVal

#endif // NAZARA_CONFIG_CHECK_RENDERER_HPP
