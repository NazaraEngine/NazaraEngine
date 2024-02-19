// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ENVIRONMENTVARIABLES_HPP
#define NAZARA_CORE_ENVIRONMENTVARIABLES_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>

namespace Nz
{
	NAZARA_CORE_API const char* GetEnvironmentVariable(const char* envVar);
	NAZARA_CORE_API bool TestEnvironmentVariable(const char* envVar);
}

#include <Nazara/Core/EnvironmentVariables.inl>

#endif // NAZARA_CORE_ENVIRONMENTVARIABLES_HPP
