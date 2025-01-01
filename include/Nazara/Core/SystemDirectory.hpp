// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SYSTEMDIRECTORY_HPP
#define NAZARA_CORE_SYSTEMDIRECTORY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/Result.hpp>
#include <filesystem>

namespace Nz
{
	NAZARA_CORE_API Result<std::filesystem::path, std::string> GetApplicationDirectory(ApplicationDirectory applicationDirectory, std::string_view applicationName);
	NAZARA_CORE_API Result<std::filesystem::path, std::string> GetSystemDirectory(SystemDirectory systemDirectory);
}

#include <Nazara/Core/SystemDirectory.inl>

#endif // NAZARA_CORE_SYSTEMDIRECTORY_HPP
