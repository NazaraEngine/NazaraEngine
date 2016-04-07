// Copyright (C) 2016
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TRANSLATOR_HPP
#define NAZARA_TRANSLATOR_HPP

#include <json/json.hpp>
#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	class NAZARA_UTILITY_API Translator
	{
		friend NAZARA_UTILITY_API String Translate(const String&);

		public:
			Translator(Lang targetLang, const String& translationsDir = "translations");
			~Translator();

            void SetTargetLang(Lang targetLang);

		private:
			std::unordered_map<Lang, json::json> m_translations;
            Lang m_targetLang;

			static std::unique_ptr<Translator> s_translatorInstance;
	};
}

#endif // NAZARA_TRANSLATOR_HPP