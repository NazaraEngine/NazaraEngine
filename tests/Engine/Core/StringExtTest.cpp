#include <Nazara/Core/StringExt.hpp>
#include <catch2/catch.hpp>

SCENARIO("String", "[CORE][STRING]")
{
	std::string unicodeString(u8"\u00E0\u00E9\u00E7\u0153\u00C2\u5B98\u46E1");

	WHEN("Converting string back and forth")
	{
		CHECK(Nz::FromUtf16String(Nz::ToUtf16String(unicodeString)) == unicodeString);
		CHECK(Nz::FromUtf32String(Nz::ToUtf32String(unicodeString)) == unicodeString);
	}

	WHEN("Fetching words")
	{
		CHECK(Nz::GetWord({}, 0).empty());
		CHECK(Nz::GetWord("  ", 0).empty());

		std::string sentence = u8"\nSay hello\tto    Nazara\u00A0Engine\n\t!  "; //< \u00A0 is a No-Break Space
		CHECK(Nz::GetWord(sentence, 0) == "Say");
		CHECK(Nz::GetWord(sentence, 1) == "hello");
		CHECK(Nz::GetWord(sentence, 2) == "to");
		CHECK(Nz::GetWord(sentence, 3) == u8"Nazara\u00A0Engine");
		CHECK(Nz::GetWord(sentence, 4) == "!");
		CHECK(Nz::GetWord(sentence, 5).empty());

		// Try the same using Unicode aware overloads
		CHECK(Nz::GetWord(sentence, 0, Nz::UnicodeAware{}) == "Say");
		CHECK(Nz::GetWord(sentence, 1, Nz::UnicodeAware{}) == "hello");
		CHECK(Nz::GetWord(sentence, 2, Nz::UnicodeAware{}) == "to");
		CHECK(Nz::GetWord(sentence, 3, Nz::UnicodeAware{}) == "Nazara");
		CHECK(Nz::GetWord(sentence, 4, Nz::UnicodeAware{}) == "Engine");
		CHECK(Nz::GetWord(sentence, 5, Nz::UnicodeAware{}) == "!");
		CHECK(Nz::GetWord(sentence, 6, Nz::UnicodeAware{}).empty());
	}

	WHEN("Checking if string is number")
	{
		CHECK(Nz::IsNumber("123456"));
		CHECK(Nz::IsNumber("-123456"));
		CHECK_FALSE(Nz::IsNumber("123 "));
		CHECK_FALSE(Nz::IsNumber("Nazara Engine"));
		CHECK_FALSE(Nz::IsNumber("12345Nazara Engine"));
	}

	WHEN("Matching patterns")
	{
		CHECK(Nz::MatchPattern("Lynix", "?????"));
		CHECK(Nz::MatchPattern("Lynix", "*Lynix"));
		CHECK(Nz::MatchPattern("Lynox", "*Lyn?x"));
		CHECK_FALSE(Nz::MatchPattern("", "?"));
		CHECK_FALSE(Nz::MatchPattern("", "*"));

		const char* pattern = "Naz?ra *gine";
		CHECK(Nz::MatchPattern("Nazara Engine", pattern));
		CHECK(Nz::MatchPattern("Nazora engine", pattern));
		CHECK(Nz::MatchPattern("Nazora Biggine", pattern));
		CHECK(Nz::MatchPattern("Nazora gine", pattern));

		CHECK_FALSE(Nz::MatchPattern("Nazaragine", pattern));
		CHECK_FALSE(Nz::MatchPattern("Nazorra Engine", pattern));
		CHECK_FALSE(Nz::MatchPattern("Nazra Engine", pattern));
		CHECK_FALSE(Nz::MatchPattern("NazaraEngine", pattern));
	}

	WHEN("Converting pointers to string")
	{
		CHECK(Nz::TrimRight(Nz::PointerToString(nullptr), '0') == "0x");

		const void* ptr = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
		CHECK(Nz::MatchPattern(Nz::PointerToString(ptr), "0x*DEADBEEF"));
	}

	WHEN("Replacing strings")
	{
		std::string str = "Nazara Engine";
		REQUIRE(Nz::ReplaceStr(str, "Nazara", "Unreal") == "Unreal Engine");
		REQUIRE(Nz::ReplaceStr(str, "Engine", "Reality") == "Unreal Reality");
		REQUIRE(Nz::ReplaceStr(str, "Unreal Reality", "Ungine") == "Ungine");
	}

	WHEN("Checking if string starts with")
	{
		CHECK(Nz::StartsWith("Nazara Engine", "Nazara"));
		CHECK(Nz::StartsWith("NAZARA Engine", "NaZaRa", Nz::CaseIndependent{}));
		CHECK(Nz::StartsWith(u8"L'\u00CEle de R\u00E9", u8"l'\u00EEle", Nz::CaseIndependent{}, Nz::UnicodeAware{}));
	}

	WHEN("Converting between lower and upper")
	{
		CHECK(Nz::ToLower("Nazara Engine") == "nazara engine");
		CHECK(Nz::ToLower(u8"L'\u00CELE DE R\u00C9", Nz::UnicodeAware{}) == u8"l'\u00EEle de r\u00E9");
		CHECK(Nz::ToUpper("Nazara Engine") == "NAZARA ENGINE");
		CHECK(Nz::ToUpper(u8"l'\u00EEle de r\u00E9", Nz::UnicodeAware{}) == u8"L'\u00CELE DE R\u00C9");
	}

	WHEN("Trimming strings")
	{
		CHECK(Nz::Trim(" \n  Hello World\t") == "Hello World");
		CHECK(Nz::Trim("Nazara Engin", 'n') == "Nazara Engi");
		CHECK(Nz::Trim("Nazara Engin", 'n', Nz::CaseIndependent{}) == "azara Engi");
		CHECK(Nz::Trim(unicodeString, Nz::UnicodeAware{}) == unicodeString);
		CHECK(Nz::Trim("\n\t" + unicodeString + "\t ", Nz::UnicodeAware{}) == unicodeString);
		CHECK(Nz::Trim(unicodeString, U'\u46E1', Nz::UnicodeAware{}) == u8"\u00E0\u00E9\u00E7\u0153\u00C2\u5B98");
		CHECK(Nz::Trim(unicodeString, Nz::Unicode::Category_Letter, Nz::UnicodeAware{}) == "");
	}
}
