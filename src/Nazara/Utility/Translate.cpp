#include <Nazara/Utility/Translate.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Translator.hpp>

#include <iostream>

namespace Nz
{
    Nz::String Translate(const Nz::String& toTranslate)
    {
        NazaraAssert(Nz::Translator::s_translatorInstance, "No Translator has been created");

        auto& translations = Nz::Translator::s_translatorInstance->m_translations[Nz::Translator::s_translatorInstance->m_targetLang];

        NazaraAssert(translations[toTranslate].is_string(), "Translations must be in string format");

        return translations[toTranslate];
    }
}