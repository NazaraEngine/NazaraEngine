// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Debug.hpp>

#if NAZARA_CORE_INCLUDE_UNICODEDATA
struct Character
{
	nzUInt16 category;	// Le type du caractère
	nzUInt8	 direction;	// Le sens de lecure du caractère
	nzUInt32 lowerCase;	// Le caractère correspondant en minuscule
	nzUInt32 titleCase;	// Le caractère correspondant en titre
	nzUInt32 upperCase;	// Le caractère correspondant en majuscule
};

#include <Nazara/Core/UnicodeData.hpp>

#else // Implémentation bidon

NzUnicode::Category NzUnicode::GetCategory(char32_t character)
{
	NazaraUnused(character);

	return Category_NoCategory;
}

NzUnicode::Direction NzUnicode::GetDirection(char32_t character)
{
	NazaraUnused(character);

	return Direction_Boundary_Neutral;
}

char32_t NzUnicode::GetLowercase(char32_t character)
{
	return character;
}

char32_t NzUnicode::GetTitlecase(char32_t character)
{
	return character;
}

char32_t NzUnicode::GetUppercase(char32_t character)
{
	return character;
}
#endif
