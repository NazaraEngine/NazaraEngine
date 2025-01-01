// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/TextRenderer/TextRenderer.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <Nazara/TextRenderer/Formats/FreeTypeLoader.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <stdexcept>

namespace Nz
{
	TextRenderer::TextRenderer(Config /*config*/) :
	ModuleBase("TextRenderer", this)
	{
		if (!Font::Initialize())
			throw std::runtime_error("failed to initialize fonts");

		if (!Loaders::InitializeFreeType())
            throw std::runtime_error("failed to initialize freetype");

		m_fontLoader.RegisterLoader(Loaders::GetFontLoader_FreeType());
	}

	TextRenderer::~TextRenderer()
	{
		Loaders::UninitializeFreeType();

		Font::Uninitialize();
	}

	FontLoader& TextRenderer::GetFontLoader()
	{
		return m_fontLoader;
	}

	const FontLoader& TextRenderer::GetFontLoader() const
	{
		return m_fontLoader;
	}

	TextRenderer* TextRenderer::s_instance = nullptr;
}
