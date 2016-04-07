#include <Nazara/Utility/Translator.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>

namespace Nz
{
    Translator::Translator(Lang targetLang, const String& translationsDir)
    {
        NazaraAssert(!s_translatorInstance, "There can have only one translator instance");

        m_targetLang = targetLang;

        Directory dir{ translationsDir };
        dir.Open();
        while (dir.NextResult())
        {
            if (dir.IsResultDirectory())
                continue;

            File file{ dir.GetResultPath(), OpenMode_ReadOnly | OpenMode_Text };

            if (file.GetFileName().SubStringFrom('.') != "tr")
                continue;

            String fileContent;
            while (!file.EndOfFile())
                fileContent += file.ReadLine() + '\n';

            Nz::String filename = file.GetFileName().SubStringTo('.');
            Nz::Lang lang;
            if (filename == "FR")
                lang = Lang_FR;
            else if (filename == "EN_US")
                lang = Lang_EN_US;
            else if (filename == "EN_UK")
                lang = Lang_EN_UK;
            else if (filename == "ES")
                lang = Lang_ES;
            else if (filename == "DE")
                lang = Lang_DE;
            else if (filename == "IT")
                lang = Lang_IT;

            try
            {
                m_translations[lang] = json::json::parse(fileContent);
            }
            catch (const std::invalid_argument& e)
            {
                NazaraError(e.what());
            }
        }

        s_translatorInstance.reset(this);
    }

	Translator::~Translator()
	{
        s_translatorInstance.release();
	}

    void Translator::SetTargetLang(Lang targetLang)
    {
        m_targetLang = targetLang;
    }

	std::unique_ptr<Translator> Translator::s_translatorInstance;
}