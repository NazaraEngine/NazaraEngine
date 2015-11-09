// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_NETWORK_HPP
#define NAZARA_CONFIG_CHECK_NETWORK_HPP

/// Ce fichier sert à vérifier la valeur des constantes du fichier Config.hpp

#include <type_traits>

// On force la valeur de MANAGE_MEMORY en mode debug
#if defined(NAZARA_DEBUG) && !NAZARA_NETWORK_MANAGE_MEMORY
	#undef NAZARA_NETWORK_MANAGE_MEMORY
	#define NAZARA_NETWORK_MANAGE_MEMORY 0
#endif

#endif // NAZARA_CONFIG_CHECK_NETWORK_HPP
