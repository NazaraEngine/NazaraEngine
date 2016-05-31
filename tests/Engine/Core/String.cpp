#include <Nazara/Core/String.hpp>
#include <Catch/catch.hpp>

SCENARIO("String", "[CORE][STRING]")
{
	GIVEN("One string 'a'")
	{
		Nz::String aDefaultString(1, 'a');

		WHEN("We add information")
		{
			aDefaultString.Append("Default");
			aDefaultString.Insert(aDefaultString.GetSize(), "String");

			THEN("The result should be 'aDefaultString'")
			{
				REQUIRE(aDefaultString == "aDefaultString");
				REQUIRE(aDefaultString.GetSize() == 14);
				REQUIRE(aDefaultString.GetCapacity() >= 14);
			}

			AND_WHEN("We test Contains and Find")
			{
				THEN("These results are expected")
				{
					CHECK(aDefaultString.Contains('D'));
					CHECK(aDefaultString.Contains("String", 3));
					CHECK(aDefaultString.Contains(Nz::String("sTRING"), 3, Nz::String::CaseInsensitive));
					REQUIRE(aDefaultString.FindLast('g') == aDefaultString.GetSize() - 1);
					CHECK(aDefaultString.EndsWith('G', Nz::String::CaseInsensitive));
					aDefaultString.Append(" ng bla");
					REQUIRE(aDefaultString.FindWord("ng") == aDefaultString.GetSize() - 6);
					//TODO REQUIRE(aDefaultString.FindWord(String("ng")) == aDefaultString.GetSize() - 6);
					CHECK(aDefaultString.StartsWith("aD"));
				}
			}
		}

		WHEN("We do operators")
		{
			aDefaultString[0] = 'a';
			aDefaultString += "Default";
			aDefaultString = aDefaultString + "String";

			THEN("The result should be 'aDefaultString'")
			{
				REQUIRE(aDefaultString == "aDefaultString");
				REQUIRE(aDefaultString.GetSize() == 14);
				REQUIRE(aDefaultString.GetCapacity() >= 14);
			}

			AND_WHEN("We test Count")
			{
				THEN("These results are expected")
				{
					REQUIRE(aDefaultString.Count('D') == 1);
					REQUIRE(aDefaultString.Count("t", 2) == 2);
				}
			}
		}
	}

	GIVEN("The string of number 16 in base 16")
	{
		Nz::String number16;

		CHECK(number16.IsEmpty());
		CHECK(number16.IsNull());

		WHEN("We assign to number 16")
		{
			number16 = Nz::String::Number(16, 16);

			THEN("These results are expected")
			{
				CHECK(number16.IsNumber(16));
				number16.Prepend("0x");
				REQUIRE(number16.GetSize() == 4);
				REQUIRE(number16.GetCapacity() >= 4);
				REQUIRE(number16.SubStringFrom('x', 1) == "10");
			}
		}
	}

	GIVEN("One character string")
	{
		Nz::String characterString;

		WHEN("We set the string to one character")
		{
			characterString.Set('/');

			THEN("The string must contain it")
			{
				REQUIRE(characterString == '/');
			}
		}
	}

	/* TODO
	GIVEN("One unicode string")
	{
		String unicodeString = String::Unicode(U"àéçœÂ官話");

		WHEN("We convert to other UTF")
		{
			REQUIRE(unicodeString.GetSize() == 7);
			REQUIRE(unicodeString.GetCapacity() >= 7);
			CHECK(unicodeString.Contains("官"));

			THEN("The result should be the identity")
			{
				char* utf8 = unicodeString.GetUtf8Buffer();
				String utf8String = String::Unicode(utf8);
				char16_t* utf16 = unicodeString.GetUtf16Buffer();
				String utf16String = String::Unicode(utf16);
				char32_t* utf32 = unicodeString.GetUtf32Buffer();
				String utf32String = String::Unicode(utf32);

				REQUIRE(utf8String == utf16String);
				REQUIRE(utf16String == utf32String);
			}
		}
	}*/

	GIVEN("A string")
	{
		Nz::String replaceAny("abapeilomuky");
		Nz::String replaceAnyWithCase("abapEilOmuky");

		WHEN("We replace any of vowels after character 3")
		{
			unsigned int nbrOfChanges = replaceAny.ReplaceAny("aeiouy", '$', 3);
			unsigned int nbrOfChangesWithCase = replaceAnyWithCase.ReplaceAny("AEIOUY", '$', 3);

			THEN("These results are expected")
			{
				REQUIRE(replaceAny == "abap$$l$m$k$");
				REQUIRE(nbrOfChanges == 5);
				REQUIRE(replaceAnyWithCase == "abap$il$muky");
				REQUIRE(nbrOfChangesWithCase == 2);
			}
		}
	}
}

