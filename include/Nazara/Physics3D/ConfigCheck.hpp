// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_PHYSICS_HPP
#define NAZARA_CONFIG_CHECK_PHYSICS_HPP

/// Ce fichier sert à vérifier la valeur des constantes du fichier Config.hpp

// On force la valeur de MANAGE_MEMORY en mode debug
#if defined(NAZARA_DEBUG) && !NAZARA_PHYSICS3D_MANAGE_MEMORY
	#undef NAZARA_PHYSICS3D_MANAGE_MEMORY
	#define NAZARA_PHYSICS3D_MANAGE_MEMORY 0
#endif

#endif // NAZARA_CONFIG_CHECK_PHYSICS_HPP
