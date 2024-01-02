// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_FORMATS_FREETYPELOADER_HPP
#define NAZARA_UTILITY_FORMATS_FREETYPELOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Utility/Font.hpp>

namespace Nz::Loaders
{
	bool InitializeFreeType();
	FontLoader::Entry GetFontLoader_FreeType();
	void UninitializeFreeType();
}

#endif // NAZARA_UTILITY_FORMATS_FREETYPELOADER_HPP
