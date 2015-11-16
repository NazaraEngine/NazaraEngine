// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

///TODO: Réécrire une bonne partie des algorithmes employés (Relu jusqu'à 3538)

#include <Nazara/Core/String.hpp>
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
		// Cet algorithme est inspiré de la documentation de Qt
		inline unsigned int GetNewSize(unsigned int newSize)
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

			while (!(ret = static_cast<unsigned char>(Detail::ToLower(*s1)) - static_cast<unsigned char>(Detail::ToLower(*s2))) && *s2)
				++s1, ++s2;

			return ret != 0 ? (ret > 0 ? 1 : -1) : 0;
		}

		inline int Unicodecasecmp(const char* s1, const char* s2)
		{
			int ret = 0;
			utf8::unchecked::iterator<const char*> it1(s1);
			utf8::unchecked::iterator<const char*> it2(s2);

			while (!(ret = Unicode::GetLowercase(*it1) - Unicode::GetLowercase(*it2)) && *it2)
				++it1, ++it2;

			return ret != 0 ? (ret > 0 ? 1 : -1) : 0;
		}
	}

	String::String() :
	m_sharedString(GetEmptyString())
	{
	}

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

	String::String(unsigned int rep, char character)
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

	String::String(unsigned int rep, const char* string) :
	String(rep, string, (string) ? std::strlen(string) : 0)
	{
	}

	String::String(unsigned int rep, const char* string, unsigned int length)
	{
		unsigned int totalSize = rep*length;

		if (totalSize > 0)
		{
			m_sharedString = std::make_shared<SharedString>(totalSize);

			for (unsigned int i = 0; i < rep; ++i)
				std::memcpy(&m_sharedString->string[i*length], string, length);
		}
		else
			m_sharedString = GetEmptyString();
	}

	String::String(unsigned int rep, const String& string) :
	String(rep, string.GetConstBuffer(), string.GetSize())
	{
	}

	String::String(const char* string) :
	String(string, (string) ? std::strlen(string) : 0)
	{
	}

	String::String(const char* string, unsigned int length)
	{
		if (length > 0)
		{
			m_sharedString = std::make_shared<SharedString>(length);
			std::memcpy(m_sharedString->string.get(), string, length);
		}
		else
			m_sharedString = GetEmptyString();
	}

	String::String(const std::string& string) :
	String(string.c_str(), string.size())
	{
	}

	String& String::Append(char character)
	{
		return Insert(m_sharedString->size, character);
	}

	String& String::Append(const char* string)
	{
		return Insert(m_sharedString->size, string);
	}

	String& String::Append(const char* string, unsigned int length)
	{
		return Insert(m_sharedString->size, string, length);
	}

	String& String::Append(const String& string)
	{
		return Insert(m_sharedString->size, string);
	}

	void String::Clear(bool keepBuffer)
	{
		if (keepBuffer)
		{
			EnsureOwnership(true);
			m_sharedString->size = 0;
			m_sharedString->string.reset();
		}
		else
			ReleaseString();
	}

	bool String::Contains(char character, int start, UInt32 flags) const
	{
		return Find(character, start, flags) != npos;
	}

	bool String::Contains(const char* string, int start, UInt32 flags) const
	{
		return Find(string, start, flags) != npos;
	}

	bool String::Contains(const String& string, int start, UInt32 flags) const
	{
		return Find(string, start, flags) != npos;
	}

	unsigned int String::Count(char character, int start, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return 0;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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
			while ((str = std::strchr(str, character)))
			{
				count++;
				str++;
			}
		}

		return count;
	}

	unsigned int String::Count(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return 0;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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
						while (true)
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
						while (true)
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
			while ((str = std::strstr(str, string)))
			{
				count++;
				str++;
			}
		}

		return count;
	}

	unsigned int String::Count(const String& string, int start, UInt32 flags) const
	{
		return Count(string.GetConstBuffer(), start, flags);
	}

	unsigned int String::CountAny(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return 0;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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
				while ((str = std::strpbrk(str, string)))
				{
					count++;
					str++;
				}
			}
		}

		return count;
	}

	unsigned int String::CountAny(const String& string, int start, UInt32 flags) const
	{
		return CountAny(string.GetConstBuffer(), start, flags);
	}

	bool String::EndsWith(char character, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return 0;

		if (flags & CaseInsensitive)
			return Detail::ToLower(m_sharedString->string[m_sharedString->size-1]) == Detail::ToLower(character);
		else
			return m_sharedString->string[m_sharedString->size-1] == character; // character == '\0' sera toujours faux
	}

	bool String::EndsWith(const char* string, UInt32 flags) const
	{
		return EndsWith(string, std::strlen(string), flags);
	}

	bool String::EndsWith(const char* string, unsigned int length, UInt32 flags) const
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

	bool String::EndsWith(const String& string, UInt32 flags) const
	{
		return EndsWith(string.GetConstBuffer(), string.m_sharedString->size, flags);
	}

	unsigned int String::Find(char character, int start, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		if (flags & CaseInsensitive)
		{
			char ch = Detail::ToLower(character);
			const char* str = m_sharedString->string.get();
			do
			{
				if (Detail::ToLower(*str) == ch)
					return static_cast<unsigned int>(str - m_sharedString->string.get());
			}
			while (*++str);

			return npos;
		}
		else
		{
			char* ch = std::strchr(&m_sharedString->string[pos], character);
			if (ch)
				return static_cast<unsigned int>(ch - m_sharedString->string.get());
			else
				return npos;
		}
	}

	unsigned int String::Find(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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
						while (true)
						{
							if (*it2 == '\0')
								return static_cast<unsigned int>(ptrPos - m_sharedString->string.get());

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
						while (true)
						{
							if (*ptr == '\0')
								return static_cast<unsigned int>(ptrPos - m_sharedString->string.get());

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
				return static_cast<unsigned int>(ch - m_sharedString->string.get());
		}

		return npos;
	}

	unsigned int String::Find(const String& string, int start, UInt32 flags) const
	{
		return Find(string.GetConstBuffer(), start, flags);
	}

	unsigned int String::FindAny(const char* string, int start, UInt32 flags) const
	{
		if (m_sharedString->size == 0 || !string || !string[0])
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size+start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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

	unsigned int String::FindAny(const String& string, int start, UInt32 flags) const
	{
		return FindAny(string.GetConstBuffer(), start, flags);
	}

	unsigned int String::FindLast(char character, int start, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* ptr = &m_sharedString->string[m_sharedString->size-1];

		if (flags & CaseInsensitive)
		{
			character = Detail::ToLower(character);
			do
			{
				if (Detail::ToLower(*ptr) == character)
					return static_cast<unsigned int>(ptr - m_sharedString->string.get());
			}
			while (ptr-- != m_sharedString->string.get());
		}
		else
		{
			do
			{
				if (*ptr == character)
					return static_cast<unsigned int>(ptr - m_sharedString->string.get());
			}
			while (ptr-- != m_sharedString->string.get());
		}

		return npos;
	}

	unsigned int String::FindLast(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		///Algo 1.FindLast#3 (Taille du pattern inconnue)
		const char* ptr = &m_sharedString->string[pos];
		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				if (utf8::internal::is_trail(*ptr))
					utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

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

						while (true)
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
						while (true)
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
					while (true)
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

	unsigned int String::FindLast(const String& string, int start, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size || string.m_sharedString->size > m_sharedString->size)
			return npos;

		const char* ptr = &m_sharedString->string[pos];
		const char* limit = &m_sharedString->string[string.m_sharedString->size-1];

		if (flags & CaseInsensitive)
		{
			if (flags & HandleUtf8)
			{
				///Algo 1.FindLast#3 (Itérateur non-adapté)
				if (utf8::internal::is_trail(*ptr))
					utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

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

						while (true)
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
				///Algo 1.FindLast#4 (Taille du pattern connue)
				char c = Detail::ToLower(string.m_sharedString->string[string.m_sharedString->size-1]);
				while (true)
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
			///Algo 1.FindLast#4 (Taille du pattern connue)
			while (true)
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

	unsigned int String::FindLastAny(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size+start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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

	unsigned int String::FindLastAny(const String& string, int start, UInt32 flags) const
	{
		return FindLastAny(string.GetConstBuffer(), start, flags);
	}

	unsigned int String::FindLastWord(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		///Algo 2.FindLastWord#1 (Taille du pattern inconnue)
		const char* ptr = &m_sharedString->string[pos];

		if (flags & HandleUtf8)
		{
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*tIt) & Unicode::Category_Separator)
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
				const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = utf8::unchecked::next(t);
				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*tIt) & Unicode::Category_Separator)
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
						if (ptr != m_sharedString->string.get() && !std::isspace(*(ptr-1)))
							continue;

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						while (true)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || std::isspace(*tPtr))
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
						if (ptr != m_sharedString->string.get() && !std::isspace(*(ptr-1)))
							continue;

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						while (true)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || std::isspace(*tPtr))
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

	unsigned int String::FindLastWord(const String& string, int start, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		const char* ptr = &m_sharedString->string[pos];
		const char* limit = &m_sharedString->string[string.m_sharedString->size-1];

		if (flags & HandleUtf8)
		{
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));
				do
				{
					if (Unicode::GetLowercase(*it) == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*tIt) & Unicode::Category_Separator)
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
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = utf8::unchecked::next(t);
				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*tIt) & Unicode::Category_Separator)
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
			///Algo 2.FindLastWord#2 (Taille du pattern connue)
			if (flags & CaseInsensitive)
			{
				char c = Detail::ToLower(string.m_sharedString->string[string.m_sharedString->size-1]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						if (*(ptr+1) != '\0' && !std::isspace(*(ptr+1)))
							continue;

						const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
						for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
						{
							if (Detail::ToLower(*ptr) != Detail::ToLower(*p))
								break;

							if (p == &string.m_sharedString->string[0])
							{
								if (ptr == m_sharedString->string.get() || std::isspace(*(ptr-1)))
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
						if (*(ptr+1) != '\0' && !std::isspace(*(ptr+1)))
							continue;

						const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
						for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
						{
							if (*ptr != *p)
								break;

							if (p == &string.m_sharedString->string[0])
							{
								if (ptr == m_sharedString->string.get() || std::isspace(*(ptr-1)))
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

	unsigned int String::FindWord(const char* string, int start, UInt32 flags) const
	{
		if (!string || !string[0] || m_sharedString->size == 0)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		///Algo 3.FindWord#3 (Taille du pattern inconnue)
		const char* ptr = m_sharedString->string.get();
		if (flags & HandleUtf8)
		{
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*it++) & Unicode::Category_Separator)
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
				const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*it++) & Unicode::Category_Separator)
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
						if (ptr != m_sharedString->string.get() && !std::isspace(*(ptr-1)))
							continue;

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						while (true)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || std::isspace(*tPtr))
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
						if (ptr != m_sharedString->string.get() && !std::isspace(*(ptr-1)))
							continue;

						const char* p = &string[1];
						const char* tPtr = ptr+1;
						while (true)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || std::isspace(*tPtr))
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

	unsigned int String::FindWord(const String& string, int start, UInt32 flags) const
	{
		if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
			return npos;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return npos;

		char* ptr = m_sharedString->string.get();
		if (flags & HandleUtf8)
		{
			///Algo 3.FindWord#3 (Itérateur trop lent pour #2)
			if (utf8::internal::is_trail(*ptr))
				utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

			utf8::unchecked::iterator<const char*> it(ptr);

			if (flags & CaseInsensitive)
			{
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*it++) & Unicode::Category_Separator)
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
				const char* t = string.GetConstBuffer(); // utf8(::unchecked)::next affecte l'itérateur en argument
				UInt32 c = Unicode::GetLowercase(utf8::unchecked::next(t));

				do
				{
					if (*it == c)
					{
						if (it.base() != m_sharedString->string.get())
						{
							--it;
							if (!(Unicode::GetCategory(*it++) & Unicode::Category_Separator))
								continue;
						}

						utf8::unchecked::iterator<const char*> p(t);
						utf8::unchecked::iterator<const char*> tIt = it;
						++tIt;

						while (true)
						{
							if (*p == '\0')
							{
								if (*tIt == '\0' || Unicode::GetCategory(*it++) & Unicode::Category_Separator)
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
			///Algo 3.FindWord#2 (Taille du pattern connue)
			if (flags & CaseInsensitive)
			{
				char c = Detail::ToLower(string.m_sharedString->string[0]);
				do
				{
					if (Detail::ToLower(*ptr) == c)
					{
						if (ptr != m_sharedString->string.get() && !std::isspace(*(ptr-1)))
							continue;

						const char* p = &string.m_sharedString->string[1];
						const char* tPtr = ptr+1;
						while (true)
						{
							if (*p == '\0')
							{
								if (*tPtr == '\0' || std::isspace(*tPtr))
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
				while ((ptr = std::strstr(ptr, string.GetConstBuffer())))
				{
					// Si le mot est bien isolé
					if ((ptr == m_sharedString->string.get() || std::isspace(*(ptr-1))) && (*(ptr+m_sharedString->size) == '\0' || std::isspace(*(ptr+m_sharedString->size))))
						return ptr - m_sharedString->string.get();

					ptr++;
				}
			}
		}

		return npos;
	}

	char* String::GetBuffer()
	{
		EnsureOwnership();

		return m_sharedString->string.get();
	}

	unsigned int String::GetCapacity() const
	{
		return m_sharedString->capacity;
	}

	const char* String::GetConstBuffer() const
	{
		return m_sharedString->string.get();
	}

	unsigned int String::GetLength() const
	{
		return utf8::distance(m_sharedString->string.get(), &m_sharedString->string[m_sharedString->size]);
	}

	unsigned int String::GetSize() const
	{
		return m_sharedString->size;
	}

	std::string String::GetUtf8String() const
	{
		return std::string(m_sharedString->string.get(), m_sharedString->size);
	}

	std::u16string String::GetUtf16String() const
	{
		if (m_sharedString->size == 0)
			return std::u16string();

		std::u16string str;
		str.reserve(m_sharedString->size);

		utf8::utf8to16(begin(), end(), std::back_inserter(str));

		return str;
	}

	std::u32string String::GetUtf32String() const
	{
		if (m_sharedString->size == 0)
			return std::u32string();

		std::u32string str;
		str.reserve(m_sharedString->size);

		utf8::utf8to32(begin(), end(), std::back_inserter(str));

		return str;
	}

	std::wstring String::GetWideString() const
	{
		static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "wchar_t size is not supported");
		if (m_sharedString->size == 0)
			return std::wstring();

		std::wstring str;
		str.reserve(m_sharedString->size);

		if (sizeof(wchar_t) == 4) // Je veux du static_if :(
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

	String String::GetWord(unsigned int index, UInt32 flags) const
	{
		unsigned int startPos = GetWordPosition(index, flags);
		if (startPos == npos)
			return String();

		int endPos = -1;
		const char* ptr = &m_sharedString->string[startPos];
		if (flags & HandleUtf8)
		{
			utf8::unchecked::iterator<const char*> it(ptr);
			do
			{
				if (Unicode::GetCategory(*it) & Unicode::Category_Separator)
				{
					endPos = static_cast<int>(it.base() - m_sharedString->string.get() - 1);
					break;
				}
			}
			while (*++it);
		}
		else
		{
			do
			{
				if (std::isspace(*ptr))
				{
					endPos = static_cast<int>(ptr - m_sharedString->string.get() - 1);
					break;
				}
			}
			while (*++ptr);
		}

		return SubString(startPos, endPos);
	}

	unsigned int String::GetWordPosition(unsigned int index, UInt32 flags) const
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
				if (Unicode::GetCategory(*it) & Unicode::Category_Separator)
					inWord = false;
				else
				{
					if (!inWord)
					{
						inWord = true;
						if (++currentWord > index)
							return static_cast<unsigned int>(it.base() - m_sharedString->string.get());
					}
				}
			}
			while (*++it);
		}
		else
		{
			do
			{
				if (std::isspace(*ptr))
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

	String& String::Insert(int pos, char character)
	{
		return Insert(pos, &character, 1);
	}

	String& String::Insert(int pos, const char* string)
	{
		return Insert(pos, string, std::strlen(string));
	}

	String& String::Insert(int pos, const char* string, unsigned int length)
	{
		if (length == 0)
			return *this;

		if (pos < 0)
			pos = std::max(m_sharedString->size + pos, 0U);

		unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedString->size);

		// Si le buffer est déjà suffisamment grand
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

	String& String::Insert(int pos, const String& string)
	{
		return Insert(pos, string.GetConstBuffer(), string.m_sharedString->size);
	}

	bool String::IsEmpty() const
	{
		return m_sharedString->size == 0;
	}

	bool String::IsNull() const
	{
		return m_sharedString.get() == GetEmptyString().get();
	}

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

	bool String::Match(const String& pattern) const
	{
		return Match(pattern.m_sharedString->string.get());
	}

	String& String::Prepend(char character)
	{
		return Insert(0, character);
	}

	String& String::Prepend(const char* string)
	{
		return Insert(0, string);
	}

	String& String::Prepend(const char* string, unsigned int length)
	{
		return Insert(0, string, length);
	}

	String& String::Prepend(const String& string)
	{
		return Insert(0, string);
	}

	unsigned int String::Replace(char oldCharacter, char newCharacter, int start, UInt32 flags)
	{
		if (oldCharacter == '\0' || oldCharacter == newCharacter)
			return 0;

		if (newCharacter == '\0') // Dans ce cas, il faut passer par un algorithme plus complexe
			return Replace(String(oldCharacter), String(), start);

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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
						unsigned int offset = ptr-m_sharedString->string.get();

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
			while ((ptr = std::strchr(ptr, oldCharacter)))
			{
				if (!found)
				{
					unsigned int offset = ptr-m_sharedString->string.get();

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

	unsigned int String::Replace(const char* oldString, const char* replaceString, int start, UInt32 flags)
	{
		return Replace(oldString, std::strlen(oldString), replaceString, std::strlen(replaceString), start, flags);
	}

	unsigned int String::Replace(const char* oldString, unsigned int oldLength, const char* replaceString, unsigned int replaceLength, int start, UInt32 flags)
	{
		if (oldLength == 0)
			return 0;

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
		if (pos >= m_sharedString->size)
			return 0;

		unsigned int count = 0;
		if (oldLength == replaceLength)
		{
			bool found = false;

			// Si aucun changement de taille n'est nécessaire, nous pouvons alors utiliser un algorithme bien plus rapide
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
		else ///TODO: Algorithme de remplacement sans changement de buffer (si replaceLength < oldLength)
		{
			unsigned int newSize = m_sharedString->size + Count(oldString)*(replaceLength - oldLength);
			if (newSize == m_sharedString->size) // Alors c'est que Count(oldString) == 0
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

	unsigned int String::Replace(const String& oldString, const String& replaceString, int start, UInt32 flags)
	{
		return Replace(oldString.GetConstBuffer(), oldString.m_sharedString->size, replaceString.GetConstBuffer(), replaceString.m_sharedString->size, start, flags);
	}

	unsigned int String::ReplaceAny(const char* oldCharacters, char replaceCharacter, int start, UInt32 flags)
	{
		///FIXME: Ne gère pas l'UTF-8
		if (!oldCharacters || !oldCharacters[0])
			return 0;

		/*if (replaceCharacter == '\0') // Dans ce cas, il faut passer par un algorithme plus complexe
			return ReplaceAny(String(oldCharacters), String(), start);*/

		if (start < 0)
			start = std::max(m_sharedString->size + start, 0U);

		unsigned int pos = static_cast<unsigned int>(start);
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
							unsigned int offset = ptr-m_sharedString->string.get();

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
			while ((ptr = std::strpbrk(ptr, oldCharacters)))
			{
				if (!found)
				{
					unsigned int offset = ptr-m_sharedString->string.get();

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
		unsigned int String::ReplaceAny(const char* oldCharacters, const char* replaceString, int start, UInt32 flags)
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

		unsigned int String::ReplaceAny(const String& oldCharacters, const String& replaceString, int start, UInt32 flags)
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
	void String::Reserve(unsigned int bufferSize)
	{
		if (m_sharedString->capacity > bufferSize)
			return;

		auto newString = std::make_shared<SharedString>(bufferSize);
		newString->size = m_sharedString->size;

		if (m_sharedString->size > 0)
			std::memcpy(newString->string.get(), m_sharedString->string.get(), m_sharedString->size);

		m_sharedString = std::move(newString);
	}

	String& String::Resize(int size, char character)
	{
		if (size == 0)
		{
			Clear(true);
			return *this;
		}

		if (size < 0)
			size = std::max(m_sharedString->size + size, 0U);

		unsigned int newSize = static_cast<unsigned int>(size);

		if (m_sharedString->capacity >= newSize)
		{
			EnsureOwnership();

			// We've got the space required, just fill it up
			if (character != '\0' && newSize > m_sharedString->size)
				std::memset(&m_sharedString->string[m_sharedString->size], character, newSize - m_sharedString->size);

			m_sharedString->size = newSize;
			m_sharedString->string[newSize] = '\0'; // Adds the EoS character
		}
		else // Then we want to make the string bigger
		{
			auto newString = std::make_shared<SharedString>(newSize);
			std::memcpy(newString->string.get(), m_sharedString->string.get(), m_sharedString->size);

			if (character != '\0')
				std::memset(&newString->string[m_sharedString->size], character, newSize - m_sharedString->size);

			m_sharedString = std::move(newString);
		}

		return *this;
	}

	String String::Resized(int size, char character) const
	{
		if (size < 0)
			size = m_sharedString->size + size;

		if (size <= 0)
			return String();

		unsigned int newSize = static_cast<unsigned int>(size);
		if (newSize == m_sharedString->size)
			return *this;

		auto sharedStr = std::make_shared<SharedString>(newSize);
		if (newSize > m_sharedString->size)
		{
			std::memcpy(sharedStr->string.get(), m_sharedString->string.get(), m_sharedString->size);
			if (character != '\0')
				std::memset(&sharedStr->string[m_sharedString->size], character, newSize - m_sharedString->size);
		}
		else
			std::memcpy(sharedStr->string.get(), m_sharedString->string.get(), newSize);

		return String(std::move(sharedStr));
	}

	String& String::Reverse()
	{
		if (m_sharedString->size != 0)
		{
			unsigned int i = 0;
			unsigned int j = m_sharedString->size-1;

			while (i < j)
				std::swap(m_sharedString->string[i++], m_sharedString->string[j--]);
		}

		return *this;
	}

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

	String& String::Set(char character)
	{
		if (character != '\0')
		{
			if (m_sharedString->capacity >= 1)
			{
				EnsureOwnership(true);

				m_sharedString->size = 1;
				m_sharedString->string[0] = character;
				m_sharedString->string[1] = '\0';
			}
			else
			{
				auto newString = std::make_shared<SharedString>(1);
				newString->string[0] = character;
			}
		}
		else
			ReleaseString();

		return *this;
	}

	String& String::Set(unsigned int rep, char character)
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

	String& String::Set(unsigned int rep, const char* string)
	{
		return Set(rep, string, (string) ? std::strlen(string) : 0);
	}

	String& String::Set(unsigned int rep, const char* string, unsigned int length)
	{
		unsigned int totalSize = rep*length;

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

			for (unsigned int i = 0; i < rep; ++i)
				std::memcpy(&m_sharedString->string[i*length], string, length);
		}
		else
			ReleaseString();

		return *this;
	}

	String& String::Set(unsigned int rep, const String& string)
	{
		return Set(rep, string.GetConstBuffer(), string.m_sharedString->size);
	}

	String& String::Set(const char* string)
	{
		return Set(string, (string) ? std::strlen(string) : 0);
	}

	String& String::Set(const char* string, unsigned int length)
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

	String& String::Set(const std::string& string)
	{
		return Set(string.data(), string.size());
	}

	String& String::Set(const String& string)
	{
		m_sharedString = string.m_sharedString;

		return *this;
	}

	String& String::Set(String&& string) noexcept
	{
		std::swap(m_sharedString, string.m_sharedString);

		return *this;
	}

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
				if (Unicode::GetCategory(*it) & Unicode::Category_Separator)
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
				if (std::isspace(*ptr))
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

		return String(std::move(newString));
	}

	String& String::Simplify(UInt32 flags)
	{
		return Set(Simplified(flags));
	}

	unsigned int String::Split(std::vector<String>& result, char separation, int start, UInt32 flags) const
	{
		if (separation == '\0' || m_sharedString->size == 0)
			return 0;

		unsigned int lastSep = Find(separation, start, flags);
		if (lastSep == npos)
		{
			result.push_back(*this);
			return 1;
		}
		else if (lastSep != 0)
			result.push_back(SubString(0, lastSep-1));

		while (true)
		{
			unsigned int sep = Find(separation, lastSep+1, flags);
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

	unsigned int String::Split(std::vector<String>& result, const char* separation, int start, UInt32 flags) const
	{
		return Split(result, separation, std::strlen(separation), start, flags);
	}

	unsigned int String::Split(std::vector<String>& result, const char* separation, unsigned int length, int start, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return 0;
		else if (length == 0)
		{
			result.reserve(m_sharedString->size);
			for (unsigned int i = 0; i < m_sharedString->size; ++i)
				result.push_back(String(m_sharedString->string[i]));

			return m_sharedString->size;
		}
		else if (length > m_sharedString->size)
		{
			result.push_back(*this);
			return 1;
		}

		unsigned int lastSep = Find(separation, start, flags);
		unsigned int oldSize = result.size();
		if (lastSep == npos)
		{
			result.push_back(*this);
			return 1;
		}
		else if (lastSep != 0)
			result.push_back(SubString(0, lastSep-1));

		unsigned int sep;
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

	unsigned int String::Split(std::vector<String>& result, const String& separation, int start, UInt32 flags) const
	{
		return Split(result, separation.m_sharedString->string.get(), separation.m_sharedString->size, start, flags);
	}

	unsigned int String::SplitAny(std::vector<String>& result, const char* separations, int start, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return 0;

		unsigned int oldSize = result.size();

		unsigned int lastSep = FindAny(separations, start, flags);
		if (lastSep == npos)
		{
			result.push_back(*this);
			return 1;
		}
		else if (lastSep != 0)
			result.push_back(SubString(0, lastSep-1));

		unsigned int sep;
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

	unsigned int String::SplitAny(std::vector<String>& result, const String& separations, int start, UInt32 flags) const
	{
		return SplitAny(result, separations.m_sharedString->string.get(), start, flags);
	}

	bool String::StartsWith(char character, UInt32 flags) const
	{
		if (character == '\0' || m_sharedString->size == 0)
			return false;

		if (flags & CaseInsensitive)
			return Detail::ToLower(m_sharedString->string[0]) == Detail::ToLower(character);
		else
			return m_sharedString->string[0] == character;
	}

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

	String String::SubString(int startPos, int endPos) const
	{
		if (startPos < 0)
			startPos = std::max(m_sharedString->size+startPos, 0U);

		unsigned int start = static_cast<unsigned int>(startPos);

		if (endPos < 0)
		{
			endPos = m_sharedString->size+endPos;
			if (endPos < 0)
				return String();
		}

		unsigned int minEnd = std::min(static_cast<unsigned int>(endPos), m_sharedString->size-1);

		if (start > minEnd || start >= m_sharedString->size)
			return String();

		unsigned int size = minEnd-start+1;

		auto str = std::make_shared<SharedString>(size);
		std::memcpy(str->string.get(), &m_sharedString->string[start], size);

		return String(std::move(str));
	}

	String String::SubStringFrom(char character, int startPos, bool fromLast, bool include, UInt32 flags) const
	{
		if (character == '\0')
			return *this;

		unsigned int pos;
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

	String String::SubStringFrom(const char* string, int startPos, bool fromLast, bool include, UInt32 flags) const
	{
		return SubStringFrom(string, std::strlen(string), startPos, fromLast, include, flags);
	}

	String String::SubStringFrom(const char* string, unsigned int length, int startPos, bool fromLast, bool include, UInt32 flags) const
	{
		unsigned int pos;
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

	String String::SubStringFrom(const String& string, int startPos, bool fromLast, bool include, UInt32 flags) const
	{
		return SubStringFrom(string.GetConstBuffer(), string.m_sharedString->size, startPos, fromLast, include, flags);
	}

	String String::SubStringTo(char character, int startPos, bool toLast, bool include, UInt32 flags) const
	{
		if (character == '\0')
			return *this;

		unsigned int pos;
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

	String String::SubStringTo(const char* string, int startPos, bool toLast, bool include, UInt32 flags) const
	{
		return SubStringTo(string, std::strlen(string), startPos, toLast, include, flags);
	}

	String String::SubStringTo(const char* string, unsigned int length, int startPos, bool toLast, bool include, UInt32 flags) const
	{
		unsigned int pos;
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

	String String::SubStringTo(const String& string, int startPos, bool toLast, bool include, UInt32 flags) const
	{
		return SubStringTo(string.GetConstBuffer(), string.m_sharedString->size, startPos, toLast, include, flags);
	}

	void String::Swap(String& str)
	{
		std::swap(m_sharedString, str.m_sharedString);
	}

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
				word = word.ToLower(); // Les mots identifiés sont en ASCII, inutile de passer le flag unicode

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

	bool String::ToDouble(double* value) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (value)
			*value = std::atof(m_sharedString->string.get());

		return true;
	}

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

	String& String::Trim(UInt32 flags)
	{
		return Set(Trimmed(flags));
	}

	String& String::Trim(char character, UInt32 flags)
	{
		return Set(Trimmed(character, flags));
	}

	String String::Trimmed(UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return *this;

		unsigned int startPos;
		unsigned int endPos;
		if (flags & HandleUtf8)
		{
			if ((flags & TrimOnlyRight) == 0)
			{
				utf8::unchecked::iterator<const char*> it(m_sharedString->string.get());
				do
				{
					if (Unicode::GetCategory(*it) & Unicode::Category_Separator)
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
					if (Unicode::GetCategory(*it) & Unicode::Category_Separator)
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
					if (!std::isspace(m_sharedString->string[startPos]))
						break;
				}
			}

			endPos = m_sharedString->size-1;
			if ((flags & TrimOnlyLeft) == 0)
			{
				for (; endPos > 0; --endPos)
				{
					if (!std::isspace(m_sharedString->string[endPos]))
						break;
				}
			}
		}

		return SubString(startPos, endPos);
	}

	String String::Trimmed(char character, UInt32 flags) const
	{
		if (m_sharedString->size == 0)
			return *this;

		unsigned int startPos = 0;
		unsigned int endPos = m_sharedString->size-1;
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

	char* String::begin()
	{
		return m_sharedString->string.get();
	}

	const char* String::begin() const
	{
		return m_sharedString->string.get();
	}

	char* String::end()
	{
		return &m_sharedString->string[m_sharedString->size];
	}

	const char* String::end() const
	{
		return &m_sharedString->string[m_sharedString->size];
	}

	void String::push_front(char c)
	{
		Prepend(c);
	}

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

	String::operator std::string() const
	{
		return std::string(m_sharedString->string.get(), m_sharedString->size);
	}

	char& String::operator[](unsigned int pos)
	{
		EnsureOwnership();

		if (pos >= m_sharedString->size)
			Resize(pos+1);

		return m_sharedString->string[pos];
	}

	char String::operator[](unsigned int pos) const
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

	String& String::operator=(char character)
	{
		return Set(character);
	}

	String& String::operator=(const char* string)
	{
		return Set(string);
	}

	String& String::operator=(const std::string& string)
	{
		return Set(string);
	}

	String& String::operator=(const String& string)
	{
		return Set(string);
	}

	String& String::operator=(String&& string) noexcept
	{
		return Set(string);
	}

	String String::operator+(char character) const
	{
		if (character == '\0')
			return *this;

		auto str = std::make_shared<SharedString>(m_sharedString->size + 1);
		std::memcpy(str->string.get(), GetConstBuffer(), m_sharedString->size);
		str->string[m_sharedString->size] = character;

		return String(std::move(str));
	}

	String String::operator+(const char* string) const
	{
		if (!string || !string[0])
			return *this;

		if (m_sharedString->size == 0)
			return string;

		unsigned int length = std::strlen(string);
		if (length == 0)
			return *this;

		auto str = std::make_shared<SharedString>(m_sharedString->size + length);
		std::memcpy(str->string.get(), GetConstBuffer(), m_sharedString->size);
		std::memcpy(&str->string[m_sharedString->size], string, length+1);

		return String(std::move(str));
	}

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

	String& String::operator+=(char character)
	{
		return Insert(m_sharedString->size, character);
	}

	String& String::operator+=(const char* string)
	{
		return Insert(m_sharedString->size, string);
	}

	String& String::operator+=(const std::string& string)
	{
		return Insert(m_sharedString->size, string.c_str(), string.size());
	}

	String& String::operator+=(const String& string)
	{
		return Insert(m_sharedString->size, string);
	}

	bool String::operator==(char character) const
	{
		if (m_sharedString->size == 0)
			return character == '\0';

		if (m_sharedString->size > 1)
			return false;

		return m_sharedString->string[0] == character;
	}

	bool String::operator==(const char* string) const
	{
		if (m_sharedString->size == 0)
			return !string || !string[0];

		if (!string || !string[0])
			return false;

		return std::strcmp(GetConstBuffer(), string) == 0;
	}

	bool String::operator==(const std::string& string) const
	{
		if (m_sharedString->size == 0 || string.empty())
			return m_sharedString->size == string.size();

		if (m_sharedString->size != string.size())
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) == 0;
	}

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

	bool String::operator!=(const char* string) const
	{
		if (m_sharedString->size == 0)
			return string && string[0];

		if (!string || !string[0])
			return true;

		return std::strcmp(GetConstBuffer(), string) != 0;
	}

	bool String::operator!=(const std::string& string) const
	{
		if (m_sharedString->size == 0 || string.empty())
			return m_sharedString->size == string.size();

		if (m_sharedString->size != string.size())
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) != 0;
	}

	bool String::operator<(char character) const
	{
		if (character == '\0')
			return false;

		if (m_sharedString->size == 0)
			return true;

		return m_sharedString->string[0] < character;
	}

	bool String::operator<(const char* string) const
	{
		if (!string || !string[0])
			return false;

		if (m_sharedString->size == 0)
			return true;

		return std::strcmp(GetConstBuffer(), string) < 0;
	}

	bool String::operator<(const std::string& string) const
	{
		if (string.empty())
			return false;

		if (m_sharedString->size == 0)
			return true;

		return std::strcmp(GetConstBuffer(), string.c_str()) < 0;
	}

	bool String::operator<=(char character) const
	{
		if (m_sharedString->size == 0)
			return true;

		if (character == '\0')
			return false;

		return m_sharedString->string[0] < character || (m_sharedString->string[0] == character && m_sharedString->size == 1);
	}

	bool String::operator<=(const char* string) const
	{
		if (m_sharedString->size == 0)
			return true;

		if (!string || !string[0])
			return false;

		return std::strcmp(GetConstBuffer(), string) <= 0;
	}

	bool String::operator<=(const std::string& string) const
	{
		if (m_sharedString->size == 0)
			return true;

		if (string.empty())
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) <= 0;
	}

	bool String::operator>(char character) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (character == '\0')
			return true;

		return m_sharedString->string[0] > character;
	}

	bool String::operator>(const char* string) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (!string || !string[0])
			return true;

		return std::strcmp(GetConstBuffer(), string) > 0;
	}

	bool String::operator>(const std::string& string) const
	{
		if (m_sharedString->size == 0)
			return false;

		if (string.empty())
			return true;

		return std::strcmp(GetConstBuffer(), string.c_str()) > 0;
	}

	bool String::operator>=(char character) const
	{
		if (character == '\0')
			return true;

		if (m_sharedString->size == 0)
			return false;

		return m_sharedString->string[0] > character || (m_sharedString->string[0] == character && m_sharedString->size == 1);
	}

	bool String::operator>=(const char* string) const
	{
		if (!string || !string[0])
			return true;

		if (m_sharedString->size == 0)
			return false;

		return std::strcmp(GetConstBuffer(), string) >= 0;
	}

	bool String::operator>=(const std::string& string) const
	{
		if (string.empty())
			return true;

		if (m_sharedString->size == 0)
			return false;

		return std::strcmp(GetConstBuffer(), string.c_str()) >= 0;
	}

	String String::Boolean(bool boolean)
	{
		unsigned int size = (boolean) ? 4 : 5;

		auto str = std::make_shared<SharedString>(size);
		std::memcpy(str->string.get(), (boolean) ? "true" : "false", size);

		return String(std::move(str));
	}

	int String::Compare(const String& first, const String& second)
	{
		if (first.m_sharedString->size == 0)
			return (second.m_sharedString->size == 0) ? 0 : -1;

		if (second.m_sharedString->size == 0)
			return 1;

		return std::strcmp(first.GetConstBuffer(), second.GetConstBuffer());
	}

	String String::Number(float number)
	{
		std::ostringstream oss;
		oss.precision(NAZARA_CORE_DECIMAL_DIGITS);
		oss << number;

		return String(oss.str());
	}

	String String::Number(double number)
	{
		std::ostringstream oss;
		oss.precision(NAZARA_CORE_DECIMAL_DIGITS);
		oss << number;

		return String(oss.str());
	}

	String String::Number(long double number)
	{
		std::ostringstream oss;
		oss.precision(NAZARA_CORE_DECIMAL_DIGITS);
		oss << number;

		return String(oss.str());
	}

	String String::Number(signed char number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(unsigned char number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(short number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(unsigned short number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(int number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(unsigned int number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(unsigned long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(long long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Number(unsigned long long number, UInt8 radix)
	{
		return NumberToString(number, radix);
	}

	String String::Pointer(const void* ptr)
	{
		const unsigned int capacity = sizeof(void*)*2 + 2;

		auto str = std::make_shared<SharedString>(capacity);
		str->size = std::sprintf(str->string.get(), "0x%p", ptr);

		return String(std::move(str));
	}

	String String::Unicode(char32_t character)
	{
		if (character == '\0')
			return String();

		unsigned int count = 0;
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

	String String::Unicode(const char* u8String)
	{
		return String(u8String);
	}

	String String::Unicode(const char16_t* u16String)
	{
		if (!u16String || !u16String[0])
			return String();

		const char16_t* ptr = u16String;
		unsigned int count = 0;
		do
			count++;
		while (*++ptr);

		count *= 2; // On s'assure d'avoir la place suffisante

		auto str = std::make_shared<SharedString>(count);

		char* r = utf8::utf16to8(u16String, ptr, str->string.get());
		*r = '\0';

		str->size = r - str->string.get();

		return String(std::move(str));
	}

	String String::Unicode(const char32_t* u32String)
	{
		if (!u32String || !u32String[0])
			return String();

		const char32_t* ptr = u32String;
		unsigned int count = 0;
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

	String String::Unicode(const wchar_t* wString)
	{
		if (!wString || !wString[0])
			return String();

		const wchar_t* ptr = wString;
		unsigned int count = 0;
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

	std::istream& operator>>(std::istream& is, String& str)
	{
		str.Clear();

		char c;
		do
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
		while (true);

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const String& str)
	{
		if (str.IsEmpty())
			return os;

		return operator<<(os, str.m_sharedString->string.get());
	}

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

	String operator+(const char* string, const String& nstring)
	{
		if (!string || !string[0])
			return nstring;

		if (nstring.IsEmpty())
			return string;

		unsigned int size = std::strlen(string);
		unsigned int totalSize = size + nstring.m_sharedString->size;

		auto str = std::make_shared<String::SharedString>(totalSize);
		std::memcpy(str->string.get(), string, size);
		std::memcpy(&str->string[size], nstring.GetConstBuffer(), nstring.m_sharedString->size+1);

		return String(std::move(str));
	}

	String operator+(const std::string& string, const String& nstring)
	{
		if (string.empty())
			return nstring;

		if (nstring.m_sharedString->size == 0)
			return string;

		unsigned int totalSize = string.size() + nstring.m_sharedString->size;

		auto str = std::make_shared<String::SharedString>(totalSize);
		std::memcpy(str->string.get(), string.c_str(), string.size());
		std::memcpy(&str->string[string.size()], nstring.GetConstBuffer(), nstring.m_sharedString->size+1);

		return String(std::move(str));
	}

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

	bool operator!=(const String& first, const String& second)
	{
		return !operator==(first, second);
	}

	bool operator<(const String& first, const String& second)
	{
		if (second.m_sharedString->size == 0)
			return false;

		if (first.m_sharedString->size == 0)
			return true;

		return std::strcmp(first.GetConstBuffer(), second.GetConstBuffer()) < 0;
	}

	bool operator<=(const String& first, const String& second)
	{
		return !operator<(second, first);
	}

	bool operator>(const String& first, const String& second)
	{
		return second < first;
	}

	bool operator>=(const String& first, const String& second)
	{
		return !operator<(first, second);
	}

	bool operator==(char character, const String& nstring)
	{
		return nstring == character;
	}

	bool operator==(const char* string, const String& nstring)
	{
		return nstring == string;
	}

	bool operator==(const std::string& string, const String& nstring)
	{
		return nstring == string;
	}

	bool operator!=(char character, const String& nstring)
	{
		return !operator==(character, nstring);
	}

	bool operator!=(const char* string, const String& nstring)
	{
		return !operator==(string, nstring);
	}

	bool operator!=(const std::string& string, const String& nstring)
	{
		return !operator==(string, nstring);
	}

	bool operator<(char character, const String& nstring)
	{
		return nstring > character;
	}

	bool operator<(const char* string, const String& nstring)
	{
		return nstring > string;
	}

	bool operator<(const std::string& string, const String& nstring)
	{
		return nstring > string;
	}

	bool operator<=(char character, const String& nstring)
	{
		return !operator<(nstring, String(character));
	}

	bool operator<=(const char* string, const String& nstring)
	{
		return !operator<(nstring, string);
	}

	bool operator<=(const std::string& string, const String& nstring)
	{
		return !operator<(nstring, string);
	}

	bool operator>(char character, const String& nstring)
	{
		return nstring < character;
	}

	bool operator>(const char* string, const String& nstring)
	{
		return nstring < string;
	}

	bool operator>(const std::string& string, const String& nstring)
	{
		return nstring < string;
	}

	bool operator>=(char character, const String& nstring)
	{
		return !operator<(character, nstring);
	}

	bool operator>=(const char* string, const String& nstring)
	{
		return !operator<(string, nstring);
	}

	bool operator>=(const std::string& string, const String& nstring)
	{
		return !operator<(string, nstring);
	}

	void String::EnsureOwnership(bool discardContent)
	{
		if (!m_sharedString)
			return;

		if (!m_sharedString.unique())
		{
			auto newSharedString = std::make_shared<SharedString>(GetSize(), GetCapacity());
			if (!discardContent)
				std::memcpy(newSharedString->string.get(), GetConstBuffer(), GetSize()+1);

			m_sharedString = std::move(newSharedString);
		}
	}

	const std::shared_ptr<String::SharedString>& String::GetEmptyString()
	{
		static auto emptyString = std::make_shared<SharedString>();

		return emptyString;
	}

	const unsigned int String::npos(std::numeric_limits<unsigned int>::max());
}

namespace std
{
	istream& getline(istream& is, Nz::String& str)
	{
		str.Clear();

		char c;
		do
		{
			is.get(c);
			if (c != '\n' && c != '\0')
				str += c;
			else
				break;
		}
		while (true);

		return is;
	}

	istream& getline(istream& is, Nz::String& str, char delim)
	{
		str.Clear();

		char c;
		do
		{
			is.get(c);
			if (c != delim && c != '\0')
				str += c;
			else
				break;
		}
		while (true);

		return is;
	}

	void swap(Nz::String& lhs, Nz::String& rhs)
	{
		lhs.Swap(rhs);
	}
}
