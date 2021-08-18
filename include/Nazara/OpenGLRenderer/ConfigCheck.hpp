// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONFIG_CHECK_OPENGLRENDERER_HPP
#define NAZARA_CONFIG_CHECK_OPENGLRENDERER_HPP

/// Ce fichier sert à vérifier la valeur des constantes du fichier Config.hpp

#include <type_traits>
#define CheckType(name, type, err) static_assert(std::is_ ##type <decltype(name)>::value, #type err)
#define CheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

// On force la valeur de MANAGE_MEMORY en mode debug
#if defined(NAZARA_DEBUG) && !NAZARA_OPENGLRENDERER_MANAGE_MEMORY
	#undef NAZARA_OPENGLRENDERER_MANAGE_MEMORY
	#define NAZARA_OPENGLRENDERER_MANAGE_MEMORY 0
#endif

#endif // NAZARA_CONFIG_CHECK_OPENGLRENDERER_HPP
