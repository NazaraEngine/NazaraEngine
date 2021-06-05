// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_FREETYPE_HPP
#define NAZARA_LOADERS_FREETYPE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Font.hpp>

namespace Nz::Loaders
{
	bool InitializeFreeType();
	FontLoader::Entry GetFontLoader_FreeType();
	void UninitializeFreeType();
}

#endif // NAZARA_LOADERS_FREETYPE_HPP
