// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_FORMATS_FREETYPELOADER_HPP
#define NAZARA_TEXTRENDERER_FORMATS_FREETYPELOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/TextRenderer/Font.hpp>

namespace Nz::Loaders
{
	bool InitializeFreeType();
	FontLoader::Entry GetFontLoader_FreeType();
	void UninitializeFreeType();
}

#endif // NAZARA_TEXTRENDERER_FORMATS_FREETYPELOADER_HPP
