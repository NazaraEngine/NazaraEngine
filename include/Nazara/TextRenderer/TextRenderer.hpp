// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_HPP
#define NAZARA_TEXTRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/TextRenderer/Config.hpp>
#include <Nazara/TextRenderer/Font.hpp>

namespace Nz
{
	class NAZARA_TEXTRENDERER_API TextRenderer : public ModuleBase<TextRenderer>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			TextRenderer(Config /*config*/);
			~TextRenderer();

			FontLoader& GetFontLoader();
			const FontLoader& GetFontLoader() const;

		private:
			FontLoader m_fontLoader;

			static TextRenderer* s_instance;
	};
}

#endif // NAZARA_TEXTRENDERER_HPP
