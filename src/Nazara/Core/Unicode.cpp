// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Core/Config.hpp>
#include <algorithm>
#include <Nazara/Core/Debug.hpp>

#if NAZARA_CORE_INCLUDE_UNICODEDATA
namespace Nz
{
	struct UnicodeCharacter
	{
		UInt32 codepoint;
		Unicode::Category category;  // The type of the character
		Unicode::Direction direction; // The reading way of the character
	};

	struct UnicodeSet
	{
		UInt32 firstCodepoint;
		UInt32 lastCodepoint;
		UnicodeCharacter character;
	};

	struct UnicodeCharacterSimpleMapping
	{
		UInt32 codepoint;
		UInt32 character;
	};

#include <Nazara/Core/UnicodeData.hpp>

	namespace
	{
		const UnicodeCharacter* GetCharacter(Nz::UInt32 codepoint)
		{
			auto it = std::lower_bound(std::begin(unicodeCharacters), std::end(unicodeCharacters), codepoint, [](const UnicodeCharacter& character, Nz::UInt32 codepoint) { return character.codepoint < codepoint; });
			if (it != std::end(unicodeCharacters) && it->codepoint == codepoint)
				return &*it;
			else
			{
				// Character is not part of the common character array, search in set
				auto itSet = std::lower_bound(std::begin(unicodeSets), std::end(unicodeSets), codepoint, [](const UnicodeSet& character, Nz::UInt32 codepoint) { return character.firstCodepoint < codepoint; });
				if (itSet != std::begin(unicodeSets))
				{
					--itSet;
					if (itSet != std::end(unicodeSets) && codepoint >= itSet->firstCodepoint && codepoint <= itSet->lastCodepoint)
						return &itSet->character;
				}
			}

			return nullptr;
		}

		template<std::size_t N>
		const UnicodeCharacterSimpleMapping* GetCharacterMapping(Nz::UInt32 codepoint, const UnicodeCharacterSimpleMapping(&mapping)[N])
		{
			auto it = std::lower_bound(std::begin(mapping), std::end(mapping), codepoint, [](const UnicodeCharacterSimpleMapping& character, Nz::UInt32 codepoint) { return character.codepoint < codepoint; });
			if (it != std::end(mapping) && it->codepoint == codepoint)
				return &*it;
			else
				return nullptr;
		}
	}

	/*!
	* \brief Gets the category of the character
	* \return Unicode category
	*
	* \param character Character to get assignated category
	*/
	Unicode::Category Unicode::GetCategory(char32_t character)
	{
		if (const UnicodeCharacter* characterData = GetCharacter(character))
			return characterData->category;
		else
			return Category_NoCategory;
	}

	/*!
	* \brief Gets the direction of reading of the character
	* \return Unicode direction
	*
	* \param character Character to get assignated direction
	*/

	Unicode::Direction Unicode::GetDirection(char32_t character)
	{
		if (const UnicodeCharacter* characterData = GetCharacter(character))
			return characterData->direction;
		else
			return Direction_Boundary_Neutral;
	}

	/*!
	* \brief Gets the lower case of the character
	* \return Unicode lower
	*
	* \param character Character to get assignated lower case
	*/

	char32_t Unicode::GetLowercase(char32_t character)
	{
		if (const UnicodeCharacterSimpleMapping* characterMapping = GetCharacterMapping(character, unicodeLower))
			return characterMapping->character;
		else
			return character;
	}

	/*!
	* \brief Gets the title case of the character
	* \return Unicode title
	*
	* \param character Character to get assignated title case
	*/
	char32_t Unicode::GetTitlecase(char32_t character)
	{
		if (const UnicodeCharacterSimpleMapping* characterMapping = GetCharacterMapping(character, unicodeTitle))
			return characterMapping->character;
		else
			return character;
	}

	/*!
	* \brief Gets the upper case of the character
	* \return Unicode upper
	*
	* \param character Character to get assignated upper case
	*/
	char32_t Unicode::GetUppercase(char32_t character)
	{
		if (const UnicodeCharacterSimpleMapping* characterMapping = GetCharacterMapping(character, unicodeUpper))
			return characterMapping->character;
		else
			return character;
	}
}

#else // Implementation handling ASCII table

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Unicode
	* \brief Core class that represents a Unicode character
	*/

	/*!
	* \brief Gets the category of the character
	* \return Unicode category
	*
	* \param character Character to get assignated category
	*/

	Unicode::Category Unicode::GetCategory(char32_t character)
	{
		switch (character)
		{
			case '\x00':
			case '\x01':
			case '\x02':
			case '\x03':
			case '\x04':
			case '\x05':
			case '\x06':
			case '\x07':
			case '\x08':
			case '\x09':
			case '\x0A':
			case '\x0B':
			case '\x0C':
			case '\x0D':
			case '\x0E':
			case '\x0F':
			case '\x10':
			case '\x11':
			case '\x12':
			case '\x13':
			case '\x14':
			case '\x15':
			case '\x16':
			case '\x17':
			case '\x18':
			case '\x19':
			case '\x1A':
			case '\x1B':
			case '\x1C':
			case '\x1D':
			case '\x1E':
			case '\x1F':
			case '\x7F':
				return Category_Other_Control;

			case ' ':
				return Category_Separator_Space;

			case '!':
			case '"':
			case '#':
			case '$':
			case '%':
			case '&':
			case '\'':
			case '*':
			case ',':
			case '.':
			case '/':
			case ':':
			case ';':
			case '?':
			case '@':
			case '\\':
				return Category_Punctuation_Other;

			case '(':
			case '[':
			case '{':
				return Category_Punctuation_Open;

			case ')':
			case '}':
			case ']':
				return Category_Punctuation_Close;

			case '+':
			case '<':
			case '=':
			case '>':
			case '|':
			case '~':
				return Category_Symbol_Math;

			case '-':
				return Category_Punctuation_Dash;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return Category_Number_DecimalDigit;

			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
				return Category_Letter_Uppercase;

			case '_':
				return Category_Punctuation_Connector;

			case '^':
			case '`':
				return Category_Symbol_Modifier;

			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
				return Category_Letter_Lowercase;

			default:
				break;
		}

		return Category_NoCategory;
	}

	/*!
	* \brief Gets the direction of reading of the character
	* \return Unicode direction
	*
	* \param character Character to get assignated direction
	*/

	Unicode::Direction Unicode::GetDirection(char32_t character)
	{
		switch (character)
		{
			case '\x00':
			case '\x01':
			case '\x02':
			case '\x03':
			case '\x04':
			case '\x05':
			case '\x06':
			case '\x07':
			case '\x08':
			case '\x0E':
			case '\x0F':
			case '\x10':
			case '\x11':
			case '\x12':
			case '\x13':
			case '\x14':
			case '\x15':
			case '\x16':
			case '\x17':
			case '\x18':
			case '\x19':
			case '\x1A':
			case '\x1B':
			case '\x7F':
				return Direction_Boundary_Neutral;

			case '\x09':
			case '\x0B':
			case '\x1F':
				return Direction_Segment_Separator;

			case '\x0A':
			case '\x0D':
			case '\x1C':
			case '\x1D':
			case '\x1E':
				return Direction_Paragraph_Separator;

			case '\x0C':
			case ' ':
				return Direction_White_Space;

			case '!':
			case '"':
			case '&':
			case '\'':
			case '(':
			case ')':
			case '*':
			case ';':
			case '<':
			case '=':
			case '>':
			case '?':
			case '@':
			case '[':
			case '\\':
			case ']':
			case '^':
			case '_':
			case '`':
			case '{':
			case '|':
			case '}':
			case '~':
				return Direction_Other_Neutral;

			case '#':
			case '$':
			case '%':
				return Direction_European_Terminator;

			case '+':
			case '-':
				return Direction_European_Separator;

			case ',':
			case '.':
			case '/':
			case ':':
				return Direction_Common_Separator;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return Direction_European_Number;

			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
				return Direction_Left_To_Right;

			default:
				break;
		}

		return Direction_Boundary_Neutral;
	}

	/*!
	* \brief Gets the lower case of the character
	* \return Unicode lower
	*
	* \param character Character to get assignated lower case
	*
	* \remark Only handling ASCII
	*/

	char32_t Unicode::GetLowercase(char32_t character)
	{
		if (character >= 'A' && character <= 'Z')
			return character + ('a' - 'A');
		else
			return character;
	}

	/*!
	* \brief Gets the title case of the character
	* \return Unicode title
	*
	* \param character Character to get assignated title case
	*
	* \remark Only handling ASCII
	*/

	char32_t Unicode::GetTitlecase(char32_t character)
	{
		return GetUppercase(character);
	}

	/*!
	* \brief Gets the upper case of the character
	* \return Unicode upper
	*
	* \param character Character to get assignated upper case
	*
	* \remark Only handling ASCII
	*/

	char32_t Unicode::GetUppercase(char32_t character)
	{
		if (character >= 'a' && character <= 'z')
			return character + ('A' - 'a');
		else
			return character;
	}
}

#endif
