// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

///TODO: Rewrite most of used algorithms (Reread to to line 4638)

#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <limits>
#include <sstream>
#include <Utfcpp/utf8.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		inline bool IsSpace(char32_t character)
		{
			return character == '\t' || Unicode::GetCategory(character) & Unicode::Category_Separator;
		}

		// This algorithm is inspired by the documentation of Qt
		inline std::size_t GetNewSize(std::size_t newSize)
		{
			if (newSize < 20)
				return newSize+4;
			else
			{
				if (newSize < (1 << 12)-12)
					return GetNearestPowerOfTwo(newSize << 1)-12;
				else
					return newSize + (1 << 11);
			}
		}

		inline char ToLower(char character)
		{
			if (character >= 'A' && character <= 'Z')
				return character + ('a' - 'A');
			else
				return character;
		}

		inline char ToUpper(char character)
		{
			if (character >= 'a' && character <= 'z')
				return character + ('A' - 'a');
			else
				return character;
		}

		inline int Strcasecmp(const char* s1, const char* s2)
		{
			int ret = 0;

			while (((ret = static_cast<unsigned char>(Detail::ToLower(*s1)) - static_cast<unsigned char>(Detail::ToLower(*s2))) != 0) && *s2)
				++s1, ++s2;

			return ret != 0 ? (ret > 0 ? 1 : -1) : 0;
		}

		inline int Unicodecasecmp(const char* s1, const char* s2)
		{
			int ret = 0;
			utf8::unchecked::iterator<const char*> it1(s1);
			utf8::unchecked::iterator<const char*> it2(s2);

			while (((ret = Unicode::GetLowercase(*it1) - Unicode::GetLowercase(*it2)) != 0) && *it2)
				++it1, ++it2;

			return ret != 0 ? (ret > 0 ? 1 : -1) : 0;
		}
	}

	/*!
	* \ingroup core
	* \class Nz::String
	* \brief Core class that represents a string
	*/

	/*!
	* \brief Constructs a String object by default
	*/

	String::String() :
	m_sharedString(GetEmptyString())
	{
	}

	/*!
	* \brief Constructs a String object with a character
	*
	* \param character Single character
	*/

	String::String(char character)
	{
		if (character != '\0')
		{
			m_sharedString = std::make_shared<SharedString>(1);
			m_sharedString->string[0] = character;
		}
		else
			m_sharedString = GetEmptyString();
	}

	/*!
	* \brief Constructs a String object with multiple times the same character
	*
	* \param rep Number of repetitions of the character
	* \param character Single character
	*/

	String::String(std::size_t rep, char character)
	{
		if (rep > 0)
		{
			m_sharedString = std::make_shared<SharedString>(rep);

			if (character != '\0')
				std::memset(m_sharedString->string.get(), character, rep);
		}
		else
			m_sharedString = GetEmptyString();
	}

	/*!
	* \brief Constructs a String object with multiple times the same string
	*
	* \param rep Number of repetitions of the string
	* \param string String to multiply
	*/

	String::String(std::size_t rep, const char* string) :
	String(rep, string, (string) ? std::strlen(string) : 0)
	{
	}

	/*!
	* \brief Constructs a String object with multiple times the same string
	*
	* \param rep Number of repetitions of the string
	* \param string String to multiply
	* \param length Length of the string
	*/

	String::String(std::size_t rep, const char* string, std::size_t length)
	{
		std::size_t totalSize = rep*length;

		if (totalSize > 0)
		{
			m_sharedString = std::make_shared<SharedString>(totalSize);

			for (std::size_t i = 0; i < rep; ++i)
				std::memcpy(&m_sharedString->string[i*length], string, length);
		}
		else
			m_sharedString = GetEmptyString();
	}

	/*!
	* \brief Constructs a String object with multiple times the same string
	*
	* \param rep Number of repetitions of the string
	* \param string String to multiply
	*/

	String::String(std::size_t rep, const String& string) :
	String(rep, string.GetConstBuffer(), string.GetSize())
	{
	}

	/*!
	* \brief Constructs a String object with a "C string"
	*
	* \param string String to represent
	*/

	String::String(const char* string) :
	String(string, (string) ? std::strlen(string) : 0)
	{
	}

	/*!
	* \brief Constructs a String object with a "C string"
	*
	* \param string String to represent
	* \param length Length of the string
	*/

	String::String(const char* string, std::size_t length)
	{
		if (length > 0)
		{
			m_sharedString = std::make_shared<SharedString>(length);
			std::memcpy(m_sharedString->string.get(), string, length);
		}
		else
			m_sharedString = GetEmptyString();
	}

	/*!
	* \brief Constructs a String object which is a copy of another
	*
	* \param string String to copy
	*/

	String::String(const std::string& string) :
	String(string.data(), string.size())
	{
	}

	/*!
	* \brief Appends the character to the string
	* \return A reference to this
	*
	* \param character Single character
	*
	* \see Insert
	*/

	String& String::Append(char character)
	{
		return Insert(m_sharedString->size, character);
	}

	/*!
	* \brief Appends the "C string" to the string
	* \return A reference to this
	*
	* \param string String to add
	*
	* \see Insert
	*/

	String& String::Append(const char* string)
	{
		return Insert(m_sharedString->size, string);
	}

	/*!
	* \brief Appends the "C string" to the string
	* \return A reference to this
	*
	* \param string String to add
	* \param length Size of the string
	*
	* \see Insert
	*/

	String& String::Append(const char* string, std::size_t length)
	{
		return Insert(m_sharedString->size, string, length);
	}

	/*!
	* \brief Appends the string to the string
	* \return A reference to this
	*
	* \param string String to add
	*
	* \see Insert
	*/

	String& String::Append(const String& string)
	{
		return Insert(m_sharedString->size, string);
	}

	/*!
	* \brief Clears the content of the string
	*
	* \param keepBuffer Should the buffer be kept
	*/

	void String::Clear(bool keepBuffer)
	{
		if (keepBuffer)
		{
			EnsureOwnership(true);
			m_sharedString->size = 0;
		}
		else
			ReleaseString();
	}

	/*!
	* \brief Checks whether the string contains the character
	* \return true if found in the string
	*
	* \param character Single character
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*
	* \see Find
	*/

	bool String::Contains(char character, std::intmax_t start, UInt32 flags) const
	{
		return Find(character, start, flags) != npos;
	}

	/*!
	* \brief Checks whether the string contains the "C string"
	* \return true if found in the string
	*
	* \param string String to search
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*
	* \see Find
	*/

	bool String::Contains(const char* string, std::intmax_t start, UInt32 flags) const
	{
		return Find(string, start, flags) != npos;
	}

	/*!
	* \brief Checks whether the string contains the string
	* \return true if found in the string
	*
	* \param string String to search
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*
	* \see Find
	*/

	bool String::Contains(const String& string, std::intmax_t start, UInt32 flags) const
	{
		return Find(string, start, flags) != npos;
	}

	/*!
	* \brief Counts the number of occurrences in the string
	* \return Number of occurrences
	*
	* \param character Single character
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*/

	unsigned int String::Count(char character, std::intmax_t start, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return 0;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return 0;

		char* str = &m_sharedString->string[pos];
		unsigned int count = 0;
		if (flags & CaseInsensitive)
		{
			char character_lower = Detail::ToLower(character);
			char character_upper = Detail::ToUpper(character);
			do
			{
				if (*str == character_lower || *str == character_upper)
					count++;
			}
			while (*++str);
		}
		else
		{
			while ((str = std::strchr(str, character)) != nullptr)
			{
				count++;
				str++;
			}
		}

		return count;
	}

	/*!
	* \brief Counts the number of occurrences in the string
	* \return Number of occurrences
	*
	* \param string String to count
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*/

	unsigned int String::Count(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return 0;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return 0;

		char* str = &m_sharedString->string[pos];
		unsigned int count = 0;
		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				while (utf8::internal::is_trail(*str))
					str++;

				utf8::unchecked::iterator<const char*> it(str);

				const char* t = string;
				char32_t c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						++it;

						utf8::unchecked::iterator<const char*> it2(t);
						for (;;)
						{
							if (*it2 == '\0')
							{
								count++;
								break;
							}

							if (*it == '\0')
								return count;

							if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
								break;

							++it;
							++it2;
						}
					}
				}
				while (*++it);
			}
			else
			{
				char c = Detail::ToLower(string[0]);
				do
				{
					if (Detail::ToLower(*str) == c)
					{
						str++;

						const char* ptr = &string[1];
						for (;;)
						{
							if (*ptr == '\0')
							{
								count++;
								break;
							}

							if (*str == '\0')
								return count;

							if (Detail::ToLower(*str) != Detail::ToLower(*ptr))
								break;

							ptr++;
							str++;
						}
					}
				}
				while (*++str);
			}
		}
		else
		{
			while ((str = std::strstr(str, string)) != nullptr)
			{
				count++;
				str++;
			}
		}

		return count;
	}

	/*!
	* \brief Counts the number of occurrences in the string
	* \return Number of occurrences
	*
	* \param string String to count
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*/

	unsigned int String::Count(const String& string, std::intmax_t start, UInt32 flags) const
	{
		return Count(string.GetConstBuffer(), start, flags);
	}

	/*!
	* \brief Counts the number of occurrences of any characters in the list in the string
	* \return Number of occurrences
	*
	* \param string String to match
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*/

	unsigned int String::CountAny(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return 0;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return 0;

		char* str = &m_sharedString->string[pos];
		unsigned int count = 0;
		if (flags & HandleUtf8)
		{
			while (utf8::internal::is_trail(*str))
				str++;

			utf8::unchecked::iterator<const char*> it(str);

			if (flags & CaseInsensitive)
			{
				do
				{
					utf8::unchecked::iterator<const char*> it2(string);
					do
					{
						if (Unicode::GetLowercase(*it) == Unicode::GetLowercase(*it2))
						{
							count++;
							break;
						}
					}
					while (*++it2);
				}
				while (*++str);
			}
			else
			{
				do
				{
					utf8::unchecked::iterator<const char*> it2(string);
					do
					{
						if (*it == *it2)
						{
							count++;
							break;
						}
					}
					while (*++it2);
				}
				while (*++str);
			}
		}
		else
		{
			if (flags & CaseInsensitive)
			{
				do
				{
					const char* c = string;
					do
					{
						if (Detail::ToLower(*str) == Detail::ToLower(*c))
						{
							count++;
							break;
						}
					}
					while (*++c);
				}
				while (*++str);
			}
			else
			{
				while ((str = std::strpbrk(str, string)) != nullptr)
				{
					count++;
					str++;
				}
			}
		}

		return count;
	}

	/*!
	* \brief Counts the number of occurrences of any characters in the list in the string
	* \return Number of occurrences
	*
	* \param string String to match
	* \param start Index to begin the research
	* \param flags Flag for the look up
	*/

	unsigned int String::CountAny(const String& string, std::intmax_t start, UInt32 flags) const
	{
		return CountAny(string.GetConstBuffer(), start, flags);
	}

	/*!
	* \brief Checks whether the string ends with the character
	* \return true if it the case
	*
	* \param character Single character
	* \param flags Flag for the look up
	*
	* \see StartsWith
	*/

	bool String::EndsWith(char character, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return 0;

		if (flags & CaseInsensitive)
			return Detail::ToLower(m_sharedString->string[m_sharedString->size-1]) == Detail::ToLower(character);
		else
			return m_sharedString->string[m_sharedString->size-1] == character; // character == '\0' will always be false
	}

	/*!
	* \brief Checks whether the string ends with the "C string"
	* \return true if it the case
	*
	* \param string String to match
	* \param flags Flag for the look up
	*
	* \see StartsWith
	*/

	bool String::EndsWith(const char* string, UInt32 flags) const
	{
		return EndsWith(string, std::strlen(string), flags);
	}

	/*!
	* \brief Checks whether the string ends with the "C string"
	* \return true if it the case
	*
	* \param string String to match
	* \param length Size of the string
	* \param flags Flag for the look up
	*
	* \see StartsWith
	*/

	bool String::EndsWith(const char* string, std::size_t length, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0 || length > m_sharedString->size)
			return false;

		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
				return Detail::Unicodecasecmp(&m_sharedString->string[m_sharedString->size - length], string) == 0;
			else
				return Detail::Strcasecmp(&m_sharedString->string[m_sharedString->size - length], string) == 0;
		}
		else
			return std::strcmp(&m_sharedString->string[m_sharedString->size - length], string) == 0;
	}

	/*!
	* \brief Checks whether the string ends with the string
	* \return true if it the case
	*
	* \param string String to match
	* \param flags Flag for the look up
	*
	* \see StartsWith
	*/

	bool String::EndsWith(const String& string, UInt32 flags) const
	{
		return EndsWith(string.GetConstBuffer(), string.m_sharedString->size, flags);
	}

	/*!
	* \brief Finds the first index of the character in the string
	* \return Index in the string
	*
	* \param character Single character
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::Find(char character, std::intmax_t start, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		if (flags & CaseInsensitive)
		{
			char ch = Detail::ToLower(character);
			const char* str = m_sharedString->string.get();
			do
			{
				if (Detail::ToLower(*str) == ch)
					return str - m_sharedString->string.get();
			}
			while (*++str);

			return npos;
		}
		else
		{
			char* ch = std::strchr(&m_sharedString->string[pos], character);
			if (ch)
				return ch - m_sharedString->string.get();
			else
				return npos;
		}
	}

	/*!
	* \brief Finds the first index of the "C string" in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::Find(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* str = &m_sharedString->string[pos];
		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				while (utf8::internal::is_trail(*str))
					str++;

				utf8::unchecked::iterator<const char*> it(str);

				const char* t = string;
				char32_t c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						const char* ptrPos = it.base();
						++it;

						utf8::unchecked::iterator<const char*> it2(t);
						for (;;)
						{
							if (*it2 == '\0')
								return ptrPos - m_sharedString->string.get();

							if (*it == '\0')
								return npos;

							if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
								break;

							++it;
							++it2;
						}
					}
				}
				while (*++it);
			}
			else
			{
				char c = Detail::ToLower(string[0]);
				do
				{
					if (Detail::ToLower(*str) == c)
					{
						char* ptrPos = str;
						str++;

						const char* ptr = &string[1];
						for (;;)
						{
							if (*ptr == '\0')
								return ptrPos - m_sharedString->string.get();

							if (*str == '\0')
								return npos;

							if (Detail::ToLower(*str) != Detail::ToLower(*ptr))
								break;

							ptr++;
							str++;
						}
					}
				}
				while (*++str);
			}
		}
		else
		{
			char* ch = std::strstr(&m_sharedString->string[pos], string);
			if (ch)
				return ch - m_sharedString->string.get();
		}

		return npos;
	}

	/*!
	* \brief Finds the first index of the string in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::Find(const String& string, std::intmax_t start, UInt32 flags) const
	{
		return Find(string.GetConstBuffer(), start, flags);
	}

	/*!
	* \brief Finds the first index of any characters in the list in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindAny(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (m_sharedString->size == 0 || !string || !string[0])
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* str = &m_sharedString->string[pos];
		if (flags & HandleUtf8)
		{
			while (utf8::internal::is_trail(*str))
				str++;

			utf8::unchecked::iterator<const char*> it(str);

			if (flags & CaseInsensitive)
			{
				do
				{
					utf8::unchecked::iterator<const char*> it2(string);
					char32_t character = Unicode::GetLowercase(*it);
					do
					{
						if (character == Unicode::GetLowercase(*it2))
							return it.base() - m_sharedString->string.get();
					}
					while (*++it2);
				}
				while (*++it);
			}
			else
			{
				do
				{
					utf8::unchecked::iterator<const char*> it2(string);
					do
					{
						if (*it == *it2)
							return it.base() - m_sharedString->string.get();
					}
					while (*++it2);
				}
				while (*++it);
			}
		}
		else
		{
			if (flags & CaseInsensitive)
			{
				do
				{
					const char* c = string;
					char character = Detail::ToLower(*str);
					do
					{
						if (character == Detail::ToLower(*c))
							return str - m_sharedString->string.get();
					}
					while (*++c);
				}
				while (*++str);
			}
			else
			{
				str = std::strpbrk(str, string);
				if (str)
					return str - m_sharedString->string.get();
			}
		}

		return npos;
	}

	/*!
	* \brief Finds the first index of any characters in the list in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindAny(const String& string, std::intmax_t start, UInt32 flags) const
	{
		return FindAny(string.GetConstBuffer(), start, flags);
	}

	/*!
	* \brief Finds the last index of the character in the string
	* \return Index in the string
	*
	* \param character Single character
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLast(char character, std::intmax_t start, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* ptr = &m_sharedString->string[pos];

		if (flags & CaseInsensitive)
		{
			character = Detail::ToLower(character);
			do
			{
				if (Detail::ToLower(*ptr) == character)
					return ptr - m_sharedString->string.get();
			}
			while (ptr-- != m_sharedString->string.get());
		}
		else
		{
			do
			{
				if (*ptr == character)
					return ptr - m_sharedString->string.get();
			}
			while (ptr-- != m_sharedString->string.get());
		}

		return npos;
	}

	/*!
	* \brief Finds the last index of the "C string" in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLast(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		///Algo 1.FindLast#3 (Size of the pattern unknown)
		const char* ptr = &m_sharedString->string[pos];
		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				if (utf8::internal::is_trail(*ptr))
					utf8::unchecked::prior(ptr); // We ensure to have one pointer pointing to the begin of the character

				utf8::unchecked::iterator<const char*> it(ptr);
				const char* t = string;
				char32_t c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						utf8::unchecked::iterator<const char*> it2(t);
						utf8::unchecked::iterator<const char*> tIt(it);
						++tIt;

						for (;;)
						{
							if (*it2 == '\0')
								return it.base() - m_sharedString->string.get();

							if (tIt.base() > &m_sharedString->string[pos])
								break;

							if (Unicode::GetLowercase(*tIt) != Unicode::GetLowercase(*it2))
								break;

							++it2;
							++tIt;
						}
					}
				}
				while (it--.base() != m_sharedString->string.get());
			}
			else
			{
				char c = Detail::ToLower(string[0]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						const char* p = &string[1];
						const char* tPtr = ptr+1;
						for (;;)
						{
							if (*p == '\0')
								return ptr - m_sharedString->string.get();

							if (tPtr > &m_sharedString->string[pos])
								break;

							if (Detail::ToLower(*tPtr) != Detail::ToLower(*p))
								break;

							p++;
							tPtr++;
						}
					}
				}
				while (ptr-- != m_sharedString->string.get());
			}
		}
		else
		{
			do
			{
				if (*ptr == string[0])
				{
					const char* p = &string[1];
					const char* tPtr = ptr+1;
					for (;;)
					{
						if (*p == '\0')
							return ptr - m_sharedString->string.get();

						if (tPtr > &m_sharedString->string[pos])
							break;

						if (*tPtr != *p)
							break;

						p++;
						tPtr++;
					}
				}
			}
			while (ptr-- != m_sharedString->string.get());
		}

		return npos;
	}

	/*!
	* \brief Finds the last index of the string in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLast(const String& string, std::intmax_t start, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size || string.m_sharedString->size > m_sharedString->size)
			return npos;

		const char* ptr = &m_sharedString->string[pos];
		const char* limit = &m_sharedString->string[string.m_sharedString->size-1];

		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				///Algo 1.FindLast#3 (Iterator non-adapted)
				if (utf8::internal::is_trail(*ptr))
					utf8::unchecked::prior(ptr); // We ensure to have one pointer pointing to the begin of the character

				utf8::unchecked::iterator<const char*> it(ptr);
				const char* t = string.GetConstBuffer();
				char32_t c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						utf8::unchecked::iterator<const char*> it2(t);
						utf8::unchecked::iterator<const char*> tIt(it);
						++tIt;

						for (;;)
						{
							if (*it2 == '\0')
								return it.base() - m_sharedString->string.get();

							if (tIt.base() > &m_sharedString->string[pos])
								break;

							if (Unicode::GetLowercase(*tIt) != Unicode::GetLowercase(*it2))
								break;

							++it2;
							++tIt;
						}
					}
				}
				while (it--.base() != limit);
			}
			else
			{
				///Algo 1.FindLast#4 (Size of the pattern unknown)
				char c = Detail::ToLower(string.m_sharedString->string[string.m_sharedString->size-1]);
				for (;;)
				{
					if (Detail::ToLower(*ptr) == c)
					{
						const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
						for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
						{
							if (Detail::ToLower(*ptr) != Detail::ToLower(*p))
								break;

							if (p == &string.m_sharedString->string[0])
								return ptr-m_sharedString->string.get();

							if (ptr == m_sharedString->string.get())
								return npos;
						}
					}
					else if (ptr-- <= limit)
						break;
				}
			}
		}
		else
		{
			///Algo 1.FindLast#4 (Size of the pattern known)
			for (;;)
			{
				if (*ptr == string.m_sharedString->string[string.m_sharedString->size-1])
				{
					const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
					for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
					{
						if (*ptr != *p)
							break;

						if (p == &string.m_sharedString->string[0])
							return ptr-m_sharedString->string.get();

						if (ptr == m_sharedString->string.get())
							return npos;
					}
				}
				else if (ptr-- <= limit)
					break;
			}
		}

		return npos;
	}

	/*!
	* \brief Finds the last index of any characters in the list in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLastAny(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* str = &m_sharedString->string[pos];
		if (flags & HandleUtf8)
		{
			while (utf8::internal::is_trail(*str))
				str++;

			utf8::unchecked::iterator<const char*> it(str);

			if (flags & CaseInsensitive)
			{
				do
				{
					utf8::unchecked::iterator<const char*> it2(string);
					char32_t character = Unicode::GetLowercase(*it);
					do
					{
						if (character == Unicode::GetLowercase(*it2))
							return it.base() - m_sharedString->string.get();
					}
					while (*++it2);
				}
				while (it--.base() != m_sharedString->string.get());
			}
			else
			{
				do
				{
					utf8::unchecked::iterator<const char*> it2(string);
					do
					{
						if (*it == *it2)
							return it.base() - m_sharedString->string.get();
					}
					while (*++it2);
				}
				while (it--.base() != m_sharedString->string.get());
			}
		}
		else
		{
			if (flags & CaseInsensitive)
			{
				do
				{
					const char* c = string;
					char character = Detail::ToLower(*str);
					do
					{
						if (character == Detail::ToLower(*c))
							return str - m_sharedString->string.get();
					}
					while (*++c);
				}
				while (str-- != m_sharedString->string.get());
			}
			else
			{
				do
				{
					const char* c = string;
					do
					{
						if (*str == *c)
							return str - m_sharedString->string.get();
					}
					while (*++c);
				}
				while (str-- != m_sharedString->string.get());
			}
		}

		return npos;
	}

	/*!
	* \brief Finds the last index of any characters in the list in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLastAny(const String& string, std::intmax_t start, UInt32 flags) const
	{
		return FindLastAny(string.GetConstBuffer(), start, flags);
	}

	/*!
	* \brief Finds the last word in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLastWord(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		///Algo 2.FindLastWord#1 (Size of the pattern unknown)
		const char* ptr = &m_sharedString->string[pos];

		if (flags & HandleUtf8)
		{
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // We ensure to have a pointer pointing to the beginning of the string

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string; // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*tIt))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (tIt.base() > &m_sharedString->string[pos])
								break;

							if (Unicode::GetLowercase(*tIt) != Unicode::GetLowercase(*p))
								break;

							++p;
							++tIt;
						}
					}
				}
				while (it--.base() != m_sharedString->string.get());
			}
			else
			{
				const char* t = string; // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = utf8::unchecked::next(t);
				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*tIt))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (tIt.base() > &m_sharedString->string[pos])
								break;

							if (*tIt != *p)
								break;

							++p;
							++tIt;
						}
					}
				}
				while (it--.base() != m_sharedString->string.get());
			}
		}
		else
		{
			if (flags & CaseInsensitive)
			{
				char c = Detail::ToLower(string[0]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						if (ptr != m_sharedString->string.get())
						{
							--ptr;
							if (!Detail::IsSpace(*ptr++))
								continue;
						}

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						for (;;)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || Detail::IsSpace(*tPtr))
									return ptr-m_sharedString->string.get();
								else
									break;
							}

							if (tPtr > &m_sharedString->string[pos])
								break;

							if (Detail::ToLower(*tPtr) != Detail::ToLower(*p))
								break;

							p++;
							tPtr++;
						}
					}
				}
				while (ptr-- != m_sharedString->string.get());
			}
			else
			{
				do
				{
					if (*ptr == string[0])
					{
						if (ptr != m_sharedString->string.get())
						{
							--ptr;
							if (!Detail::IsSpace(*ptr++))
								continue;
						}

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						for (;;)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || Detail::IsSpace(*tPtr))
									return ptr-m_sharedString->string.get();
								else
									break;
							}

							if (tPtr > &m_sharedString->string[pos])
								break;

							if (*tPtr != *p)
								break;

							p++;
							tPtr++;
						}
					}
				}
				while (ptr-- != m_sharedString->string.get());
			}
		}

		return npos;
	}

	/*!
	* \brief Finds the last word in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindLastWord(const String& string, std::intmax_t start, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		const char* ptr = &m_sharedString->string[pos];
		const char* limit = &m_sharedString->string[string.m_sharedString->size-1];

		if (flags & HandleUtf8)
		{
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // We ensure to have a pointer pointing to the beginning of the string

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*tIt))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (tIt.base() > &m_sharedString->string[pos])
								break;

							if (Unicode::GetLowercase(*tIt) != Unicode::GetLowercase(*p))
								break;

							++p;
							++tIt;
						}
					}
				}
				while (it--.base() != m_sharedString->string.get());
			}
			else
			{
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = utf8::unchecked::next(t);
				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*tIt))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (tIt.base() > &m_sharedString->string[pos])
								break;

							if (*tIt != *p)
								break;

							++p;
							++tIt;
						}
					}
				}
				while (it--.base() != m_sharedString->string.get());
			}
		}
		else
		{
			///Algo 2.FindLastWord#2 (Size of the pattern known)
			if (flags & CaseInsensitive)
			{
				char c = Detail::ToLower(string.m_sharedString->string[string.m_sharedString->size-1]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						char nextC = *(ptr + 1);
						if (nextC != '\0' && (Detail::IsSpace(nextC)) == 0)
							continue;

						const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
						for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
						{
							if (Detail::ToLower(*ptr) != Detail::ToLower(*p))
								break;

							if (p == &string.m_sharedString->string[0])
							{
								if (ptr == m_sharedString->string.get() || Detail::IsSpace(*(ptr-1)))
									return ptr-m_sharedString->string.get();
								else
									break;
							}

							if (ptr == m_sharedString->string.get())
								return npos;
						}
					}
				}
				while (ptr-- > limit);
			}
			else
			{
				do
				{
					if (*ptr == string.m_sharedString->string[string.m_sharedString->size-1])
					{
						char nextC = *(ptr + 1);
						if (nextC != '\0' && !Detail::IsSpace(nextC))
							continue;

						const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
						for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
						{
							if (*ptr != *p)
								break;

							if (p == &string.m_sharedString->string[0])
							{
								if (ptr == m_sharedString->string.get() || Detail::IsSpace(*(ptr - 1)))
									return ptr-m_sharedString->string.get();
								else
									break;
							}

							if (ptr == m_sharedString->string.get())
								return npos;
						}
					}
				}
				while (ptr-- > limit);
			}
		}

		return npos;
	}

	/*!
	* \brief Finds the first word in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindWord(const char* string, std::intmax_t start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		///Algo 3.FindWord#3 (Size of the pattern unknown)
		const char* ptr = &m_sharedString->string[pos];
		if (flags & HandleUtf8)
		{
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // We ensure to have one pointer pointing to the begin of the character

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string; // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*it++))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (Unicode::GetLowercase(*tIt) != Unicode::GetLowercase(*p))
								break;

							++p;
							++tIt;
						}
					}
				}
				while (*++ptr);
			}
			else
			{
				const char* t = string; // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*it++))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (*tIt != *p)
								break;

							++p;
							++tIt;
						}
					}
				}
				while (*++ptr);
			}
		}
		else
		{
			if (flags & CaseInsensitive)
			{
				char c = Detail::ToLower(string[0]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						if (ptr != m_sharedString->string.get() && !Detail::IsSpace(*(ptr - 1)))
							continue;

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						for (;;)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || Detail::IsSpace(*tPtr))
									return ptr - m_sharedString->string.get();
								else
									break;
							}

							if (Detail::ToLower(*tPtr) != Detail::ToLower(*p))
								break;

							p++;
							tPtr++;
						}
					}
				}
				while (*++ptr);
			}
			else
			{
				do
				{
					if (*ptr == string[0])
					{
						if (ptr != m_sharedString->string.get() && !Detail::IsSpace(*(ptr-1)))
							continue;

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						for (;;)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || Detail::IsSpace(*tPtr))
									return ptr - m_sharedString->string.get();
								else
									break;
							}

							if (*tPtr != *p)
								break;

							p++;
							tPtr++;
						}
					}
				}
				while (*++ptr);
			}
		}

		return npos;
	}

	/*!
	* \brief Finds the first word in the string
	* \return Index in the string
	*
	* \param string String to match
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::FindWord(const String& string, std::intmax_t start, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
			return npos;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* ptr = &m_sharedString->string[pos];
		if (flags & HandleUtf8)
		{
			///Algo 3.FindWord#3 (Iterator too slow for #2)
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // We ensure to have one pointer pointing to the begin of the character

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*it++))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (Unicode::GetLowercase(*tIt) != Unicode::GetLowercase(*p))
								break;

							++p;
							++tIt;
						}
					}
				}
				while (*++ptr);
			}
			else
			{
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affects the iterator on argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!Detail::IsSpace(*it++))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						for (;;)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Detail::IsSpace(*it++))
									return it.base() - m_sharedString->string.get();
								else
									break;
							}

							if (*tIt != *p)
								break;

							++p;
							++tIt;
						}
					}
				}
				while (*++ptr);
			}
		}
		else
		{
			///Algo 3.FindWord#2 (Size of the pattern known)
			if (flags & CaseInsensitive)
			{
				char c = Detail::ToLower(string.m_sharedString->string[0]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						if (ptr != m_sharedString->string.get() && !Detail::IsSpace(*(ptr-1)))
							continue;

						const char* p = &string.m_sharedString->string[1];
						const char* tPtr = ptr+1;
						for (;;)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || Detail::IsSpace(*tPtr))
									return ptr - m_sharedString->string.get();
								else
									break;
							}

							if (Detail::ToLower(*tPtr) != Detail::ToLower(*p))
								break;

							p++;
							tPtr++;
						}
					}
				}
				while (*++ptr);
			}
			else
			{
				while ((ptr = std::strstr(ptr, string.GetConstBuffer())) != nullptr)
				{
					// If the word is really alone
					if ((ptr == m_sharedString->string.get() || Detail::IsSpace(*(ptr-1))) && (*(ptr+m_sharedString->size) == '\0' || Detail::IsSpace(*(ptr+m_sharedString->size))))
						return ptr - m_sharedString->string.get();

					ptr++;
				}
			}
		}

		return npos;
	}

	/*!
	* \brief Gets the raw buffer
	* \return Raw buffer
	*/

	char* String::GetBuffer()
	{
		EnsureOwnership();

		return m_sharedString->string.get();
	}

	/*!
	* \brief Gets the capacity of the string
	* \return Capacity of the string
	*/

	std::size_t String::GetCapacity() const
	{
		return m_sharedString->capacity;
	}

	/*!
	* \brief Gets the index where a character begin
	*
	* Iterate through the string to find the starting position of a specific character index
	* This is useful because non-ASCII characters may be encoded using multiple bytes.
	*
	* \param characterIndex Index of the character to search for
	*
	* \return Starting index
	*/
	std::size_t String::GetCharacterPosition(std::size_t characterIndex) const
	{
		const char* ptr = m_sharedString->string.get();
		const char* end = &m_sharedString->string[m_sharedString->size];

		try
		{
			utf8::advance(ptr, characterIndex, end);

			return ptr - m_sharedString->string.get();
		}
		catch (utf8::not_enough_room& /*e*/)
		{
			// Returns npos
		}
		catch (utf8::exception& e)
		{
			NazaraError("UTF-8 error: " + String(e.what()));
		}
		catch (std::exception& e)
		{
			NazaraError(e.what());
		}

		return npos;
	}

	/*!
	* \brief Gets the raw buffer
	* \return Raw buffer
	*/

	const char* String::GetConstBuffer() const
	{
		return m_sharedString->string.get();
	}

	/*!
	* \brief Gets the length of the string
	* \return Length of the string with UTF-8 awareness
	*/

	std::size_t String::GetLength() const
	{
		return utf8::distance(m_sharedString->string.get(), &m_sharedString->string[m_sharedString->size]);
	}

	/*!
	* \brief Gets the size of the string
	* \return Size of the string without UTF-8 awareness
	*/

	std::size_t String::GetSize() const
	{
		return m_sharedString->size;
	}

	/*!
	* \brief Gets the std::string corresponding
	* \return String in UTF-8
	*/

	std::string String::GetUtf8String() const
	{
		return std::string(m_sharedString->string.get(), m_sharedString->size);
	}

	/*!
	* \brief Gets the std::string corresponding
	* \return String in UTF-16
	*/

	std::u16string String::GetUtf16String() const
	{
		if (m_sharedString->size == 0)
			return std::u16string();

		std::u16string str;
		str.reserve(m_sharedString->size);

		utf8::utf8to16(begin(), end(), std::back_inserter(str));

		return str;
	}

	/*!
	* \brief Gets the std::string corresponding
	* \return String in UTF-32
	*/

	std::u32string String::GetUtf32String() const
	{
		if (m_sharedString->size == 0)
			return std::u32string();

		std::u32string str;
		str.reserve(m_sharedString->size);

		utf8::utf8to32(begin(), end(), std::back_inserter(str));

		return str;
	}

	/*!
	* \brief Gets the std::wstring corresponding
	* \return String in Wide
	*/

	std::wstring String::GetWideString() const
	{
		static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "wchar_t size is not supported");
		if (m_sharedString->size == 0)
			return std::wstring();

		std::wstring str;
		str.reserve(m_sharedString->size);

		if (sizeof(wchar_t) == 4) // I want a static_if :(
			utf8::utf8to32(begin(), end(), std::back_inserter(str));
		else
		{
			utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
			do
			{
				char32_t cp = *it;
				if (cp <= 0xFFFF && (cp < 0xD800 || cp > 0xDFFF)) // @Laurent Gomila
					str.push_back(static_cast<wchar_t>(cp));
				else
					str.push_back(L'?');
			}
			while (*it++);
		}

		return str;
	}

	/*!
	* \brief Gets the word until next separator
	* \return Word string
	*
	* \param index Index to begin the search
	* \param flags Flag for the look up
	*/

	String String::GetWord(unsigned int index, UInt32 flags) const
	{
		std::size_t startPos = GetWordPosition(index, flags);
		if (startPos == npos)
			return String();

		std::intmax_t endPos = -1;
		const char* ptr = &m_sharedString->string[startPos];
		if (flags & HandleUtf8)
		{
			utf8::unchecked::iterator<const char*> it(ptr);
			do
			{
				if (Detail::IsSpace(*it))
				{
					endPos = static_cast<std::intmax_t>(it.base() - m_sharedString->string.get() - 1);
					break;
				}
			}
			while (*++it);
		}
		else
		{
			do
			{
				if (Detail::IsSpace(*ptr))
				{
					endPos = static_cast<std::intmax_t>(ptr - m_sharedString->string.get() - 1);
					break;
				}
			}
			while (*++ptr);
		}

		return SubString(startPos, endPos);
	}

	/*!
	* \brief Gets the word position
	* \return Position of the beginning of the word
	*
	* \param index Index to begin the search
	* \param flags Flag for the look up
	*/

	std::size_t String::GetWordPosition(unsigned int index, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return npos;

		unsigned int currentWord = 0;
		bool inWord = false;

		const char* ptr = m_sharedString->string.get();
		if (flags & HandleUtf8)
		{
			utf8::unchecked::iterator<const char*> it(ptr);
			do
			{
				if (Detail::IsSpace(*it))
					inWord = false;
				else
				{
					if (!inWord)
					{
						inWord = true;
						if (++currentWord > index)
							return it.base() - m_sharedString->string.get();
					}
				}
			}
			while (*++it);
		}
		else
		{
			do
			{
				if (Detail::IsSpace(*ptr))
					inWord = false;
				else
				{
					if (!inWord)
					{
						inWord = true;
						if (++currentWord > index)
							return ptr - m_sharedString->string.get();
					}
				}
			}
			while (*++ptr);
		}

		return npos;
	}

	/*!
	* \brief Inserts the character into the string
	* \return A reference to this
	*
	* \param pos Position in the string
	* \param character Single character
	*/

	String& String::Insert(std::intmax_t pos, char character)
	{
		return Insert(pos, &character, 1);
	}

	/*!
	* \brief Inserts the "C string" into the string
	* \return A reference to this
	*
	* \param pos Position in the string
	* \param string String to add
	*/

	String& String::Insert(std::intmax_t pos, const char* string)
	{
		return Insert(pos, string, std::strlen(string));
	}

	/*!
	* \brief Inserts the "C string" into the string
	* \return A reference to this
	*
	* \param pos Position in the string
	* \param string String to add
	* \param length Size of the string
	*/

	String& String::Insert(std::intmax_t pos, const char* string, std::size_t length)
	{
		if (length == 0)
			return *this;

		if (pos < 0)
			pos = std::max<std::size_t>(m_sharedString->size + pos, 0);

		std::size_t start = std::min<std::size_t>(pos, m_sharedString->size);

		// If buffer is already big enough
		if (m_sharedString->capacity >= m_sharedString->size + length)
		{
			EnsureOwnership();

			std::memmove(&m_sharedString->string[start+length], &m_sharedString->string[start], m_sharedString->size - start);
			std::memcpy(&m_sharedString->string[start], string, length);

			m_sharedString->size += length;
			m_sharedString->string[m_sharedString->size] = '\0';
		}
		else
		{
			auto newString = std::make_shared<SharedString>(m_sharedString->size + length);

			char* ptr = newString->string.get();

			if (start > 0)
			{
				std::memcpy(ptr, m_sharedString->string.get(), start*sizeof(char));
				ptr += start;
			}

			std::memcpy(ptr, string, length*sizeof(char));
			ptr += length;

			if (m_sharedString->size > start)
				std::memcpy(ptr, &m_sharedString->string[start], m_sharedString->size - start);

			m_sharedString = std::move(newString);
		}

		return *this;
	}

	/*!
	* \brief Inserts the string into the string
	* \return A reference to this
	*
	* \param pos Position in the string
	* \param string String to add
	*/

	String& String::Insert(std::intmax_t pos, const String& string)
	{
		return Insert(pos, string.GetConstBuffer(), string.m_sharedString->size);
	}

	/*!
	* \brief Checks whether the string is empty
	* \return true if string is empty
	*/

	bool String::IsEmpty() const
	{
		return m_sharedString->size == 0;
	}

	/*!
	* \brief Checks whether the string is null
	* \return true if string is null
	*/

	bool String::IsNull() const
	{
		return m_sharedString.get() == GetEmptyString().get();
	}

	/*!
	* \brief Checks whether the string is a number
	* \return true if string is a number
	*
	* \param base Base of the number
	* \param flags Flag for the look up
	*
	* \remark Produces a NazaraError if base is not in [2, 36( with NAZARA_CORE_SAFE defined
	*/

	bool String::IsNumber(UInt8 base, UInt32 flags) const
	{
		#if NAZARA_CORE_SAFE
		if (base < 2 || base > 36)
		{
			NazaraError("Base must be between 2 and 36");
			return false;
		}
		#endif

		if (m_sharedString->size == 0)
			return false;

		String check = Simplified();
		if (check.m_sharedString->size == 0)
			return false;

		char* ptr = (check.m_sharedString->string[0] == '-') ? &check.m_sharedString->string[1] : check.m_sharedString->string.get();

		if (base > 10)
		{
			if (flags & CaseInsensitive)
			{
				char limitLower = 'a' + base-10 - 1;
				char limitUpper = 'A' + base-10 - 1;

				do
				{
					char c = *ptr;
					if (c != ' ' && (c < '0' || (c > '9' && c < 'A') || (c > limitUpper && c < 'a') || c > limitLower))
						return false;
				}
				while (*++ptr);
			}
			else
			{
				char limit = 'a' + base-10 - 1;
				do
				{
					char c = *ptr;
					if (c != ' ' && (c < '0' || (c > '9' && c < 'a') || c > limit))
						return false;
				}
				while (*++ptr);
			}
		}
		else
		{
			char limit = '0' + base - 1;

			do
			{
				char c = *ptr;
				if (c != ' ' && (c < '0' || c > limit))
					return false;
			}
			while (*++ptr);
		}

		return true;
	}

	/*!
	* \brief Checks whether the string matches the pattern
	* \return true if string matches
	*
	* \param pattern Pattern to search
	*/

	bool String::Match(const char* pattern) const
	{
		if (m_sharedString->size == 0 || !pattern)
			return false;

		// Par Jack Handy - akkhandy@hotmail.com
		// From : http://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
		const char* str = m_sharedString->string.get();
		while (*str && *pattern != '*')
		{
			if (*pattern != *str && *pattern != '?')
				return false;

			pattern++;
			str++;
		}

		const char* cp = nullptr;
		const char* mp = nullptr;
		while (*str)
		{
			if (*pattern == '*')
			{
				if (!*++pattern)
					return true;

				mp = pattern;
				cp = str+1;
			}
			else if (*pattern == *str || *pattern == '?')
			{
				pattern++;
				str++;
			}
			else
			{
				pattern = mp;
				str = cp++;
			}
		}

		while (*pattern == '*')
			pattern++;

		return !*pattern;
	}

	/*!
	* \brief Checks whether the string matches the pattern
	* \return true if string matches
	*
	* \param pattern Pattern to search
	*/

	bool String::Match(const String& pattern) const
	{
		return Match(pattern.m_sharedString->string.get());
	}

	/*!
	* \brief Prepends the character to the string
	* \return A reference to this
	*
	* \param character Single character
	*
	* \see Insert
	*/

	String& String::Prepend(char character)
	{
		return Insert(0, character);
	}

	/*!
	* \brief Prepends the "C string" to the string
	* \return A reference to this
	*
	* \param string String to add
	*
	* \see Insert
	*/

	String& String::Prepend(const char* string)
	{
		return Insert(0, string);
	}

	/*!
	* \brief Prepends the "C string" to the string
	* \return A reference to this
	*
	* \param string String to add
	* \param length Size of the string
	*
	* \see Insert
	*/

	String& String::Prepend(const char* string, std::size_t length)
	{
		return Insert(0, string, length);
	}

	/*!
	* \brief Prepends the string to the string
	* \return A reference to this
	*
	* \param string String to add
	*
	* \see Insert
	*/

	String& String::Prepend(const String& string)
	{
		return Insert(0, string);
	}

	/*!
	* \brief Replaces the old character by the new one
	* \return Number of changes
	*
	* \param oldCharacter Pattern to find
	* \param newCharacter Pattern to change for
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Replace(char oldCharacter, char newCharacter, std::intmax_t start, UInt32 flags)
	{
		if (oldCharacter == '\0' || oldCharacter == newCharacter)
			return 0;

		if (newCharacter == '\0') // In this case, we must use a more advanced algorithm
			return Replace(String(oldCharacter), String(), start);

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		unsigned int count = 0;
		char* ptr = &m_sharedString->string[pos];
		bool found = false;
		if (flags & CaseInsensitive)
		{
			char character_lower = Detail::ToLower(oldCharacter);
			char character_upper = Detail::ToUpper(oldCharacter);
			do
			{
				if (*ptr == character_lower || *ptr == character_upper)
				{
					if (!found)
					{
						std::ptrdiff_t offset = ptr - m_sharedString->string.get();

						EnsureOwnership();

						ptr = &m_sharedString->string[offset];
						found = true;
					}

					*ptr = newCharacter;
					++count;
				}
			}
			while (*++ptr);
		}
		else
		{
			while ((ptr = std::strchr(ptr, oldCharacter)) != nullptr)
			{
				if (!found)
				{
					std::ptrdiff_t offset = ptr-m_sharedString->string.get();

					EnsureOwnership();

					ptr = &m_sharedString->string[offset];
					found = true;
				}

				*ptr = newCharacter;
				++count;
			}
		}

		return count;
	}

	/*!
	* \brief Replaces the old "C string" by the new one
	* \return Number of changes
	*
	* \param oldString Pattern to find
	* \param replaceString Pattern to change for
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Replace(const char* oldString, const char* replaceString, std::intmax_t start, UInt32 flags)
	{
		return Replace(oldString, std::strlen(oldString), replaceString, std::strlen(replaceString), start, flags);
	}

	/*!
	* \brief Replaces the old "C string" by the new one
	* \return Number of changes
	*
	* \param oldString Pattern to find
	* \param oldLength Length of the old string
	* \param replaceString Pattern to change for
	* \param replaceLength of the new string
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Replace(const char* oldString, std::size_t oldLength, const char* replaceString, std::size_t replaceLength, std::intmax_t start, UInt32 flags)
	{
		if (oldLength == 0)
			return 0;

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return 0;

		unsigned int count = 0;
		if (oldLength == replaceLength)
		{
			bool found = false;

			// If no size change is necessary, we can thus use a quicker algorithm
			while ((pos = Find(oldString, pos, flags)) != npos)
			{
				if (!found)
				{
					EnsureOwnership();
					found = true;
				}

				std::memcpy(&m_sharedString->string[pos], replaceString, oldLength);
				pos += oldLength;

				++count;
			}
		}
		else ///TODO: Replacement algorithm without changing the buffer (if replaceLength < oldLength)
		{
			std::size_t newSize = m_sharedString->size + Count(oldString)*(replaceLength - oldLength);
			if (newSize == m_sharedString->size) // Then it's the fact that Count(oldString) == 0
				return 0;

			auto newString = std::make_shared<SharedString>(newSize);

			///Algo 4.Replace#2
			char* ptr = newString->string.get();
			const char* p = m_sharedString->string.get();

			while ((pos = Find(oldString, pos, flags)) != npos)
			{
				const char* r = &m_sharedString->string[pos];

				std::memcpy(ptr, p, r-p);
				ptr += r-p;
				std::memcpy(ptr, replaceString, replaceLength);
				ptr += replaceLength;
				p = r+oldLength;
				pos += oldLength;

				count++;
			}

			std::strcpy(ptr, p);

			m_sharedString = std::move(newString);
		}

		return count;
	}

	/*!
	* \brief Replaces the old string by the new one
	* \return Number of changes
	*
	* \param oldString Pattern to find
	* \param replaceString Pattern to change for
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Replace(const String& oldString, const String& replaceString, std::intmax_t start, UInt32 flags)
	{
		return Replace(oldString.GetConstBuffer(), oldString.m_sharedString->size, replaceString.GetConstBuffer(), replaceString.m_sharedString->size, start, flags);
	}

	/*!
	* \brief Replaces the old characters in the list by the new one
	* \return Number of changes
	*
	* \param oldCharacters Pattern to find
	* \param replaceCharacter Pattern to change for
	* \param start Index to begin the search
	* \param flags Flag for the look up
	*
	* \remark Does not handle UTF-8 currently
	*/

	unsigned int String::ReplaceAny(const char* oldCharacters, char replaceCharacter, std::intmax_t start, UInt32 flags)
	{
		///FIXME: Does not handle UTF-8
		if (!oldCharacters || !oldCharacters[0])
			return 0;

		/*if (replaceCharacter == '\0') // In this case, we must use a more advance algorithm
			return ReplaceAny(String(oldCharacters), String(), start);*/

		if (start < 0)
			start = std::max<std::size_t>(m_sharedString->size + start, 0);

		std::size_t pos = static_cast<std::size_t>(start);
		if (pos >= m_sharedString->size)
			return npos;

		unsigned int count = 0;
		char* ptr = &m_sharedString->string[pos];
		if (flags & CaseInsensitive)
		{
			do
			{
				const char* c = oldCharacters;
				char character = Detail::ToLower(*ptr);
				bool found = false;
				do
				{
					if (character == Detail::ToLower(*c))
					{
						if (!found)
						{
							std::ptrdiff_t offset = ptr - m_sharedString->string.get();

							EnsureOwnership();

							ptr = &m_sharedString->string[offset];
							found = true;
						}

						*ptr = replaceCharacter;
						++count;
						break;
					}
				}
				while (*++c);
			}
			while (*++ptr);
		}
		else
		{
			bool found = false;
			while ((ptr = std::strpbrk(ptr, oldCharacters)) != nullptr)
			{
				if (!found)
				{
					std::ptrdiff_t offset = ptr - m_sharedString->string.get();

					EnsureOwnership();

					ptr = &m_sharedString->string[offset];
					found = true;
				}

				*ptr++ = replaceCharacter;
				++count;
			}
		}

		return count;
	}
	/*
		unsigned int String::ReplaceAny(const char* oldCharacters, const char* replaceString, std::intmax_t start, UInt32 flags)
		{
			if (start < 0)
			{
				start = m_sharedString->size+start;
				if (start < 0)
					start = 0;
			}

			unsigned int pos = static_cast<unsigned int>(start);
			unsigned int oSize = (oldCharacters) ? std::strlen(oldCharacters) : 0;
			unsigned int rSize = (replaceString) ? std::strlen(replaceString) : 0;

			if (pos >= m_sharedString->size || m_sharedString->size == 0 || oSize == 0)
				return 0;

			unsigned int count = 0;

			if (rSize == 1) // On utilise un algorithme optimisé
			{
				EnsureOwnership();

				f or (; pos < m_sharedString->size; ++pos)
				{
					for (unsigned int i = 0; i < oSize; ++i)
					{
						if (m_sharedString->string[pos] == oldCharacters[i])
						{
							m_sharedString->string[pos] = replaceString[0];
							++count;

							break;
						}
					}
				}
			}
			else
			{
				unsigned int newSize;
				{
					unsigned int count = CountAny(oldCharacters);
					newSize = m_sharedString->size - count + count*rSize;
				}
				char* newString = new char[newSize+1];

				unsigned int j = 0;
				for (unsigned int i = 0; i < m_sharedString->size; ++i)
				{
					if (i < pos) // Avant la position où on est censé commencer à remplacer, on ne fait que recopier
						newString[j++] = m_sharedString->string[i];
					else
					{
						bool found = false;
						for (unsigned int l = 0; l < oSize; ++l)
						{
							if (m_sharedString->string[i] == oldCharacters[l])
							{
								for (unsigned int k = 0; k < rSize; ++k)
									newString[j++] = replaceString[k];

								++count;
								found = true;
								break; // Simple façon d'éviter la ligne après la boucle
							}
						}

						if (!found)
							newString[j++] = m_sharedString->string[i];
					}
				}
				newString[newSize] = '\0';

				ReleaseString();

				m_sharedString->size = newSize;
				m_sharedString->string = newString;
			}

			return count;
		}

		unsigned int String::ReplaceAny(const String& oldCharacters, const String& replaceString, std::intmax_t start, UInt32 flags)
		{
			if (start < 0)
			{
				start = m_sharedString->size+start;
				if (start < 0)
					start = 0;
			}

			unsigned int pos = static_cast<unsigned int>(start);

			if (pos >= m_sharedString->size || m_sharedString->size == 0 || oldCharacters.m_sharedString->size == 0)
				return 0;

			unsigned int count = 0;

			if (replaceString.m_sharedString->size == 1) // On utilise un algorithme optimisé
			{
				EnsureOwnership();

				char character = replaceString[0];
				for (; pos < m_sharedString->size; ++pos)
				{
					for (unsigned int i = 0; i < oldCharacters.m_sharedString->size; ++i)
					{
						if (m_sharedString->string[pos] == oldCharacters[i])
						{
							m_sharedString->string[pos] = character;
							++count;
							break;
						}
					}
				}
			}
			else
			{
				unsigned int newSize;
				{
					unsigned int count = CountAny(oldCharacters);
					newSize = m_sharedString->size - count + count*replaceString.m_sharedString->size;
				}
				char* newString = new char[newSize+1];

				unsigned int j = 0;
				for (unsigned int i = 0; i < m_sharedString->size; ++i)
				{
					if (i < pos) // Avant la position où on est censé commencer à remplacer, on ne fait que recopier
						newString[j++] = m_sharedString->string[i];
					else
					{
						bool found = false;
						for (unsigned int l = 0; l < oldCharacters.m_sharedString->size; ++l)
						{
							if (m_sharedString->string[i] == oldCharacters[l])
							{
								for (unsigned int k = 0; k < replaceString.m_sharedString->size; ++k)
									newString[j++] = replaceString[k];

								++count;
								found = true;
								break; // Simple façon d'éviter la ligne après la boucle
							}
						}

						if (!found)
							newString[j++] = m_sharedString->string[i];
					}
				}
				newString[newSize] = '\0';

				ReleaseString();

				m_sharedString->size = newSize;
				m_sharedString->string = newString;
			}

			return count;
		}
	*/

	/*!
	* \brief Reserves enough memory for the buffer size
	*
	* \param bufferSize Size of the buffer to allocate
	*
	* \remark If bufferSize is smaller than the old one, nothing is done
	*/

	void String::Reserve(std::size_t bufferSize)
	{
		if (m_sharedString->capacity > bufferSize)
			return;

		auto newString = std::make_shared<SharedString>(bufferSize);
		newString->size = m_sharedString->size;

		if (m_sharedString->size > 0)
			std::memcpy(newString->string.get(), m_sharedString->string.get(), m_sharedString->size);

		m_sharedString = std::move(newString);
	}

	/*!
	* \brief Resizes the string
	* \return A reference to this
	*
	* \param size Target size
	* \param flags Flag for the look up
	*/

	String& String::Resize(std::intmax_t size, UInt32 flags)
	{
		if (size == 0)
		{
			Clear(true);
			return *this;
		}

		if (size < 0)
			size = std::max<std::intmax_t>(m_sharedString->size + size, 0);

		std::size_t newSize = static_cast<std::size_t>(size);

		if (flags & HandleUtf8 && newSize < m_sharedString->size)
		{
			std::size_t characterToRemove = m_sharedString->size - newSize;

			char* ptr = &m_sharedString->string[m_sharedString->size];
			for (std::size_t i = 0; i < characterToRemove; ++i)
				utf8::prior(ptr, m_sharedString->string.get());

			newSize = ptr - m_sharedString->string.get();
		}

		if (m_sharedString->capacity >= newSize)
		{
			EnsureOwnership();

			m_sharedString->size = newSize;
			m_sharedString->string[newSize] = '\0'; // Adds the EoS character
		}
		else // Then we want to make the string bigger
		{
			auto newString = std::make_shared<SharedString>(newSize);
			std::memcpy(newString->string.get(), m_sharedString->string.get(), m_sharedString->size);

			m_sharedString = std::move(newString);
		}

		return *this;
	}

	/*!
	* \brief Resize a copy of the string
	* \return A copy of what would be the string resized
	*
	* \param size Target size
	* \param flags Flag for the look up
	*/

	String String::Resized(std::intmax_t size, UInt32 flags) const
	{
		if (size < 0)
			size = m_sharedString->size + size;

		if (size <= 0)
			return String();

		std::size_t newSize = static_cast<std::size_t>(size);
		if (newSize == m_sharedString->size)
			return *this;

		if (flags & HandleUtf8 && newSize < m_sharedString->size)
		{
			std::size_t characterToRemove = m_sharedString->size - newSize;

			char* ptr = &m_sharedString->string[m_sharedString->size - 1];
			for (std::size_t i = 0; i < characterToRemove; ++i)
				utf8::prior(ptr, m_sharedString->string.get());

			newSize = ptr - m_sharedString->string.get();
		}

		auto sharedStr = std::make_shared<SharedString>(newSize);
		if (newSize > m_sharedString->size)
			std::memcpy(sharedStr->string.get(), m_sharedString->string.get(), m_sharedString->size);
		else
			std::memcpy(sharedStr->string.get(), m_sharedString->string.get(), newSize);

		return String(std::move(sharedStr));
	}

	/*!
	* \brief Reverses the string
	* \return A reference to this
	*/

	String& String::Reverse()
	{
		if (m_sharedString->size != 0)
		{
			std::size_t i = 0;
			std::size_t j = m_sharedString->size-1;

			while (i < j)
				std::swap(m_sharedString->string[i++], m_sharedString->string[j--]);
		}

		return *this;
	}

	/*!
	* \brief Reverses a copy of the string
	* \return A copy of what would be the string reversed
	*/

	String String::Reversed() const
	{
		if (m_sharedString->size == 0)
			return String();

		auto sharedStr = std::make_shared<SharedString>(m_sharedString->size);

		char* ptr = &sharedStr->string[m_sharedString->size - 1];
		char* p = m_sharedString->string.get();

		do
			*ptr-- = *p;
		while (*(++p));

		return String(std::move(sharedStr));
	}

	/*!
	* \brief Sets the string to the character
	* \return A reference to this
	*
	* \param character Single character
	*/

	String& String::Set(char character)
	{
		if (character != '\0')
		{
			if (m_sharedString->capacity >= 1)
			{
				EnsureOwnership(true);

				m_sharedString->size = 1;
				m_sharedString->string[1] = '\0';
			}
			else
				m_sharedString = std::make_shared<SharedString>(1);

			m_sharedString->string[0] = character;
		}
		else
			ReleaseString();

		return *this;
	}


	/*!
	* \brief Sets the string with multiple times the same character
	* \return A reference to this
	*
	* \param rep Number of repetitions of the character
	* \param character Single character
	*/

	String& String::Set(std::size_t rep, char character)
	{
		if (rep > 0)
		{
			if (m_sharedString->capacity >= rep)
			{
				EnsureOwnership(true);

				m_sharedString->size = rep;
				m_sharedString->string[rep] = '\0';
			}
			else
				m_sharedString = std::make_shared<SharedString>(rep);

			if (character != '\0')
				std::memset(m_sharedString->string.get(), character, rep);
		}
		else
			ReleaseString();

		return *this;
	}

	/*!
	* \brief Sets the string with multiple times the same string
	* \return A reference to this
	*
	* \param rep Number of repetitions of the string
	* \param string String to multiply
	*/

	String& String::Set(std::size_t rep, const char* string)
	{
		return Set(rep, string, (string) ? std::strlen(string) : 0);
	}

	/*!
	* \brief Sets the string with multiple times the same string
	* \return A reference to this
	*
	* \param rep Number of repetitions of the string
	* \param string String to multiply
	* \param length Length of the string
	*/

	String& String::Set(std::size_t rep, const char* string, std::size_t length)
	{
		std::size_t totalSize = rep*length;

		if (totalSize > 0)
		{
			if (m_sharedString->capacity >= totalSize)
			{
				EnsureOwnership(true);

				m_sharedString->size = totalSize;
				m_sharedString->string[totalSize] = '\0';
			}
			else
				m_sharedString = std::make_shared<SharedString>(totalSize);

			for (std::size_t i = 0; i < rep; ++i)
				std::memcpy(&m_sharedString->string[i*length], string, length);
		}
		else
			ReleaseString();

		return *this;
	}

	/*!
	* \brief Sets the string with multiple times the same string
	* \return A reference to this
	*
	* \param rep Number of repetitions of the string
	* \param string String to multiply
	*/

	String& String::Set(std::size_t rep, const String& string)
	{
		return Set(rep, string.GetConstBuffer(), string.m_sharedString->size);
	}

	/*!
	* \brief Sets the string with other "C string"
	* \return A reference to this
	*
	* \param string String to copy
	*/

	String& String::Set(const char* string)
	{
		return Set(string, (string) ? std::strlen(string) : 0);
	}

	/*!
	* \brief Sets the string with other "C string"
	* \return A reference to this
	*
	* \param string String to represent
	* \param length Length of the string
	*/

	String& String::Set(const char* string, std::size_t length)
	{
		if (length > 0)
		{
			if (m_sharedString->capacity >= length)
			{
				EnsureOwnership(true);

				m_sharedString->size = length;
				m_sharedString->string[length] = '\0';
			}
			else
				m_sharedString = std::make_shared<SharedString>(length);

			std::memcpy(m_sharedString->string.get(), string, length);
		}
		else
			ReleaseString();

		return *this;
	}

	/*!
	* \brief Sets the string with a std::string
	* \return A reference to this
	*
	* \param string String to copy
	*/

	String& String::Set(const std::string& string)
	{
		return Set(string.data(), string.size());
	}

	/*!
	* \brief Sets the string with other string
	* \return A reference to this
	*
	* \param string String to copy
	*/

	String& String::Set(const String& string)
	{
		m_sharedString = string.m_sharedString;

		return *this;
	}

	/*!
	* \brief Sets the string by move semantic
	* \return A reference to this
	*
	* \param string String to move
	*/

	String& String::Set(String&& string) noexcept
	{
		std::swap(m_sharedString, string.m_sharedString);

		return *this;
	}

	/*!
	* \brief Simplifies a copy of the string
	* \return A copy of what would be the string simplified
	*
	* \param flags Flag for the look up
	*/

	String String::Simplified(UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return String();

		auto newString = std::make_shared<SharedString>(m_sharedString->size);
		char* str = newString->string.get();
		char* p = str;

		const char* ptr = m_sharedString->string.get();
		bool inword = false;
		if (flags & HandleUtf8)
		{
			utf8::unchecked::iterator<const char*> it(ptr);
			do
			{
				if (Detail::IsSpace(*it))
				{
					if (inword)
					{
						*p++ = ' ';
						inword = false;
					}
				}
				else
				{
					p = utf8::append(*it, p);
					inword = true;
				}
			}
			while (*++it);
		}
		else
		{
			const char* limit = &m_sharedString->string[m_sharedString->size];
			do
			{
				if (Detail::IsSpace(*ptr))
				{
					if (inword)
					{
						*p++ = ' ';
						inword = false;
					}
				}
				else
				{
					*p++ = *ptr;
					inword = true;
				}
			}
			while (++ptr != limit);
		}

		if (!inword && p != str)
			p--;

		*p = '\0';
		newString->size = p - str;

		return String(std::move(newString));
	}

	/*!
	* \brief Simplifies the string
	* \return A reference to this
	*
	* \param flags Flag for the look up
	*/

	String& String::Simplify(UInt32 flags)
	{
		return Set(Simplified(flags));
	}

	/*!
	* \brief Splits the string into others
	* \return The number of splits
	*
	* \param result Resulting tokens
	* \param separation Separation character
	* \param start Index for the beginning of the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Split(std::vector<String>& result, char separation, std::intmax_t start, UInt32 flags) const
	{
		if (separation == '\0' || m_sharedString->size == 0)
			return 0;

		std::size_t lastSep = Find(separation, start, flags);
		if (lastSep == npos)
		{
			result.push_back(*this);
			return 1;
		}
		else if (lastSep != 0)
			result.push_back(SubString(0, lastSep-1));

		for (;;)
		{
			std::size_t sep = Find(separation, lastSep+1, flags);
			if (sep == npos)
				break;

			if (sep-lastSep > 1)
				result.push_back(SubString(lastSep+1, sep-1));

			lastSep = sep;
		}

		if (lastSep != m_sharedString->size-1)
			result.push_back(SubString(lastSep+1));

		return result.size();
	}

	/*!
	* \brief Splits the string into others
	* \return The number of splits
	*
	* \param result Resulting tokens
	* \param separation Separation string
	* \param start Index for the beginning of the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Split(std::vector<String>& result, const char* separation, std::intmax_t start, UInt32 flags) const
	{
		return Split(result, separation, std::strlen(separation), start, flags);
	}

	/*!
	* \brief Splits the string into others
	* \return The number of splits
	*
	* \param result Resulting tokens
	* \param separation Separation String
	* \param length Length of the string
	* \param start Index for the beginning of the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Split(std::vector<String>& result, const char* separation, std::size_t length, std::intmax_t start, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return 0;
		else if (length == 0)
		{
			result.reserve(m_sharedString->size);
			for (std::size_t i = 0; i < m_sharedString->size; ++i)
				result.push_back(String(m_sharedString->string[i]));

			return m_sharedString->size;
		}
		else if (length > m_sharedString->size)
		{
			result.push_back(*this);
			return 1;
		}

		std::size_t lastSep = Find(separation, start, flags);
		std::size_t oldSize = result.size();
		if (lastSep == npos)
		{
			result.push_back(*this);
			return 1;
		}
		else if (lastSep != 0)
			result.push_back(SubString(0, lastSep-1));

		std::size_t sep;
		while ((sep = Find(separation, lastSep + length, flags)) != npos)
		{
			if (sep-lastSep > length)
				result.push_back(SubString(lastSep + length, sep-1));

			lastSep = sep;
		}

		if (lastSep != m_sharedString->size - length)
			result.push_back(SubString(lastSep + length));

		return result.size()-oldSize;
	}

	/*!
	* \brief Splits the string into others
	* \return The number of splits
	*
	* \param result Resulting tokens
	* \param separation Separation string
	* \param start Index for the beginning of the search
	* \param flags Flag for the look up
	*/

	unsigned int String::Split(std::vector<String>& result, const String& separation, std::intmax_t start, UInt32 flags) const
	{
		return Split(result, separation.m_sharedString->string.get(), separation.m_sharedString->size, start, flags);
	}

	/*!
	* \brief Splits the string into others
	* \return The number of splits
	*
	* \param result Resulting tokens
	* \param separations List of characters for separation
	* \param start Index for the beginning of the search
	* \param flags Flag for the look up
	*/

	unsigned int String::SplitAny(std::vector<String>& result, const char* separations, std::intmax_t start, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return 0;

		std::size_t oldSize = result.size();

		std::size_t lastSep = FindAny(separations, start, flags);
		if (lastSep == npos)
		{
			result.push_back(*this);
			return 1;
		}
		else if (lastSep != 0)
			result.push_back(SubString(0, lastSep-1));

		std::size_t sep;
		while ((sep = FindAny(separations, lastSep+1, flags)) != npos)
		{
			if (sep-lastSep > 1)
				result.push_back(SubString(lastSep+1, sep-1));

			lastSep = sep;
		}

		if (lastSep != m_sharedString->size-1)
			result.push_back(SubString(lastSep+1));

		return result.size()-oldSize;
	}

	/*!
	* \brief Splits the string into others
	* \return The number of splits
	*
	* \param result Resulting tokens
	* \param separations List of characters for separation
	* \param start Index for the beginning of the search
	* \param flags Flag for the look up
	*/

	unsigned int String::SplitAny(std::vector<String>& result, const String& separations, std::intmax_t start, UInt32 flags) const
	{
		return SplitAny(result, separations.m_sharedString->string.get(), start, flags);
	}

	/*!
	* \brief Checks whether the string begins with the character
	* \return true if it the case
	*
	* \param character Single character
	* \param flags Flag for the look up
	*
	* \see EndsWith
	*/

	bool String::StartsWith(char character, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return false;

		if (flags & CaseInsensitive)
			return Detail::ToLower(m_sharedString->string[0]) == Detail::ToLower(character);
		else
			return m_sharedString->string[0] == character;
	}

	/*!
	* \brief Checks whether the string begins with the "C string"
	* \return true if it the case
	*
	* \param string String to match
	* \param flags Flag for the look up
	*
	* \see EndsWith
	*/

	bool String::StartsWith(const char* string, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return false;

		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
				utf8::unchecked::iterator<const char*> it2(string);
				do
				{
					if (*it2 == '\0')
						return true;

					if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
						return false;

					++it2;
				}
				while (*it++);
			}
			else
			{
				char* ptr = m_sharedString->string.get();
				const char* s = string;
				do
				{
					if (*s == '\0')
						return true;

					if (Detail::ToLower(*ptr) != Detail::ToLower(*s))
						return false;

					s++;
				}
				while (*ptr++);
			}
		}
		else
		{
			char* ptr = m_sharedString->string.get();
			const char* s = string;
			do
			{
				if (*s == '\0')
					return true;

				if (*ptr != *s)
					return false;

				s++;
			}
			while (*ptr++);
		}

		return false;
	}

	/*!
	* \brief Checks whether the string begins with the string
	* \return true if it the case
	*
	* \param string String to match
	* \param flags Flag for the look up
	*
	* \see EndsWith
	*/

	bool String::StartsWith(const String& string, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0)
			return false;

		if (m_sharedString->size < string.m_sharedString->size)
			return false;

		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
				utf8::unchecked::iterator<const char*> it2(string.GetConstBuffer());
				do
				{
					if (*it2 == '\0')
						return true;

					if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
						return false;

					++it2;
				}
				while (*it++);
			}
			else
			{
				char* ptr = m_sharedString->string.get();
				const char* s = string.GetConstBuffer();
				do
				{
					if (*s == '\0')
						return true;

					if (Detail::ToLower(*ptr) != Detail::ToLower(*s))
						return false;

					s++;
				}
				while (*ptr++);
			}
		}
		else
			return std::memcmp(m_sharedString->string.get(), string.GetConstBuffer(), string.m_sharedString->size) == 0;

		return false;
	}

	/*!
	* \brief Returns a sub string of the string
	* \return SubString
	*
	* \param startPos Index for the beginning of the search
	* \param endPos Index for the end of the search
	*/

	String String::SubString(std::intmax_t startPos, std::intmax_t endPos) const
	{
		if (startPos < 0)
			startPos = std::max<std::size_t>(m_sharedString->size + startPos, 0);

		std::size_t start = static_cast<std::size_t>(startPos);

		if (endPos < 0)
		{
			endPos = m_sharedString->size+endPos;
			if (endPos < 0)
				return String();
		}

		std::size_t minEnd = std::min(static_cast<std::size_t>(endPos), m_sharedString->size - 1);
		if (start > minEnd || start >= m_sharedString->size)
			return String();

		std::size_t size = minEnd - start + 1;

		auto str = std::make_shared<SharedString>(size);
		std::memcpy(str->string.get(), &m_sharedString->string[start], size);

		return String(std::move(str));
	}

	/*!
	* \brief Returns a sub string of the string from a character
	* \return SubString
	*
	* \param character Pattern to find
	* \param startPos Index for the beginning of the search
	* \param fromLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringFrom(char character, std::intmax_t startPos, bool fromLast, bool include, UInt32 flags) const
	{
		if (character == '\0')
			return *this;

		std::size_t pos;
		if (fromLast)
			pos = FindLast(character, startPos, flags);
		else
			pos = Find(character, startPos, flags);

		if (pos == 0 && include)
			return *this;
		else if (pos == npos)
			return String();

		return SubString(pos + ((include) ? 0 : 1));
	}

	/*!
	* \brief Returns a sub string of the string from a string
	* \return SubString
	*
	* \param string Pattern to find
	* \param startPos Index for the beginning of the search
	* \param fromLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringFrom(const char* string, std::intmax_t startPos, bool fromLast, bool include, UInt32 flags) const
	{
		return SubStringFrom(string, std::strlen(string), startPos, fromLast, include, flags);
	}

	/*!
	* \brief Returns a sub string of the string from a string
	* \return SubString
	*
	* \param string Pattern to find
	* \param length Size of the string
	* \param startPos Index for the beginning of the search
	* \param fromLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringFrom(const char* string, std::size_t length, std::intmax_t startPos, bool fromLast, bool include, UInt32 flags) const
	{
		std::size_t pos;
		if (fromLast)
			pos = FindLast(string, startPos, flags);
		else
			pos = Find(string, startPos, flags);

		if (pos == 0 && include)
			return *this;
		else if (pos == npos)
			return String();

		return SubString(pos + ((include) ? 0 : length));
	}

	/*!
	* \brief Returns a sub string of the string from a string
	* \return SubString
	*
	* \param string Pattern to find
	* \param startPos Index for the beginning of the search
	* \param fromLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringFrom(const String& string, std::intmax_t startPos, bool fromLast, bool include, UInt32 flags) const
	{
		return SubStringFrom(string.GetConstBuffer(), string.m_sharedString->size, startPos, fromLast, include, flags);
	}

	/*!
	* \brief Returns a sub string of the string up to a character
	* \return SubString
	*
	* \param character Pattern to find
	* \param startPos Index for the beginning of the search
	* \param toLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringTo(char character, std::intmax_t startPos, bool toLast, bool include, UInt32 flags) const
	{
		if (character == '\0')
			return *this;

		std::size_t pos;
		if (toLast)
			pos = FindLast(character, startPos);
		else
			pos = Find(character, startPos, flags);

		if (pos == 0)
			return (include) ? String(character) : String();
		else if (pos == npos)
			return *this;

		return SubString(0, pos+((include) ? 1 : 0)-1);
	}

	/*!
	* \brief Returns a sub string of the string up to a string
	* \return SubString
	*
	* \param string Pattern to find
	* \param startPos Index for the beginning of the search
	* \param toLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringTo(const char* string, std::intmax_t startPos, bool toLast, bool include, UInt32 flags) const
	{
		return SubStringTo(string, std::strlen(string), startPos, toLast, include, flags);
	}

	/*!
	* \brief Returns a sub string of the string up to a string
	* \return SubString
	*
	* \param string Pattern to find
	* \param length Size of the string
	* \param startPos Index for the beginning of the search
	* \param toLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringTo(const char* string, std::size_t length, std::intmax_t startPos, bool toLast, bool include, UInt32 flags) const
	{
		std::size_t pos;
		if (toLast)
			pos = FindLast(string, startPos, flags);
		else
			pos = Find(string, startPos, flags);

		if (pos == 0)
			return (include) ? string : String();
		else if (pos == npos)
			return *this;

		return SubString(0, pos+((include) ? length : 0)-1);
	}

	/*!
	* \brief Returns a sub string of the string up to a string
	* \return SubString
	*
	* \param string Pattern to find
	* \param startPos Index for the beginning of the search
	* \param toLast beginning by the end
	* \param include Include the character
	* \param flags Flag for the look up
	*/

	String String::SubStringTo(const String& string, std::intmax_t startPos, bool toLast, bool include, UInt32 flags) const
	{
		return SubStringTo(string.GetConstBuffer(), string.m_sharedString->size, startPos, toLast, include, flags);
	}

	/*!
	* \brief Swaps the content with the other string
	*
	* \param str Other string to swap with
	*/

	void String::Swap(String& str)
	{
		std::swap(m_sharedString, str.m_sharedString);
	}

	/*!
	* \brief Converts the string to boolean
	* \return true if successful
	*
	* \param value Boolean to convert to
	* \param flags Flag for the look up
	*/

	bool String::ToBool(bool* value, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return false;

		String word = GetWord(0);

		if (word[0] == '1')
		{
			if (value)
				*value = true;
		}
		else if (word[0] == '0')
		{
			if (value)
				*value = false;
		}
		else
		{
			if (flags & CaseInsensitive)
				word = word.ToLower(); // The identified words are in ASCII, no use of Unicode flag

			if (word == "true")
			{
				if (value)
					*value = true;
			}
			else if (word == "false")
			{
				if (value)
					*value = false;
			}
			else
				return false;
		}

		return true;
	}

	/*!
	* \brief Converts the string to double
	* \return true if successful
	*
	* \param value Double to convert to
	*/

	bool String::ToDouble(double* value) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (value)
			*value = std::atof(m_sharedString->string.get());

		return true;
	}

	/*!
	* \brief Converts the string to integer
	* \return true if successful
	*
	* \param value Integer to convert to
	* \param base Base to convert the integer to
	*/

	bool String::ToInteger(long long* value, UInt8 base) const
	{
		if (value)
		{
			bool ok;
			*value = StringToNumber(*this, base, &ok);

			return ok;
		}
		else
			return IsNumber(base);
	}

	/*!
	* \brief Converts the string to lower
	* \return Lower string
	*
	* \param flags Flag for the look up
	*/

	String String::ToLower(UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return *this;

		if (flags & HandleUtf8)
		{
			String lower;
			lower.Reserve(m_sharedString->size);
			utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
			do
				utf8::append(Unicode::GetLowercase(*it), std::back_inserter(lower));
			while (*++it);

			return lower;
		}
		else
		{
			auto str = std::make_shared<SharedString>(m_sharedString->size);

			char* ptr = m_sharedString->string.get();
			char* s = str->string.get();
			do
				*s++ = Detail::ToLower(*ptr);
			while (*++ptr);

			*s = '\0';

			return String(std::move(str));
		}
	}

	/*!
	* \brief Converts the string to std::string
	* \return std::string representation
	*/
	std::string String::ToStdString() const
	{
		return std::string(m_sharedString->string.get(), m_sharedString->size);
	}

	/*!
	* \brief Converts the string to upper
	* \return Upper string
	*
	* \param flags Flag for the look up
	*/
	String String::ToUpper(UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return *this;

		if (flags & HandleUtf8)
		{
			String upper;
			upper.Reserve(m_sharedString->size);
			utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
			do
				utf8::append(Unicode::GetUppercase(*it), std::back_inserter(upper));
			while (*++it);

			return upper;
		}
		else
		{
			auto str = std::make_shared<SharedString>(m_sharedString->size);

			char* ptr = m_sharedString->string.get();
			char* s = str->string.get();
			do
				*s++ = Detail::ToUpper(*ptr);
			while (*++ptr);

			*s = '\0';

			return String(std::move(str));
		}
	}

	/*!
	* \brief Trims the string
	* \return A reference to this
	*
	* \param flags Flag for the look up
	*/

	String& String::Trim(UInt32 flags)
	{
		return Set(Trimmed(flags));
	}

	/*!
	* \brief Trims the string from a character
	* \return A reference to this
	*
	* \param character Character to suppress
	* \param flags Flag for the look up
	*/

	String& String::Trim(char character, UInt32 flags)
	{
		return Set(Trimmed(character, flags));
	}

	/*!
	* \brief Trims a copy of the string
	* \return A copy of what would be the string trimmed
	*
	* \param flags Flag for the look up
	*/

	String String::Trimmed(UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return *this;

		std::size_t startPos;
		std::size_t endPos;
		if (flags & HandleUtf8)
		{
			if ((flags & TrimOnlyRight) == 0)
			{
				utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
				do
				{
					if (!Detail::IsSpace(*it))
						break;
				}
				while (*++it);

				startPos = it.base() - m_sharedString->string.get();
			}
			else
				startPos = 0;

			if ((flags & TrimOnlyLeft) == 0)
			{
				utf8::unchecked::iterator<const char*> it(&m_sharedString->string[m_sharedString->size]);
				while ((it--).base() != m_sharedString->string.get())
				{
					if (!Detail::IsSpace(*it))
						break;
				}

				endPos = it.base() - m_sharedString->string.get();
			}
			else
				endPos = m_sharedString->size-1;
		}
		else
		{
			startPos = 0;
			if ((flags & TrimOnlyRight) == 0)
			{
				for (; startPos < m_sharedString->size; ++startPos)
				{
					char c = m_sharedString->string[startPos];
					if (!Detail::IsSpace(c))
						break;
				}
			}

			endPos = m_sharedString->size-1;
			if ((flags & TrimOnlyLeft) == 0)
			{
				for (; endPos > 0; --endPos)
				{
					char c = m_sharedString->string[endPos];
					if (!Detail::IsSpace(c))
						break;
				}
			}
		}

		return SubString(startPos, endPos);
	}

	/*!
	* \brief Trims a copy of the string from a character
	* \return A copy of what would be the string trimmed
	*
	* \param character Character to suppress
	* \param flags Flag for the look up
	*/

	String String::Trimmed(char character, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return *this;

		std::size_t startPos = 0;
		std::size_t endPos = m_sharedString->size-1;
		if (flags & CaseInsensitive)
		{
			char ch = Detail::ToLower(character);
			if ((flags & TrimOnlyRight) == 0)
			{
				for (; startPos < m_sharedString->size; ++startPos)
				{
					if (Detail::ToLower(m_sharedString->string[startPos]) != ch)
						break;
				}
			}

			if ((flags & TrimOnlyLeft) == 0)
			{
				for (; endPos > 0; --endPos)
				{
					if (Detail::ToLower(m_sharedString->string[endPos]) != ch)
						break;
				}
			}
		}
		else
		{
			if ((flags & TrimOnlyRight) == 0)
			{
				for (; startPos < m_sharedString->size; ++startPos)
				{
					if (m_sharedString->string[startPos] != character)
						break;
				}
			}

			if ((flags & TrimOnlyLeft) == 0)
			{
				for (; endPos > 0; --endPos)
				{
					if (m_sharedString->string[endPos] != character)
						break;
				}
			}
		}

		return SubString(startPos, endPos);
	}

	/*!
	* \brief Returns an iterator pointing to the beginning of the string
	* \return beginning of the string
	*/

	char* String::begin()
	{
		return m_sharedString->string.get();
	}

	/*!
	* \brief Returns an iterator pointing to the beginning of the string
	* \return beginning of the string
	*/

	const char* String::begin() const
	{
		return m_sharedString->string.get();
	}

	/*!
	* \brief Returns an iterator pointing to the end of the string
	* \return End of the string
	*/

	char* String::end()
	{
		return &m_sharedString->string[m_sharedString->size];
	}

	/*!
	* \brief Returns an iterator pointing to the end of the string
	* \return End of the string
	*/

	const char* String::end() const
	{
		return &m_sharedString->string[m_sharedString->size];
	}

	/*!
	* \brief Pushed the character to the front of the string
	*
	* \param c Single character
	*
	* \see Prepend
	*/

	void String::push_front(char c)
	{
		Prepend(c);
	}

	/*!
	* \brief Pushed the character to the back of the string
	*
	* \param c Single character
	*
	* \see Append
	*/

	void String::push_back(char c)
	{
		Append(c);
	}
	/*
	char* String::rbegin()
	{
		return &m_sharedString->string[m_sharedString->size-1];
	}

	const char* String::rbegin() const
	{
		return &m_sharedString->string[m_sharedString->size-1];
	}

	char* String::rend()
	{
		return &m_sharedString->string[-1];
	}

	const char* String::rend() const
	{
		return &m_sharedString->string[-1];
	}
	*/

	/*!
	* \brief Gets the ith character in the string
	* \return A reference to the character
	*
	* \param pos Index of the character
	*
	* \remark If pos is greather than the size, Resize is called
	*/

	char& String::operator[](std::size_t pos)
	{
		EnsureOwnership();

		if (pos >= m_sharedString->size)
			Resize(pos+1);

		return m_sharedString->string[pos];
	}

	/*!
	* \brief Gets the ith character in the string
	* \return The character
	*
	* \param pos Index of the character
	*
	* \remark Produces a NazaraError if pos is greather than the size
	*/

	char String::operator[](std::size_t pos) const
	{
		#if NAZARA_CORE_SAFE
		if (pos >= m_sharedString->size)
		{
			NazaraError("Index out of range (" + Number(pos) + " >= " + Number(m_sharedString->size) + ')');
			return 0;
		}
		#endif

		return m_sharedString->string[pos];
	}

	/*!
	* \brief Assigns the string to the character
	* \return A reference to this
	*
	* \param character Single character
	*/

	String& String::operator=(char character)
	{
		return Set(character);
	}

	/*!
	* \brief Assigns the string with other "C string"
	* \return A reference to this
	*
	* \param string String to copy
	*/

	String& String::operator=(const char* string)
	{
		return Set(string);
	}

	/*!
	* \brief Assigns the string with a std::string
	* \return A reference to this
	*
	* \param string String to copy
	*/

	String& String::operator=(const std::string& string)
	{
		return Set(string);
	}

	/*!
	* \brief Assigns the string with other string
	* \return A reference to this
	*
	* \param string String to copy
	*/

	String& String::operator=(const String& string)
	{
		return Set(string);
	}

	/*!
	* \brief Assigns the string by move semantic
	* \return A reference to this
	*
	* \param string String to move
	*/

	String& String::operator=(String&& string) noexcept
	{
		return Set(std::move(string));
	}

	/*!
	* \brief Concatenates the character to the string
	* \return String which is the result of the concatenation
	*
	* \param character Single character
	*/

	String String::operator+(char character) const
	{
		if (character == '\0')
			return *this;

		auto str = std::make_shared<SharedString>(m_sharedString->size + 1);
		std::memcpy(str->string.get(), GetConstBuffer(), m_sharedString->size);
		str->string[m_sharedString->size] = character;

		return String(std::move(str));
	}

	/*!
	* \brief Concatenates the "C string" to the string
	* \return String which is the result of the concatenation
	*
	* \param string String to add
	*/

	String String::operator+(const char* string) const
	{
		if (!string || !string[0])
			return *this;

		if (m_sharedString->size == 0)
			return string;

		std::size_t length = std::strlen(string);
		if (length == 0)
			return *this;

		auto str = std::make_shared<SharedString>(m_sharedString->size + length);
		std::memcpy(str->string.get(), GetConstBuffer(), m_sharedString->size);
		std::memcpy(&str->string[m_sharedString->size], string, length+1);

		return String(std::move(str));
	}

	/*!
	* \brief Concatenates the std::string to the string
	* \return String which is the result of the concatenation
	*
	* \param string String to add
	*/

	String String::operator+(const std::string& string) const
	{
		if (string.empty())
			return *this;

		if (m_sharedString->size == 0)
			return string;

		auto str = std::make_shared<SharedString>(m_sharedString->size + string.size());
		std::memcpy(str->string.get(), GetConstBuffer(), m_sharedString->size);
		std::memcpy(&str->string[m_sharedString->size], string.c_str(), string.size()+1);

		return String(std::move(str));
	}

	/*!
	* \brief Concatenates the string to the string
	* \return String which is the result of the concatenation
	*
	* \param string String to add
	*/

	String String::operator+(const String& string) const
	{
		if (string.m_sharedString->size == 0)
			return *this;

		if (m_sharedString->size == 0)
			return string;

		auto str = std::make_shared<SharedString>(m_sharedString->size + string.m_sharedString->size);
		std::memcpy(str->string.get(), GetConstBuffer(), m_sharedString->size);
		std::memcpy(&str->string[m_sharedString->size], string.GetConstBuffer(), string.m_sharedString->size);

		return String(std::move(str));
	}

	/*!
	* \brief Concatenates the character to this string
	* \return A reference to this
	*
	* \param character Single character
	*/

	String& String::operator+=(char character)
	{
		return Insert(m_sharedString->size, character);
	}

	/*!
	* \brief Concatenates the "C string" to this string
	* \return A reference to this
	*
	* \param string String to add
	*/

	String& String::operator+=(const char* string)
	{
		return Insert(m_sharedString->size, string);
	}

	/*!
	* \brief Concatenates the std::string to this string
	* \return A reference to this
	*
	* \param string String to add
	*/

	String& String::operator+=(const std::string& string)
	{
		return Insert(m_sharedString->size, string.c_str(), string.size());
	}

	/*!
	* \brief Concatenates the string to this string
	* \return A reference to this
	*
	* \param string String to add
	*/

	String& String::operator+=(const String& string)
	{
		return Insert(m_sharedString->size, string);
	}

	/*!
	* \brief Checks whether the string is equal to the character
	* \return true if it is the case
	*
	* \param character Single character
	*/

	bool String::operator==(char character) const
	{
		if (m_sharedString->size == 0)
			return character == '\0';

		if (m_sharedString->size > 1)
			return false;

		return m_sharedString->string[0] == character;
	}

	/*!
	* \brief Checks whether the string is equal to the "C string"
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator==(const char* string) const
	{
		if (m_sharedString->size == 0)
			return !string || !string[0];

		if (!string || !string[0])
			return false;

		return std::strcmp(GetConstBuffer(), string) == 0;
	}

	/*!
	* \brief Checks whether the string is equal to the std::string
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator==(const std::string& string) const
	{
		if (m_sharedString->size == 0 || string.empty())
			return m_sharedString->size == string.size();

		if (m_sharedString->size != string.size())
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) == 0;
	}

	/*!
	* \brief Checks whether the string is equal to the character
	* \return false if it is the case
	*
	* \param character Single character
	*/

	bool String::operator!=(char character) const
	{
		if (m_sharedString->size == 0)
			return character != '\0';

		if (character == '\0' || m_sharedString->size != 1)
			return true;

		if (m_sharedString->size != 1)
			return true;

		return m_sharedString->string[0] != character;
	}

	/*!
	* \brief Checks whether the string is equal to the "C string"
	* \return false if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator!=(const char* string) const
	{
		if (m_sharedString->size == 0)
			return string && string[0];

		if (!string || !string[0])
			return true;

		return std::strcmp(GetConstBuffer(), string) != 0;
	}

	/*!
	* \brief Checks whether the string is equal to the std::string
	* \return false if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator!=(const std::string& string) const
	{
		if (m_sharedString->size == 0 || string.empty())
			return m_sharedString->size == string.size();

		if (m_sharedString->size != string.size())
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) != 0;
	}

	/*!
	* \brief Checks whether the string is less than the character
	* \return true if it is the case
	*
	* \param character Single character
	*/

	bool String::operator<(char character) const
	{
		if (character == '\0')
			return false;

		if (m_sharedString->size == 0)
			return true;

		return m_sharedString->string[0] < character;
	}

	/*!
	* \brief Checks whether the string is less than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator<(const char* string) const
	{
		if (!string || !string[0])
			return false;

		if (m_sharedString->size == 0)
			return true;

		return std::strcmp(GetConstBuffer(), string) < 0;
	}

	/*!
	* \brief Checks whether the string is less than the std::string
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator<(const std::string& string) const
	{
		if (string.empty())
			return false;

		if (m_sharedString->size == 0)
			return true;

		return std::strcmp(GetConstBuffer(), string.c_str()) < 0;
	}

	/*!
	* \brief Checks whether the string is less or equal than the character
	* \return true if it is the case
	*
	* \param character Single character
	*/

	bool String::operator<=(char character) const
	{
		if (m_sharedString->size == 0)
			return true;

		if (character == '\0')
			return false;

		return m_sharedString->string[0] < character || (m_sharedString->string[0] == character && m_sharedString->size == 1);
	}

	/*!
	* \brief Checks whether the string is less or equal than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator<=(const char* string) const
	{
		if (m_sharedString->size == 0)
			return true;

		if (!string || !string[0])
			return false;

		return std::strcmp(GetConstBuffer(), string) <= 0;
	}

	/*!
	* \brief Checks whether the string is less or equal than the std::string
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator<=(const std::string& string) const
	{
		if (m_sharedString->size == 0)
			return true;

		if (string.empty())
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) <= 0;
	}

	/*!
	* \brief Checks whether the string is greather than the character
	* \return true if it is the case
	*
	* \param character Single character
	*/

	bool String::operator>(char character) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (character == '\0')
			return true;

		return m_sharedString->string[0] > character;
	}

	/*!
	* \brief Checks whether the string is greather than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator>(const char* string) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (!string || !string[0])
			return true;

		return std::strcmp(GetConstBuffer(), string) > 0;
	}

	/*!
	* \brief Checks whether the string is greather than the std::string
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator>(const std::string& string) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (string.empty())
			return true;

		return std::strcmp(GetConstBuffer(), string.c_str()) > 0;
	}

	/*!
	* \brief Checks whether the string is greather or equal than the character
	* \return true if it is the case
	*
	* \param character Single character
	*/

	bool String::operator>=(char character) const
	{
		if (character == '\0')
			return true;

		if (m_sharedString->size == 0)
			return false;

		return m_sharedString->string[0] > character || (m_sharedString->string[0] == character && m_sharedString->size == 1);
	}

	/*!
	* \brief Checks whether the string is greather or equal than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator>=(const char* string) const
	{
		if (!string || !string[0])
			return true;

		if (m_sharedString->size == 0)
			return false;

		return std::strcmp(GetConstBuffer(), string) >= 0;
	}

	/*!
	* \brief Checks whether the string is greather or equal than the std::string
	* \return true if it is the case
	*
	* \param string String to compare
	*/

	bool String::operator>=(const std::string& string) const
	{
		if (string.empty())
			return true;

		if (m_sharedString->size == 0)
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) >= 0;
	}

	/*!
	* \brief Converts the boolean to string
	* \return String representation of the boolean
	*
	* \param boolean Boolean value
	*/

	String String::Boolean(bool boolean)
	{
		std::size_t size = (boolean) ? 4 : 5;

		auto str = std::make_shared<SharedString>(size);
		std::memcpy(str->string.get(), (boolean) ? "true" : "false", size);

		return String(std::move(str));
	}

	/*!
	* \brief Lexicographically compares the string
	* \return The expected result
	*
	* \param first First string to use for comparison
	* \param second Second string to use for comparison
	*/

	int String::Compare(const String& first, const String& second)
	{
		if (first.m_sharedString->size == 0)
			return (second.m_sharedString->size == 0) ? 0 : -1;

		if (second.m_sharedString->size == 0)
			return 1;

		return std::strcmp(first.GetConstBuffer(), second.GetConstBuffer());
	}

	/*!
	* \brief Build a string using a format and returns it
	* \return Formatted string
	*
	* \param format String format
	* \param args Format arguments
	*/
	String String::FormatVA(const char* format, va_list args)
	{
		// Copy va_list to use it twice
		va_list args2;
		va_copy(args2, args);

		std::size_t length = std::vsnprintf(nullptr, 0, format, args);

		auto str = std::make_shared<SharedString>(length);
		std::vsnprintf(str->string.get(), length + 1, format, args2);

		return String(std::move(str));
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Float value
	*/

	String String::Number(float number)
	{
		std::ostringstream oss;
		oss.precision(NAZARA_CORE_DECIMAL_DIGITS);
		oss << number;

		return String(oss.str());
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Double value
	*/

	String String::Number(double number)
	{
		std::ostringstream oss;
		oss.precision(NAZARA_CORE_DECIMAL_DIGITS);
		oss << number;

		return String(oss.str());
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Long double value
	*/

	String String::Number(long double number)
	{
		std::ostringstream oss;
		oss.precision(NAZARA_CORE_DECIMAL_DIGITS);
		oss << number;

		return String(oss.str());
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Signed char value
	* \param radix Base of the number
	*/

	String String::Number(signed char number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Unsigned char value
	* \param radix Base of the number
	*/

	String String::Number(unsigned char number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Short value
	* \param radix Base of the number
	*/

	String String::Number(short number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Unsigned short value
	* \param radix Base of the number
	*/

	String String::Number(unsigned short number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Int value
	* \param radix Base of the number
	*/

	String String::Number(int number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Unsigned int value
	* \param radix Base of the number
	*/

	String String::Number(unsigned int number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Long value
	* \param radix Base of the number
	*/

	String String::Number(long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Unsigned long value
	* \param radix Base of the number
	*/

	String String::Number(unsigned long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Long long value
	* \param radix Base of the number
	*/

	String String::Number(long long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the number to string
	* \return String representation of the number
	*
	* \param number Unsigned long long value
	* \param radix Base of the number
	*/

	String String::Number(unsigned long long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	/*!
	* \brief Converts the pointer to string
	* \return String representation of the pointer
	*
	* \param ptr Pointer to represent
	*/

	String String::Pointer(const void* ptr)
	{
		const std::size_t capacity = sizeof(void*)*2 + 2;

		auto str = std::make_shared<SharedString>(capacity);
		str->size = std::sprintf(str->string.get(), "0x%p", ptr);

		return String(std::move(str));
	}

	/*!
	* \brief Converts the unicode point to string
	* \return String representation of the unicode point
	*
	* \param character Unicode point
	*/

	String String::Unicode(char32_t character)
	{
		if (character == '\0')
			return String();

		std::size_t count = 0;
		if (character < 0x80)
			count = 1;
		else if (character < 0x800)
			count = 2;
		else if (character < 0x10000)
			count = 3;
		else
			count = 4;

		auto str = std::make_shared<SharedString>(count);
		utf8::append(character, str->string.get());

		return String(std::move(str));
	}

	/*!
	* \brief Converts the unicode "C string" to string
	* \return String representation of the unicode "C string"
	*
	* \param u8String String in UTF-8
	*/

	String String::Unicode(const char* u8String)
	{
		return String(u8String);
	}

	/*!
	* \brief Converts the unicode "C string" to string
	* \return String representation of the unicode "C string"
	*
	* \param u16String String in UTF-16
	*/

	String String::Unicode(const char16_t* u16String)
	{
		if (!u16String || !u16String[0])
			return String();

		const char16_t* ptr = u16String;
		std::size_t count = 0;
		do
			count++;
		while (*++ptr);

		count *= 2; // We ensure to have enough place

		auto str = std::make_shared<SharedString>(count);

		char* r = utf8::utf16to8(u16String, ptr, str->string.get());
		*r = '\0';

		str->size = r - str->string.get();

		return String(std::move(str));
	}

	/*!
	* \brief Converts the unicode "C string" to string
	* \return String representation of the unicode "C string"
	*
	* \param u32String String in UTF-32
	*/

	String String::Unicode(const char32_t* u32String)
	{
		if (!u32String || !u32String[0])
			return String();

		const char32_t* ptr = u32String;
		std::size_t count = 0;
		do
		{
			char32_t cp = *ptr;
			if (cp < 0x80)
				count += 1;
			else if (cp < 0x800)
				count += 2;
			else if (cp < 0x10000)
				count += 3;
			else
				count += 4;
		}
		while (*++ptr);

		auto str = std::make_shared<SharedString>(count);
		utf8::utf32to8(u32String, ptr, str->string.get());

		return String(std::move(str));
	}

	/*!
	* \brief Converts the unicode "C string" to string
	* \return String representation of the unicode "C string"
	*
	* \param wString String in Wide
	*/

	String String::Unicode(const wchar_t* wString)
	{
		if (!wString || !wString[0])
			return String();

		const wchar_t* ptr = wString;
		std::size_t count = 0;
		do
		{
			char32_t cp = *ptr;
			if (cp < 0x80)
				count += 1;
			else if (cp < 0x800)
				count += 2;
			else if (cp < 0x10000)
				count += 3;
			else
				count += 4;
		}
		while (*++ptr);

		auto str = std::make_shared<SharedString>(count);
		utf8::utf32to8(wString, ptr, str->string.get());

		return String(std::move(str));
	}

	/*!
	* \brief Inputs the stream into the string
	* \return A reference to the stream
	*
	* \param is Stream to get information from
	* \param str String to set value
	*/

	std::istream& operator>>(std::istream& is, String& str)
	{
		str.Clear();

		char c;
		for (;;)
		{
			is.get(c);
			if (c == '\0')
				break;
			else if (std::isspace(c))
			{
				if (str.IsNull())
					continue;
				else
					break;
			}
			else
				str += c;
		}

		return is;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param os The stream
	* \param str The string to output
	*/

	std::ostream& operator<<(std::ostream& os, const String& str)
	{
		if (str.IsEmpty())
			return os;

		return operator<<(os, str.m_sharedString->string.get());
	}

	/*!
	* \brief Concatenates the character to the string
	* \return String which is the result of the concatenation
	*
	* \param character Single character
	* \param string String in the right hand side
	*/

	String operator+(char character, const String& string)
	{
		if (character == '\0')
			return string;

		if (string.IsEmpty())
			return String(character);

		auto str = std::make_shared<String::SharedString>(string.m_sharedString->size + 1);
		str->string[0] = character;
		std::memcpy(&str->string[1], string.GetConstBuffer(), string.m_sharedString->size);

		return String(std::move(str));
	}

	/*!
	* \brief Concatenates the "C string" to the string
	* \return String which is the result of the concatenation
	*
	* \param string String to add
	* \param nstring String in the right hand side
	*/

	String operator+(const char* string, const String& nstring)
	{
		if (!string || !string[0])
			return nstring;

		if (nstring.IsEmpty())
			return string;

		std::size_t size = std::strlen(string);
		std::size_t totalSize = size + nstring.m_sharedString->size;

		auto str = std::make_shared<String::SharedString>(totalSize);
		std::memcpy(str->string.get(), string, size);
		std::memcpy(&str->string[size], nstring.GetConstBuffer(), nstring.m_sharedString->size+1);

		return String(std::move(str));
	}

	/*!
	* \brief Concatenates the std::string to the string
	* \return String which is the result of the concatenation
	*
	* \param string String to add
	* \param nstring String in the right hand side
	*/

	String operator+(const std::string& string, const String& nstring)
	{
		if (string.empty())
			return nstring;

		if (nstring.m_sharedString->size == 0)
			return string;

		std::size_t totalSize = string.size() + nstring.m_sharedString->size;

		auto str = std::make_shared<String::SharedString>(totalSize);
		std::memcpy(str->string.get(), string.c_str(), string.size());
		std::memcpy(&str->string[string.size()], nstring.GetConstBuffer(), nstring.m_sharedString->size+1);

		return String(std::move(str));
	}

	/*!
	* \brief Checks whether the first string is equal to the second string
	* \return true if it is the case
	*
	* \param first String to compare in left hand side
	* \param second String to compare in right hand side
	*/

	bool operator==(const String& first, const String& second)
	{
		if (first.m_sharedString->size == 0 || second.m_sharedString->size == 0)
			return first.m_sharedString->size == second.m_sharedString->size;

		if (first.m_sharedString->size != second.m_sharedString->size)
			return false;

		if (first.m_sharedString == second.m_sharedString)
			return true;

		return std::strcmp(first.GetConstBuffer(), second.GetConstBuffer()) == 0;
	}

	/*!
	* \brief Checks whether the first string is equal to the second string
	* \return false if it is the case
	*
	* \param first String to compare in left hand side
	* \param second String to compare in right hand side
	*/

	bool operator!=(const String& first, const String& second)
	{
		return !operator==(first, second);
	}

	/*!
	* \brief Checks whether the first string is less than the second string
	* \return true if it is the case
	*
	* \param first String to compare in left hand side
	* \param second String to compare in right hand side
	*/

	bool operator<(const String& first, const String& second)
	{
		if (second.m_sharedString->size == 0)
			return false;

		if (first.m_sharedString->size == 0)
			return true;

		return std::strcmp(first.GetConstBuffer(), second.GetConstBuffer()) < 0;
	}

	/*!
	* \brief Checks whether the first string is less or equal than the second string
	* \return true if it is the case
	*
	* \param first String to compare in left hand side
	* \param second String to compare in right hand side
	*/

	bool operator<=(const String& first, const String& second)
	{
		return !operator<(second, first);
	}

	/*!
	* \brief Checks whether the first string is greather than the second string
	* \return true if it is the case
	*
	* \param first String to compare in left hand side
	* \param second String to compare in right hand side
	*/

	bool operator>(const String& first, const String& second)
	{
		return second < first;
	}

	/*!
	* \brief Checks whether the first string is greather or equal than the second string
	* \return true if it is the case
	*
	* \param first String to compare in left hand side
	* \param second String to compare in right hand side
	*/

	bool operator>=(const String& first, const String& second)
	{
		return !operator<(first, second);
	}

	/*!
	* \brief Checks whether the string is equal to the character
	* \return true if it is the case
	*
	* \param character Single character in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator==(char character, const String& nstring)
	{
		return nstring == character;
	}

	/*!
	* \brief Checks whether the string is equal to the "C string"
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator==(const char* string, const String& nstring)
	{
		return nstring == string;
	}

	/*!
	* \brief Checks whether the string is equal to the std::string
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator==(const std::string& string, const String& nstring)
	{
		return nstring == string;
	}

	/*!
	* \brief Checks whether the string is equal to the character
	* \return false if it is the case
	*
	* \param character Single character in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator!=(char character, const String& nstring)
	{
		return !operator==(character, nstring);
	}

	/*!
	* \brief Checks whether the string is equal to the "C string"
	* \return false if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator!=(const char* string, const String& nstring)
	{
		return !operator==(string, nstring);
	}

	/*!
	* \brief Checks whether the string is equal to the std::string
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator!=(const std::string& string, const String& nstring)
	{
		return !operator==(string, nstring);
	}

	/*!
	* \brief Checks whether the string is less than the character
	* \return true if it is the case
	*
	* \param character Single character in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator<(char character, const String& nstring)
	{
		return nstring > character;
	}

	/*!
	* \brief Checks whether the string is less than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator<(const char* string, const String& nstring)
	{
		return nstring > string;
	}

	/*!
	* \brief Checks whether the string is less than the std::string
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator<(const std::string& string, const String& nstring)
	{
		return nstring > string;
	}

	/*!
	* \brief Checks whether the string is less or equal than the character
	* \return true if it is the case
	*
	* \param character Single character in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator<=(char character, const String& nstring)
	{
		return !operator<(nstring, String(character));
	}

	/*!
	* \brief Checks whether the string is less or equal than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator<=(const char* string, const String& nstring)
	{
		return !operator<(nstring, string);
	}

	/*!
	* \brief Checks whether the string is less or equal than the std::string
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator<=(const std::string& string, const String& nstring)
	{
		return !operator<(nstring, string);
	}

	/*!
	* \brief Checks whether the string is greather than the character
	* \return true if it is the case
	*
	* \param character Single character in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator>(char character, const String& nstring)
	{
		return nstring < character;
	}

	/*!
	* \brief Checks whether the string is greather than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator>(const char* string, const String& nstring)
	{
		return nstring < string;
	}

	/*!
	* \brief Checks whether the string is greather than the std::string
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator>(const std::string& string, const String& nstring)
	{
		return nstring < string;
	}

	/*!
	* \brief Checks whether the string is greather or equal than the character
	* \return true if it is the case
	*
	* \param character Single character in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator>=(char character, const String& nstring)
	{
		return !operator<(character, nstring);
	}

	/*!
	* \brief Checks whether the string is greather or equal than the "C string"
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator>=(const char* string, const String& nstring)
	{
		return !operator<(string, nstring);
	}

	/*!
	* \brief Checks whether the string is greather or equal than the std::string
	* \return true if it is the case
	*
	* \param string String to compare in left hand side
	* \param nstring String to compare in right hand side
	*/

	bool operator>=(const std::string& string, const String& nstring)
	{
		return !operator<(string, nstring);
	}

	/*!
	* \brief Ensures the ownership of the string
	*
	* \param discardContent Should discard the content
	*/

	void String::EnsureOwnership(bool discardContent)
	{
		if (!m_sharedString)
			return;

		if (!m_sharedString.unique())
		{
			auto newSharedString = std::make_shared<SharedString>(GetSize(), GetCapacity());
			if (!discardContent && newSharedString->size > 0)
				std::memcpy(newSharedString->string.get(), GetConstBuffer(), GetSize()+1);

			m_sharedString = std::move(newSharedString);
		}
	}

	/*!
	* \brief Gets the empty string
	* \return A reference to the empty string
	*/

	const std::shared_ptr<String::SharedString>& String::GetEmptyString()
	{
		static auto emptyString = std::make_shared<SharedString>();
		return emptyString;
	}

	/*!
	* \brief Serializes a string
	* \return true if successful
	*
	* \param context Context of serialization
	* \param string String to serialize
	*/
	bool Serialize(SerializationContext& context, const String& string, TypeTag<String>)
	{
		if (!Serialize(context, UInt32(string.GetSize())))
			return false;

		return context.stream->Write(string.GetConstBuffer(), string.GetSize()) == string.GetSize();
	}

	/*!
	* \brief Unserializes a string
	* \return true if successful
	*
	* \param context Context of unserialization
	* \param string String to unserialize
	*/
	bool Unserialize(SerializationContext& context, String* string, TypeTag<String>)
	{
		UInt32 size;
		if (!Unserialize(context, &size))
			return false;

		string->Resize(size);
		return context.stream->Read(string->GetBuffer(), size) == size;
	}

	const std::size_t String::npos(std::numeric_limits<std::size_t>::max());
}

namespace std
{
	/*!
	* \brief Gets the line from the input stream
	* \return A reference to the stream
	*
	* \param is Input stream to get information from
	* \param str String to set
	*/

	istream& getline(istream& is, Nz::String& str)
	{
		return getline(is, str, is.widen('\n'));
	}

	/*!
	* \brief Gets the line from the input stream
	* \return A reference to the stream
	*
	* \param is Input stream to get information from
	* \param str String to set
	* \param delim Delimitor defining the end
	*/

	istream& getline(istream& is, Nz::String& str, char delim)
	{
		str.Clear();

		char c;

		for (;;)
		{
			is.get(c);
			if (c != delim && c != '\0')
				str += c;
			else
			{
				if (c == '\0')
					is.setstate(std::ios_base::eofbit);
				break;
			}
		}

		return is;
	}

	/*!
	* \brief Swaps two strings, specialisation of std
	*
	* \param lhs First string
	* \param rhs Second string
	*/

	void swap(Nz::String& lhs, Nz::String& rhs)
	{
		lhs.Swap(rhs);
	}
}
