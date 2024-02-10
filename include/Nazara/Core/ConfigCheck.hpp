// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_CONFIGCHECK_HPP
#define NAZARA_CORE_CONFIGCHECK_HPP

/// This file is used to check the constant values defined in Config.hpp

#include <type_traits>
#define NazaraCheckTypeAndVal(name, type, op, val, err) static_assert(std::is_ ##type <decltype(name)>::value && name op val, #type err)

NazaraCheckTypeAndVal(NAZARA_CORE_FILE_BUFFERSIZE, integral, >, 0, " shall be a strictly positive integer");
NazaraCheckTypeAndVal(NAZARA_CORE_SKINNING_MAX_WEIGHTS, integral, >, 0, " shall be a strictly positive integer");

#undef NazaraCheckTypeAndVal

#endif // NAZARA_CORE_CONFIGCHECK_HPP
