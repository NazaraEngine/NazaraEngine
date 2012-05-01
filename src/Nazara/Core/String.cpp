// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#define NAZARA_STRING_CPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HashImpl.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <Utfcpp/utf8.h>
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Core/Debug.hpp>

inline unsigned int nzPow2(unsigned int n)
{
	unsigned int x = 1;

	while(x <= n)
		x <<= 1;

	return x;
}
// Cet algorithme est inspiré de la documentation de Qt
inline unsigned int nzGetNewSize(unsigned int newSize)
{
	if (newSize < 20)
		return newSize+4;
	else
	{
		if (newSize < (1 << 12)-12)
			return nzPow2(newSize << 1)-12;
		else
			return newSize + (1 << 11);
	}
}

inline char nzToLower(char character)
{
	if (character >= 'A' && character <= 'Z')
		return character + ('a' - 'A');
	else
		return character;
}

inline char nzToUpper(char character)
{
	if (character >= 'a' && character <= 'z')
		return character + ('A' - 'a');
	else
		return character;
}

inline int nzStrcasecmp(const char* s1, const char* s2)
{
    int ret = 0;

    while (!(ret = static_cast<unsigned char>(nzToLower(*s1)) - static_cast<unsigned char>(nzToLower(*s2))) && *s2)
        ++s1, ++s2;

    return ret != 0 ? (ret > 0 ? 1 : -1) : 0;
}

inline int nzUnicodecasecmp(const char* s1, const char* s2)
{
    int ret = 0;
	utf8::unchecked::iterator<const char*> it1(s1);
	utf8::unchecked::iterator<const char*> it2(s2);

    while (!(ret = NzUnicode::GetLowercase(*it1) - NzUnicode::GetLowercase(*it2)) && *it2)
        ++it1, ++it2;

    return ret != 0 ? (ret > 0 ? 1 : -1) : 0;
}

NzString::NzString() :
m_sharedString(&emptyString)
{
}

NzString::NzString(char character)
{
	if (character == '\0')
		m_sharedString = &emptyString;
	else
	{
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = 1;
		m_sharedString->size = 1;
		m_sharedString->string = new char[2];
		m_sharedString->string[0] = character;
		m_sharedString->string[1] = '\0';
	}
}

NzString::NzString(const char* string)
{
	if (string)
	{
		unsigned int size = std::strlen(string);
		if (size > 0)
		{
			m_sharedString = new SharedString;
			m_sharedString->allocatedSize = size;
			m_sharedString->size = size;
			m_sharedString->string = new char[size+1];
			std::strcpy(m_sharedString->string, string);
		}
		else
			m_sharedString = &emptyString;
	}
	else
		m_sharedString = &emptyString;
}

NzString::NzString(const std::string& string)
{
	if (string.size() > 0)
	{
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = string.capacity();
		m_sharedString->size = string.size();
		m_sharedString->string = new char[string.capacity()+1];
		std::strcpy(m_sharedString->string, string.c_str());
	}
	else
		m_sharedString = &emptyString;
}

NzString::NzString(const NzString& string) :
m_sharedString(string.m_sharedString)
{
	if (m_sharedString != &emptyString)
	{
		NazaraMutexLock(m_sharedString->mutex);
		m_sharedString->refCount++;
		NazaraMutexUnlock(m_sharedString->mutex);
	}
}

NzString::NzString(NzString&& string) :
m_sharedString(string.m_sharedString)
{
	string.m_sharedString = &emptyString;
}

NzString::NzString(SharedString* sharedString) :
m_sharedString(sharedString)
{
}

NzString::~NzString()
{
	ReleaseString();
}

NzString& NzString::Append(char character)
{
	if (character == '\0')
		return *this;

	if (m_sharedString->size == 0 && m_sharedString->allocatedSize == 0)
		return operator=(character);

	if (m_sharedString->allocatedSize > m_sharedString->size)
	{
		EnsureOwnership();

		m_sharedString->string[m_sharedString->size] = character;
		m_sharedString->string[m_sharedString->size+1] = '\0';
		m_sharedString->size++;
	}
	else
	{
		unsigned int newSize = m_sharedString->size+1;
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		str[m_sharedString->size] = character;
		str[newSize] = '\0';

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

NzString& NzString::Append(const char* string)
{
	if (!string || !string[0])
		return *this;

	unsigned int length = std::strlen(string);
	if (length == 0)
		return *this;

	if (m_sharedString->allocatedSize >= m_sharedString->size + length)
	{
		EnsureOwnership();

		std::strcpy(&m_sharedString->string[m_sharedString->size], string);
		m_sharedString->size += length;
	}
	else
	{
		unsigned int newSize = m_sharedString->size + length;
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		std::strcpy(&str[m_sharedString->size], string);

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

NzString& NzString::Append(const NzString& string)
{
	if (string.m_sharedString->size == 0)
		return *this;

	if (m_sharedString->size == 0 && m_sharedString->allocatedSize < string.m_sharedString->size)
		return operator=(string);

	if (m_sharedString->allocatedSize >= m_sharedString->size + string.m_sharedString->size)
	{
		EnsureOwnership();

		std::strcpy(&m_sharedString->string[m_sharedString->size], string.m_sharedString->string);
		m_sharedString->size += string.m_sharedString->size;
	}
	else
	{
		unsigned int newSize = m_sharedString->size + string.m_sharedString->size;
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		std::strcpy(&str[m_sharedString->size], string.m_sharedString->string);

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

void NzString::Clear()
{
	ReleaseString();
}

bool NzString::Contains(char character, int start, nzUInt32 flags) const
{
	return Find(character, start, flags) != npos;
}

bool NzString::Contains(const char* string, int start, nzUInt32 flags) const
{
	return Find(string, start, flags) != npos;
}

bool NzString::Contains(const NzString& string, int start, nzUInt32 flags) const
{
	return Find(string, start, flags) != npos;
}

unsigned int NzString::Count(char character, int start, nzUInt32 flags) const
{
	if (character == '\0' || m_sharedString->size == 0)
		return 0;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size)
		return 0;

	char* str = &m_sharedString->string[pos];
	unsigned int count = 0;
	if (flags & CaseInsensitive)
	{
		char character_lower = nzToLower(character);
		char character_upper = nzToUpper(character);
		unsigned int count = 0;
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

unsigned int NzString::Count(const char* string, int start, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return 0;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
			char32_t c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
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

						if (NzUnicode::GetLowercase(*it) != NzUnicode::GetLowercase(*it2))
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
			char c = nzToLower(string[0]);
			do
			{
				if (nzToLower(*str) == c)
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

						if (nzToLower(*str) != nzToLower(*ptr))
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

unsigned int NzString::Count(const NzString& string, int start, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
		return 0;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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

			const char* t = string.m_sharedString->string;
			char32_t c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
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

						if (NzUnicode::GetLowercase(*it) != NzUnicode::GetLowercase(*it2))
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
			char c = nzToLower(string.m_sharedString->string[0]);
			do
			{
				if (nzToLower(*str) == c)
				{
					str++;

					const char* ptr = &string.m_sharedString->string[1];
					while (true)
					{
						if (*ptr == '\0')
						{
							count++;
							break;
						}

						if (*str == '\0')
							return count;

						if (nzToLower(*str) != nzToLower(*ptr))
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
		while ((str = std::strstr(str, string.m_sharedString->string)))
		{
			count++;
			str++;
		}
	}

	return count;
}

unsigned int NzString::CountAny(const char* string, int start, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return 0;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
					if (NzUnicode::GetLowercase(*it) == NzUnicode::GetLowercase(*it2))
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
					if (nzToLower(*str) == nzToLower(*c))
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

unsigned int NzString::CountAny(const NzString& string, int start, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || m_sharedString->size == 0)
		return 0;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
				utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
				do
				{
					if (NzUnicode::GetLowercase(*it) == NzUnicode::GetLowercase(*it2))
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
				utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
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
				const char* c = string.m_sharedString->string;
				do
				{
					if (nzToLower(*str) == nzToLower(*c))
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
			while ((str = std::strpbrk(str, string.m_sharedString->string)))
			{
				count++;
				str++;
			}
		}
	}

	return count;
}

bool NzString::EndsWith(char character, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return 0;

	if (flags & CaseInsensitive)
		return nzToLower(m_sharedString->string[m_sharedString->size-1]) == nzToLower(character);
	else
		return m_sharedString->string[m_sharedString->size-1] == character; // character == '\0' sera toujours faux
}

bool NzString::EndsWith(const char* string, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return false;

	unsigned int len = std::strlen(string);
	if (len > m_sharedString->size)
		return false;

	if (flags & CaseInsensitive)
	{
		if (flags & HandleUtf8)
			return nzUnicodecasecmp(&m_sharedString->string[m_sharedString->size - len], string) == 0;
		else
			return nzStrcasecmp(&m_sharedString->string[m_sharedString->size - len], string) == 0;
	}
	else
		return std::strcmp(&m_sharedString->string[m_sharedString->size - len], string) == 0;
}

bool NzString::EndsWith(const NzString& string, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
		return false;

	if (flags & CaseInsensitive)
	{
		if (flags & HandleUtf8)
			return nzUnicodecasecmp(&m_sharedString->string[m_sharedString->size - string.m_sharedString->size], string.m_sharedString->string) == 0;
		else
			return nzStrcasecmp(&m_sharedString->string[m_sharedString->size - string.m_sharedString->size], string.m_sharedString->string) == 0;
	}
	else
		return std::strcmp(&m_sharedString->string[m_sharedString->size - string.m_sharedString->size], string.m_sharedString->string) == 0;
}

unsigned int NzString::Find(char character, int start, nzUInt32 flags) const
{
	if (character == '\0' || m_sharedString->size == 0)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size)
		return npos;

	if (flags & CaseInsensitive)
	{
		char ch = nzToLower(character);
		const char* str = m_sharedString->string;
		do
		{
			if (nzToLower(*str) == ch)
				return static_cast<unsigned int>(str - m_sharedString->string);
		}
		while (*++str);

		return npos;
	}
	else
	{
		char* ch = std::strchr(&m_sharedString->string[pos], character);
		if (ch)
			return static_cast<unsigned int>(ch - m_sharedString->string);
		else
			return npos;
	}
}

unsigned int NzString::Find(const char* string, int start, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
			char32_t c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
				{
					const char* pos = it.base();
					++it;

					utf8::unchecked::iterator<const char*> it2(t);
					while (true)
					{
						if (*it2 == '\0')
							return static_cast<unsigned int>(pos - m_sharedString->string);

						if (*it == '\0')
							return npos;

						if (NzUnicode::GetLowercase(*it) != NzUnicode::GetLowercase(*it2))
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
			char c = nzToLower(string[0]);
			do
			{
				if (nzToLower(*str) == c)
				{
					char* pos = str;
					str++;

					const char* ptr = &string[1];
					while (true)
					{
						if (*ptr == '\0')
							return static_cast<unsigned int>(pos - m_sharedString->string);

						if (*str == '\0')
							return npos;

						if (nzToLower(*str) != nzToLower(*ptr))
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
			return static_cast<unsigned int>(ch - m_sharedString->string);
	}

	return npos;
}

unsigned int NzString::Find(const NzString& string, int start, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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

			const char* t = string.m_sharedString->string;
			char32_t c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
				{
					const char* pos = it.base();
					++it;

					utf8::unchecked::iterator<const char*> it2(t);
					while (true)
					{
						if (*it2 == '\0')
							return static_cast<unsigned int>(pos - m_sharedString->string);

						if (*it == '\0')
							return npos;

						if (NzUnicode::GetLowercase(*it) != NzUnicode::GetLowercase(*it2))
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
			char c = nzToLower(string.m_sharedString->string[0]);
			do
			{
				if (nzToLower(*str) == c)
				{
					char* pos = str;
					str++;

					const char* ptr = &string.m_sharedString->string[1];
					while (true)
					{
						if (*ptr == '\0')
							return static_cast<unsigned int>(pos - m_sharedString->string);

						if (*str == '\0')
							return npos;

						if (nzToLower(*str) != nzToLower(*ptr))
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
		char* ch = std::strstr(&m_sharedString->string[pos], string.m_sharedString->string);
		if (ch)
			return static_cast<unsigned int>(ch - m_sharedString->string);
	}

	return npos;
}

unsigned int NzString::FindAny(const char* string, int start, nzUInt32 flags) const
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
				char32_t character = NzUnicode::GetLowercase(*it);
				do
				{
					if (character == NzUnicode::GetLowercase(*it2))
						return it.base() - m_sharedString->string;
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
						return it.base() - m_sharedString->string;
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
				char character = nzToLower(*str);
				do
				{
					if (character == nzToLower(*c))
						return str - m_sharedString->string;
				}
				while (*++c);
			}
			while (*++str);
		}
		else
		{
			str = std::strpbrk(str, string);
			if (str)
				return str - m_sharedString->string;
		}
	}

	return npos;
}

unsigned int NzString::FindAny(const NzString& string, int start, nzUInt32 flags) const
{
	if (m_sharedString->size == 0 || string.m_sharedString->size == 0)
		return npos;

	if (string.m_sharedString->size > m_sharedString->size)
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
				utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
				char32_t character = NzUnicode::GetLowercase(*it);
				do
				{
					if (character == NzUnicode::GetLowercase(*it2))
						return it.base() - m_sharedString->string;
				}
				while (*++it2);
			}
			while (*++it);
		}
		else
		{
			do
			{
				utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
				do
				{
					if (*it == *it2)
						return it.base() - m_sharedString->string;
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
				const char* c = string.m_sharedString->string;
				char character = nzToLower(*str);
				do
				{
					if (character == nzToLower(*c))
						return str - m_sharedString->string;
				}
				while (*++c);
			}
			while (*++str);
		}
		else
		{
			str = std::strpbrk(str, string.m_sharedString->string);
			if (str)
				return str - m_sharedString->string;
		}
	}

	return npos;
}

unsigned int NzString::FindLast(char character, int start, nzUInt32 flags) const
{
	if (character == '\0' || m_sharedString->size == 0)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size)
		return npos;

	char* ptr = &m_sharedString->string[m_sharedString->size-1];

	if (flags & CaseInsensitive)
	{
		character = nzToLower(character);
		do
		{
			if (nzToLower(*ptr) == character)
				return static_cast<unsigned int>(ptr - m_sharedString->string);
		}
		while (ptr-- != m_sharedString->string);
	}
	else
	{
		// strrchr ?
		do
		{
			if (*ptr == character)
				return static_cast<unsigned int>(ptr - m_sharedString->string);
		}
		while (ptr-- != m_sharedString->string);
	}

	return npos;
}

unsigned int NzString::FindLast(const char* string, int start, nzUInt32 flags) const
{
	if (!string || !string[0] ||m_sharedString->size == 0)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
			char32_t c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
				{
					utf8::unchecked::iterator<const char*> it2(t);
					utf8::unchecked::iterator<const char*> tIt(it);
					++tIt;

					while (true)
					{
						if (*it2 == '\0')
							return it.base() - m_sharedString->string;

						if (tIt.base() > &m_sharedString->string[pos])
							break;

						if (NzUnicode::GetLowercase(*tIt) != NzUnicode::GetLowercase(*it2))
							break;

						++it2;
						++tIt;
					}
				}
			}
			while (it--.base() != m_sharedString->string);
		}
		else
		{
			char c = nzToLower(string[0]);
			do
			{
				if (nzToLower(*ptr) == c)
				{
					const char* p = &string[1];
					const char* tPtr = ptr+1;
					while (true)
					{
						if (*p == '\0')
							return ptr - m_sharedString->string;

						if (tPtr > &m_sharedString->string[pos])
							break;

						if (nzToLower(*tPtr) != nzToLower(*p))
							break;

						p++;
						tPtr++;
					}
				}
			}
			while (ptr-- != m_sharedString->string);
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
						return ptr - m_sharedString->string;

					if (tPtr > &m_sharedString->string[pos])
						break;

					if (*tPtr != *p)
						break;

					p++;
					tPtr++;
				}
			}
		}
		while (ptr-- != m_sharedString->string);
	}

	return npos;
}

unsigned int NzString::FindLast(const NzString& string, int start, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
			const char* t = string.m_sharedString->string;
			char32_t c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
				{
					utf8::unchecked::iterator<const char*> it2(t);
					utf8::unchecked::iterator<const char*> tIt(it);
					++tIt;

					while (true)
					{
						if (*it2 == '\0')
							return it.base() - m_sharedString->string;

						if (tIt.base() > &m_sharedString->string[pos])
							break;

						if (NzUnicode::GetLowercase(*tIt) != NzUnicode::GetLowercase(*it2))
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
			char c = nzToLower(string.m_sharedString->string[string.m_sharedString->size-1]);
			while (true)
			{
				if (nzToLower(*ptr) == c)
				{
					const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
					for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
					{
						if (nzToLower(*ptr) != nzToLower(*p))
							break;

						if (p == &string.m_sharedString->string[0])
							return ptr-m_sharedString->string;

						if (ptr == m_sharedString->string)
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
						return ptr-m_sharedString->string;

					if (ptr == m_sharedString->string)
						return npos;
				}
			}
			else if (ptr-- <= limit)
				break;
		}
	}

	return npos;
}

unsigned int NzString::FindLastAny(const char* string, int start, nzUInt32 flags) const
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
				char32_t character = NzUnicode::GetLowercase(*it);
				do
				{
					if (character == NzUnicode::GetLowercase(*it2))
						return it.base() - m_sharedString->string;
				}
				while (*++it2);
			}
			while (it--.base() != m_sharedString->string);
		}
		else
		{
			do
			{
				utf8::unchecked::iterator<const char*> it2(string);
				do
				{
					if (*it == *it2)
						return it.base() - m_sharedString->string;
				}
				while (*++it2);
			}
			while (it--.base() != m_sharedString->string);
		}
	}
	else
	{
		if (flags & CaseInsensitive)
		{
			do
			{
				const char* c = string;
				char character = nzToLower(*str);
				do
				{
					if (character == nzToLower(*c))
						return str - m_sharedString->string;
				}
				while (*++c);
			}
			while (str-- != m_sharedString->string);
		}
		else
		{
			do
			{
				const char* c = string;
				do
				{
					if (*str == *c)
						return str - m_sharedString->string;
				}
				while (*++c);
			}
			while (str-- != m_sharedString->string);
		}
	}

	return npos;
}

unsigned int NzString::FindLastAny(const NzString& string, int start, nzUInt32 flags) const
{
	if (m_sharedString->size == 0 || string.m_sharedString->size == 0)
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
				utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
				char32_t character = NzUnicode::GetLowercase(*it);
				do
				{
					if (character == NzUnicode::GetLowercase(*it2))
						return it.base() - m_sharedString->string;
				}
				while (*++it2);
			}
			while (it--.base() != m_sharedString->string);
		}
		else
		{
			do
			{
				utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
				do
				{
					if (*it == *it2)
						return it.base() - m_sharedString->string;
				}
				while (*++it2);
			}
			while (it--.base() != m_sharedString->string);
		}
	}
	else
	{
		if (flags & CaseInsensitive)
		{
			do
			{
				const char* c = string.m_sharedString->string;
				char character = nzToLower(*str);
				do
				{
					if (character == nzToLower(*c))
						return str - m_sharedString->string;
				}
				while (*++c);
			}
			while (str-- != m_sharedString->string);
		}
		else
		{
			do
			{
				const char* c = string.m_sharedString->string;
				do
				{
					if (*str == *c)
						return str - m_sharedString->string;
				}
				while (*++c);
			}
			while (str-- != m_sharedString->string);
		}
	}

	return npos;
}

unsigned int NzString::FindLastWord(const char* string, int start, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
			nzUInt32 c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*tIt) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
							else
								break;
						}

						if (tIt.base() > &m_sharedString->string[pos])
							break;

						if (NzUnicode::GetLowercase(*tIt) != NzUnicode::GetLowercase(*p))
							break;

						++p;
						++tIt;
					}
				}
			}
			while (it--.base() != m_sharedString->string);
		}
		else
		{
			const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = utf8::unchecked::next(t);
			do
			{
				if (*it == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*tIt) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
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
			while (it--.base() != m_sharedString->string);
		}
	}
	else
	{
		if (flags & CaseInsensitive)
		{
			char c = nzToLower(string[0]);
			do
			{
				if (nzToLower(*ptr) == c)
				{
					if (ptr != m_sharedString->string && !std::isspace(*(ptr-1)))
						continue;

					const char* p = &string[1];
					const char* tPtr = ptr+1;
					while (true)
					{
						if (*p == '\0')
						{
							if (*tPtr == '\0' || std::isspace(*tPtr))
								return ptr-m_sharedString->string;
							else
								break;
						}

						if (tPtr > &m_sharedString->string[pos])
							break;

						if (nzToLower(*tPtr) != nzToLower(*p))
							break;

						p++;
						tPtr++;
					}
				}
			}
			while (ptr-- != m_sharedString->string);
		}
		else
		{
			do
			{
				if (*ptr == string[0])
				{
					if (ptr != m_sharedString->string && !std::isspace(*(ptr-1)))
						continue;

					const char* p = &string[1];
					const char* tPtr = ptr+1;
					while (true)
					{
						if (*p == '\0')
						{
							if (*tPtr == '\0' || std::isspace(*tPtr))
								return ptr-m_sharedString->string;
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
			while (ptr-- != m_sharedString->string);
		}
	}

	return npos;
}

unsigned int NzString::FindLastWord(const NzString& string, int start, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

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
			const char* t = string.m_sharedString->string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = NzUnicode::GetLowercase(utf8::unchecked::next(t));
			do
			{
				if (NzUnicode::GetLowercase(*it) == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*tIt) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
							else
								break;
						}

						if (tIt.base() > &m_sharedString->string[pos])
							break;

						if (NzUnicode::GetLowercase(*tIt) != NzUnicode::GetLowercase(*p))
							break;

						++p;
						++tIt;
					}
				}
			}
			while (it--.base() != m_sharedString->string);
		}
		else
		{
			const char* t = string.m_sharedString->string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = utf8::unchecked::next(t);
			do
			{
				if (*it == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*tIt) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
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
			while (it--.base() != m_sharedString->string);
		}
	}
	else
	{
		///Algo 2.FindLastWord#2 (Taille du pattern connue)
		if (flags & CaseInsensitive)
		{
			char c = nzToLower(string.m_sharedString->string[string.m_sharedString->size-1]);
			do
			{
				if (nzToLower(*ptr) == c)
				{
					if (*(ptr+1) != '\0' && !std::isspace(*(ptr+1)))
						continue;

					const char* p = &string.m_sharedString->string[string.m_sharedString->size-1];
					for (; p >= &string.m_sharedString->string[0]; --p, --ptr)
					{
						if (nzToLower(*ptr) != nzToLower(*p))
							break;

						if (p == &string.m_sharedString->string[0])
						{
							if (ptr == m_sharedString->string || std::isspace(*(ptr-1)))
								return ptr-m_sharedString->string;
							else
								break;
						}

						if (ptr == m_sharedString->string)
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
							if (ptr == m_sharedString->string || std::isspace(*(ptr-1)))
								return ptr-m_sharedString->string;
							else
								break;
						}

						if (ptr == m_sharedString->string)
							return npos;
					}
				}
			}
			while (ptr-- > limit);
		}
	}

	return npos;
}

unsigned int NzString::FindWord(const char* string, int start, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size)
		return npos;

	///Algo 3.FindWord#3 (Taille du pattern inconnue)
	const char* ptr = m_sharedString->string;
	if (flags & HandleUtf8)
	{
		if (utf8::internal::is_trail(*ptr))
			utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

		utf8::unchecked::iterator<const char*> it(ptr);

		if (flags & CaseInsensitive)
		{
			const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = NzUnicode::GetLowercase(utf8::unchecked::next(t));

			do
			{
				if (*it == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
							else
								break;
						}

						if (NzUnicode::GetLowercase(*tIt) != NzUnicode::GetLowercase(*p))
							break;

						p++;
						tIt++;
					}
				}
			}
			while (*++ptr);
		}
		else
		{
			const char* t = string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = NzUnicode::GetLowercase(utf8::unchecked::next(t));

			do
			{
				if (*it == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
							else
								break;
						}

						if (*tIt != *p)
							break;

						p++;
						tIt++;
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
			char c = nzToLower(string[0]);
			do
			{
				if (nzToLower(*ptr) == c)
				{
					if (ptr != m_sharedString->string && !std::isspace(*(ptr-1)))
						continue;

					const char* p = &string[1];
					const char* tPtr = ptr+1;
					while (true)
					{
						if (*p == '\0')
						{
							if (*tPtr == '\0' || std::isspace(*tPtr))
								return ptr - m_sharedString->string;
							else
								break;
						}

						if (nzToLower(*tPtr) != nzToLower(*p))
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
					if (ptr != m_sharedString->string && !std::isspace(*(ptr-1)))
						continue;

					const char* p = &string[1];
					const char* tPtr = ptr+1;
					while (true)
					{
						if (*p == '\0')
						{
							if (*tPtr == '\0' || std::isspace(*tPtr))
								return ptr - m_sharedString->string;
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

unsigned int NzString::FindWord(const NzString& string, int start, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0 || string.m_sharedString->size > m_sharedString->size)
		return npos;

	if (start < 0)
		start = std::max(static_cast<int>(m_sharedString->size + start), 0);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size)
		return npos;

	char* ptr = m_sharedString->string;
	if (flags & HandleUtf8)
	{
		///Algo 3.FindWord#3 (Itérateur trop lent pour #2)
		if (utf8::internal::is_trail(*ptr))
			utf8::unchecked::prior(ptr); // On s'assure d'avoir un pointeur vers le début d'un caractère

		utf8::unchecked::iterator<const char*> it(ptr);

		if (flags & CaseInsensitive)
		{
			const char* t = string.m_sharedString->string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = NzUnicode::GetLowercase(utf8::unchecked::next(t));

			do
			{
				if (*it == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
							else
								break;
						}

						if (NzUnicode::GetLowercase(*tIt) != NzUnicode::GetLowercase(*p))
							break;

						p++;
						tIt++;
					}
				}
			}
			while (*++ptr);
		}
		else
		{
			const char* t = string.m_sharedString->string; // utf8(::unchecked)::next affecte l'itérateur en argument
			nzUInt32 c = NzUnicode::GetLowercase(utf8::unchecked::next(t));

			do
			{
				if (*it == c)
				{
					if (it.base() != m_sharedString->string)
					{
						it--;
						if (!(NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator))
							continue;
					}

					utf8::unchecked::iterator<const char*> p(t);
					utf8::unchecked::iterator<const char*> tIt = it;
					++tIt;

					while (true)
					{
						if (*p == '\0')
						{
							if (*tIt == '\0' || NzUnicode::GetCategory(*it++) & NzUnicode::Category_Separator)
								return it.base() - m_sharedString->string;
							else
								break;
						}

						if (*tIt != *p)
							break;

						p++;
						tIt++;
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
			char c = nzToLower(string.m_sharedString->string[0]);
			do
			{
				if (nzToLower(*ptr) == c)
				{
					if (ptr != m_sharedString->string && !std::isspace(*(ptr-1)))
						continue;

					const char* p = &string.m_sharedString->string[1];
					const char* tPtr = ptr+1;
					while (true)
					{
						if (*p == '\0')
						{
							if (*tPtr == '\0' || std::isspace(*tPtr))
								return ptr - m_sharedString->string;
							else
								break;
						}

						if (nzToLower(*tPtr) != nzToLower(*p))
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
			while ((ptr = std::strstr(ptr, string.m_sharedString->string)))
			{
				// Si le mot est bien isolé
				if ((ptr == m_sharedString->string || std::isspace(*(ptr-1))) && (*(ptr+m_sharedString->size) == '\0' || std::isspace(*(ptr+m_sharedString->size))))
					return ptr - m_sharedString->string;

				ptr++;
			}
		}
	}

	return npos;
}

char* NzString::GetBuffer()
{
	EnsureOwnership();

	return m_sharedString->string;
}

unsigned int NzString::GetCapacity() const
{
	return m_sharedString->allocatedSize;
}

const char* NzString::GetConstBuffer() const
{
	return m_sharedString->string;
}

unsigned int NzString::GetLength() const
{
	#if NAZARA_CORE_SAFE
	try
	{
	#endif
	return utf8::distance(m_sharedString->string, &m_sharedString->string[m_sharedString->size]);
	#if NAZARA_CORE_SAFE
	}
	catch (const utf8::exception& exception)
	{
		NazaraError("UTF-8 error : " + NzString(exception.what()));
		return 0;
	}
	#endif
}

unsigned int NzString::GetSize() const
{
	return m_sharedString->size;
}

char* NzString::GetUtf8Buffer(unsigned int* size) const
{
	if (m_sharedString->size == 0)
		return nullptr;

	char* buffer = new char[m_sharedString->size+1];
	std::strcpy(buffer, m_sharedString->string);

	if (size)
		*size = m_sharedString->size;

	return buffer;
}

char16_t* NzString::GetUtf16Buffer(unsigned int* size) const
{
	if (m_sharedString->size == 0)
		return nullptr;

	std::vector<char16_t> utf16;
	utf16.reserve(m_sharedString->size);
	#if NAZARA_CORE_SAFE
	try
	{
	#endif
	utf8::utf8to16(m_sharedString->string, &m_sharedString->string[m_sharedString->size], std::back_inserter(utf16));
	#if NAZARA_CORE_SAFE
	}
	catch (const utf8::exception& exception)
	{
		NazaraError("UTF-8 error : " + NzString(exception.what()));
		return 0;
	}
	#endif

	unsigned int bufferSize = utf16.size();
	if (bufferSize == 0)
		return nullptr;

	char16_t* buffer = new char16_t[bufferSize+1];
	std::memcpy(buffer, &utf16[0], bufferSize*sizeof(char16_t));
	buffer[bufferSize] ='\0';

	if (size)
		*size = bufferSize;

	return buffer;
}

char32_t* NzString::GetUtf32Buffer(unsigned int* size) const
{
	if (m_sharedString->size == 0)
		return nullptr;

	#if NAZARA_CORE_SAFE
	try
	{
	#endif
	unsigned int bufferSize = utf8::distance(m_sharedString->string, &m_sharedString->string[m_sharedString->size]);
	if (bufferSize == 0)
		return nullptr;

	char32_t* buffer = new char32_t[bufferSize+1];
	utf8::utf8to32(m_sharedString->string, &m_sharedString->string[m_sharedString->size], buffer);
	buffer[bufferSize] ='\0';

	if (size)
		*size = bufferSize;

	return buffer;
	#if NAZARA_CORE_SAFE
	}
	catch (const utf8::exception& exception)
	{
		NazaraError("UTF-8 error : " + NzString(exception.what()));
		return 0;
	}
	#endif
}

wchar_t* NzString::GetWideBuffer(unsigned int* size) const
{
	static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "wchar_t size is not supported");
	if (m_sharedString->size == 0)
		return nullptr;

	#if NAZARA_CORE_SAFE
	try
	{
	#endif
	unsigned int bufferSize = utf8::distance(m_sharedString->string, &m_sharedString->string[m_sharedString->size]);
	if (bufferSize == 0)
		return nullptr;

	wchar_t* buffer = new wchar_t[bufferSize+1];
	if (sizeof(wchar_t) == 4)
		utf8::utf8to32(m_sharedString->string, &m_sharedString->string[m_sharedString->size], buffer);
	else
	{
		wchar_t* ptr = buffer;

		utf8::unchecked::iterator<const char*> it(m_sharedString->string);
		do
		{
			char32_t cp = *it;
			if (cp <= 0xFFFF && (cp < 0xD800 || cp > 0xDFFF)) // @Laurent Gomila
				*ptr++ = static_cast<wchar_t>(cp);
			else
				*ptr++ = static_cast<wchar_t>('?');
		}
		while (*it++);
	}

	if (size)
		*size = bufferSize;

	return buffer;
	#if NAZARA_CORE_SAFE
	}
	catch (const utf8::exception& exception)
	{
		NazaraError("UTF-8 error : " + NzString(exception.what()));
		return 0;
	}
	#endif
}

NzString NzString::GetWord(unsigned int index, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return NzString();

	NzString temp = Simplified(flags); // Évitons les mauvaises surprises
	if (temp.IsEmpty())
		return NzString();

	unsigned int foundPos = temp.Find(' '); // Simplified nous assure que nous n'avons plus que des espaces comme séparation
	unsigned int lastPos = 0;
	for (; index > 0; --index)
	{
		if (foundPos == npos)
			return NzString();

		lastPos = foundPos;
		foundPos = temp.Find(' ', foundPos+1);
	}
	return temp.Substr((lastPos == 0) ? 0 : lastPos+1, (foundPos == npos) ? -1 : static_cast<int>(foundPos-1));
}

unsigned int NzString::GetWordPosition(unsigned int index, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return npos;

	unsigned int currentWord = 0;
	bool inWord = false;

	const char* ptr = m_sharedString->string;
	if (flags & HandleUtf8)
	{
		utf8::unchecked::iterator<const char*> it(ptr);
		do
		{
			if (NzUnicode::GetCategory(*it) & NzUnicode::Category_Separator)
				inWord = false;
			else
			{
				if (!inWord)
				{
					inWord = true;
					if (++currentWord > index)
						return static_cast<unsigned int>(it.base() - m_sharedString->string);
				}
			}
		}
		while (*++it);
	}
	else
	{
		while (ptr != &m_sharedString->string[m_sharedString->size])
		{
			if (!std::isspace(*ptr++))
			{
				if (!inWord)
				{
					inWord = true;
					if (++currentWord > index)
						return ptr-m_sharedString->string;
				}
			}
			else
				inWord = false;
		}
	}

	return npos;
}

NzString& NzString::Insert(int pos, char character)
{
	if (character == '\0')
		return *this;

	if (m_sharedString->size == 0 && m_sharedString->allocatedSize == 0)
		return operator=(character);

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedString->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedString->size);

	// Si le buffer est déjà suffisamment grand
	if (m_sharedString->allocatedSize >= m_sharedString->size+1)
	{
		EnsureOwnership();

		std::memmove(&m_sharedString->string[start+1], &m_sharedString->string[start], m_sharedString->size*sizeof(char));
		m_sharedString->string[start] = character;

		m_sharedString->size += 1;
	}
	else
	{
		unsigned int newSize = m_sharedString->size+1;
		char* newString = new char[newSize+1];

		char* ptr = newString;
		const char* s = m_sharedString->string;
		while (ptr != &newString[start])
			*ptr++ = *s++;

		*ptr++ = character;

		std::strcpy(ptr, s);

		ReleaseString();

		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = newSize;
		m_sharedString->size = newSize;
		m_sharedString->string = newString;
	}

	return *this;
}

NzString& NzString::Insert(int pos, const char* string)
{
	if (!string || !string[0])
		return *this;

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedString->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedString->size);

	// Si le buffer est déjà suffisamment grand
	unsigned int len = std::strlen(string);
	if (m_sharedString->allocatedSize >= m_sharedString->size+len)
	{
		EnsureOwnership();

		std::memmove(&m_sharedString->string[start+len], &m_sharedString->string[start], m_sharedString->size*sizeof(char));
		std::memcpy(&m_sharedString->string[start], string, len*sizeof(char));

		m_sharedString->size += len;
	}
	else
	{
		unsigned int newSize = m_sharedString->size+len;
		char* newString = new char[newSize+1];

		char* ptr = newString;
		const char* s = m_sharedString->string;

		while (ptr != &newString[start])
			*ptr++ = *s++;

		const char* p = string;
		while (ptr != &newString[start+len])
			*ptr++ = *p++;

		std::strcpy(ptr, s);

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = newSize;
		m_sharedString->size = newSize;
		m_sharedString->string = newString;
	}

	return *this;
}

NzString& NzString::Insert(int pos, const NzString& string)
{
	if (string.m_sharedString->size == 0)
		return *this;

	if (m_sharedString->size == 0 && m_sharedString->allocatedSize < string.m_sharedString->size)
		return operator=(string);

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedString->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedString->size);

	// Si le buffer est déjà suffisamment grand
	if (m_sharedString->allocatedSize >= m_sharedString->size + string.m_sharedString->size)
	{
		EnsureOwnership();

		std::memmove(&m_sharedString->string[start+string.m_sharedString->size], &m_sharedString->string[start], m_sharedString->size*sizeof(char));
		std::memcpy(&m_sharedString->string[start], string.m_sharedString->string, string.m_sharedString->size*sizeof(char));

		m_sharedString->size += string.m_sharedString->size;
	}
	else
	{
		unsigned int newSize = m_sharedString->size+string.m_sharedString->size;
		char* newString = new char[newSize+1];

		char* ptr = newString;
		const char* s = m_sharedString->string;

		while (ptr != &newString[start])
			*ptr++ = *s++;

		const char* p = string.m_sharedString->string;
		while (ptr != &newString[start+string.m_sharedString->size])
			*ptr++ = *p++;

		std::strcpy(ptr, s);

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = newSize;
		m_sharedString->size = newSize;
		m_sharedString->string = newString;
	}

	return *this;
}

bool NzString::IsEmpty() const
{
	return m_sharedString->size == 0;
}

bool NzString::IsNull() const
{
	return m_sharedString == &emptyString;
}

bool NzString::IsNumber(nzUInt8 base, nzUInt32 flags) const
{
	#if !NAZARA_UNSAFE
	if (base < 2 || base > 36)
	{
		NazaraError("Base must be between 2 and 36");
		return false;
	}
	#endif

	if (m_sharedString->size == 0)
		return false;

	NzString check = Simplified();
	if (check.m_sharedString->size)
		return false;

	char* ptr = (check.m_sharedString->string[0] == '-') ? &check.m_sharedString->string[1] : check.m_sharedString->string;

	if (base > 10)
	{
		if (flags & CaseInsensitive)
		{
			do
			{
				char c = *ptr;
				if (c != ' ' && (c < '0' || (c > '9' && c < 'A') || (c > 'A'+base-1 && c < 'a') || c > 'a'+base-1))
					return false;
			}
			while (*++ptr);
		}
		else
		{
			do
			{
				char c = *ptr;
				if (c != ' ' && (c < '0' || (c > '9' && c < 'a') || c > 'a'+base-1))
					return false;
			}
			while (*++ptr);
		}
	}
	else
	{
		do
		{
			char c = *ptr;
			if (c != ' ' && (c < '0' || c > '0'+base-1))
				return false;
		}
		while (*++ptr);
	}

	return true;
}

bool NzString::Match(const char* pattern) const
{
	if (m_sharedString->size == 0 || !pattern)
		return false;

	// Par Jack Handy - akkhandy@hotmail.com
	// From : http://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
	const char* str = m_sharedString->string;
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

bool NzString::Match(const NzString& pattern) const
{
	return Match(pattern.m_sharedString->string);
}

NzString& NzString::Prepend(char character)
{
	return Insert(0, character);
}

NzString& NzString::Prepend(const char* string)
{
	return Insert(0, string);
}

NzString& NzString::Prepend(const NzString& string)
{
	return Insert(0, string);
}

unsigned int NzString::Replace(char oldCharacter, char newCharacter, int start, nzUInt32 flags)
{
	if (oldCharacter == '\0' || oldCharacter == newCharacter)
		return 0;

	if (newCharacter == '\0') // Dans ce cas, il faut passer par un algorithme plus complexe
		return Replace(NzString(oldCharacter), NzString(), start);

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
		char character_lower = nzToLower(oldCharacter);
		char character_upper = nzToUpper(oldCharacter);
		do
		{
			if (*ptr == character_lower || *ptr == character_upper)
			{
				if (!found)
				{
					unsigned int pos = ptr-m_sharedString->string;

					EnsureOwnership();

					ptr = &m_sharedString->string[pos];
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
				unsigned int pos = ptr-m_sharedString->string;

				EnsureOwnership();

				ptr = &m_sharedString->string[pos];
				found = true;
			}

			*ptr = newCharacter;
			++count;
		}
	}

	return count;
}

unsigned int NzString::Replace(const char* oldString, const char* replaceString, int start, nzUInt32 flags)
{
	if (!oldString || !oldString[0])
		return 0;

	if (start < 0)
		start = std::max(m_sharedString->size + start, 0U);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size)
		return 0;

	unsigned int oSize = std::strlen(oldString);
	if (oSize == 0)
		return 0;

	unsigned int rSize = (replaceString) ? std::strlen(replaceString) : 0;

	unsigned int count = 0;
	if (oSize == rSize)
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

			std::memcpy(&m_sharedString->string[pos], replaceString, oSize*sizeof(char));
			pos += oSize;

			++count;
		}
	}
	else ///TODO: Algorithme de remplacement sans changement de buffer (si rSize < oSize)
	{
		unsigned int newSize = m_sharedString->size + Count(oldString)*(rSize - oSize);
		if (newSize == m_sharedString->size) // Count(oldString) == 0
			return 0;

		char* newString = new char[newSize+1];

		///Algo 4.Replace#2
		char* ptr = newString;
		const char* p = m_sharedString->string;

		while ((pos = Find(oldString, pos, flags)) != npos)
		{
			const char* r = &m_sharedString->string[pos];

			std::memcpy(ptr, p, (r-p)*sizeof(char));
			ptr += r-p;
			std::memcpy(ptr, replaceString, rSize*sizeof(char));
			ptr += rSize;
			p = r+oSize;
			pos += oSize;

			count++;
		}

		std::strcpy(ptr, p); // Ajoute le caractère de fin par la même occasion

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = newSize;
		m_sharedString->size = newSize;
		m_sharedString->string = newString;
	}

	return count;
}

unsigned int NzString::Replace(const NzString& oldString, const NzString& replaceString, int start, nzUInt32 flags)
{
	if (oldString.m_sharedString->size == 0)
		return 0;

	if (start < 0)
		start = std::max(m_sharedString->size + start, 0U);

	unsigned int pos = static_cast<unsigned int>(start);
	if (pos >= m_sharedString->size || oldString.m_sharedString->size == 0)
		return 0;

	unsigned int count = 0;
	if (oldString.m_sharedString->size == replaceString.m_sharedString->size)
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

			std::memcpy(&m_sharedString->string[pos], replaceString.m_sharedString->string, oldString.m_sharedString->size*sizeof(char));
			pos += oldString.m_sharedString->size;

			++count;
		}
	}
	else
	{
		unsigned int newSize = m_sharedString->size + Count(oldString)*(replaceString.m_sharedString->size - oldString.m_sharedString->size);
		if (newSize == m_sharedString->size) // Count(oldString) == 0
			return 0;

		char* newString = new char[newSize+1];

		///Algo 4.Replace#2
		char* ptr = newString;
		const char* p = m_sharedString->string;

		while ((pos = Find(oldString, pos, flags)) != npos)
		{
			const char* r = &m_sharedString->string[pos];

			std::memcpy(ptr, p, (r-p)*sizeof(char));
			ptr += r-p;
			std::memcpy(ptr, replaceString.m_sharedString->string, replaceString.m_sharedString->size*sizeof(char));
			ptr += replaceString.m_sharedString->size;
			p = r+oldString.m_sharedString->size;
			pos += oldString.m_sharedString->size;

			count++;
		}

		std::strcpy(ptr, p); // Ajoute le caractère de fin par la même occasion

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = newSize;
		m_sharedString->size = newSize;
		m_sharedString->string = newString;
	}

	return count;
}

unsigned int NzString::ReplaceAny(const char* oldCharacters, char replaceCharacter, int start, nzUInt32 flags)
{
	if (!oldCharacters || !oldCharacters[0])
		return 0;

	/*if (replaceCharacter == '\0') // Dans ce cas, il faut passer par un algorithme plus complexe
		return ReplaceAny(NzString(oldCharacters), NzString(), start);*/

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
			char character = nzToLower(*ptr);
			bool found = false;
			do
			{
				if (character == nzToLower(*c))
				{
					if (!found)
					{
						unsigned int pos = ptr-m_sharedString->string;

						EnsureOwnership();

						ptr = &m_sharedString->string[pos];
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
				unsigned int pos = ptr-m_sharedString->string;

				EnsureOwnership();

				ptr = &m_sharedString->string[pos];
				found = true;
			}

			*ptr++ = replaceCharacter;
			++count;
		}
	}

	return count;
}
/*
	unsigned int NzString::ReplaceAny(const char* oldCharacters, const char* replaceString, int start, nzUInt32 flags)
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

	unsigned int NzString::ReplaceAny(const NzString& oldCharacters, const NzString& replaceString, int start, nzUInt32 flags)
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
void NzString::Reserve(unsigned int bufferSize)
{
	if (m_sharedString->allocatedSize >= bufferSize)
		return;

	char* ptr = new char[bufferSize+1];
	if (m_sharedString->size > 0)
		std::strcpy(ptr, m_sharedString->string);

	unsigned int size = m_sharedString->size;

	ReleaseString();
	m_sharedString = new SharedString;
	m_sharedString->allocatedSize = bufferSize;
	m_sharedString->size = size;
	m_sharedString->string = ptr;
}

NzString& NzString::Resize(int size, char character)
{
	if (size < 0)
		size = std::max(static_cast<int>(m_sharedString->size + size), 0);

	unsigned int newSize = static_cast<unsigned int>(size);

	if (m_sharedString->allocatedSize >= newSize)
	{
		EnsureOwnership();

		// Nous avons déjà la place requise, contentons-nous de remplir le buffer
		if (character != '\0' && newSize > m_sharedString->size)
		{
			char* ptr = &m_sharedString->string[m_sharedString->size];
			char* limit = &m_sharedString->string[newSize];
			while (ptr != limit)
				*ptr++ = character;
		}

		m_sharedString->size = newSize;
		m_sharedString->string[newSize] = '\0';
	}
	else // On veut forcément agrandir la chaine
	{
		char* newString = new char[newSize+1];
		if (m_sharedString->size != 0)
			std::memcpy(newString, m_sharedString->string, newSize*sizeof(char));

		char* ptr = &newString[m_sharedString->size];
		char* limit = &newString[newSize];
		while (ptr != limit)
			*ptr++ = character;

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = newSize;
		m_sharedString->size = newSize;
		m_sharedString->string = newString;
	}

	return *this;
}

NzString NzString::Resized(int size, char character) const
{
	if (size < 0)
		size = m_sharedString->size + size;

	if (size <= 0)
		return NzString();

	unsigned int newSize = static_cast<unsigned int>(size);
	if (newSize == m_sharedString->size)
		return *this;

	char* str = new char[newSize+1];
	if (newSize > m_sharedString->size)
	{
		std::memcpy(str, m_sharedString->string, m_sharedString->size);
		if (character != '\0')
		{
			char* ptr = &str[m_sharedString->size];
			char* limit = &str[newSize];
			while (ptr != limit)
				*ptr++ = character;
		}
	}
	else
		std::memcpy(str, m_sharedString->string, newSize);

	str[newSize] = '\0';

	return NzString(new SharedString(newSize, newSize, 1, str));
}

NzString& NzString::Reverse()
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

NzString NzString::Reversed() const
{
	if (m_sharedString->size == 0)
		return NzString();

	char* str = new char[m_sharedString->size+1];

	char* ptr = &str[m_sharedString->size-1];
	char* p = m_sharedString->string;

	do
		*ptr-- = *p;
	while (*(++p));

	str[m_sharedString->size] = '\0';

	return NzString(new SharedString(m_sharedString->size, m_sharedString->size, 1, str));
}

NzString NzString::Simplified(nzUInt32 flags) const
{
	char* str = new char[m_sharedString->size+1];
	char* p = str;

	const char* ptr = m_sharedString->string;
	bool inword = false;
	if (flags & HandleUtf8)
	{
		utf8::unchecked::iterator<const char*> it(ptr);
		do
		{
			if (NzUnicode::GetCategory(*it))
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

	if (!inword)
		p--;

	*p = '\0';

	return NzString(new SharedString(m_sharedString->size, p-str, 1, str));
}

NzString& NzString::Simplify(nzUInt32 flags)
{
	return operator=(Simplified(flags));
}

unsigned int NzString::Split(std::vector<NzString>& result, char separation, int start, nzUInt32 flags) const
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
		result.push_back(Substr(0, lastSep-1));

	while (true)
	{
		unsigned int sep = Find(separation, lastSep+1, flags);
		if (sep == npos)
			break;

		if (sep-lastSep > 1)
			result.push_back(Substr(lastSep+1, sep-1));

		lastSep = sep;
	}

	if (lastSep != m_sharedString->size-1)
		result.push_back(Substr(lastSep+1));

	return result.size();
}

unsigned int NzString::Split(std::vector<NzString>& result, const char* separation, int start, nzUInt32 flags) const
{
	unsigned int size = (separation) ? std::strlen(separation) : 0;
	if (m_sharedString->size == 0)
		return 0;
	else if (size == 0)
	{
		result.reserve(m_sharedString->size);
		for (unsigned int i = 0; i < m_sharedString->size; ++i)
			result.push_back(NzString(m_sharedString->string[i]));

		return m_sharedString->size;
	}
	else if (size > m_sharedString->size)
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
		result.push_back(Substr(0, lastSep-1));

	unsigned int sep;
	while ((sep = Find(separation, lastSep+size, flags)) != npos)
	{
		if (sep-lastSep > size)
			result.push_back(Substr(lastSep+size, sep-1));

		lastSep = sep;
	}

	if (lastSep != m_sharedString->size-size)
		result.push_back(Substr(lastSep+size));

	return result.size()-oldSize;
}

unsigned int NzString::Split(std::vector<NzString>& result, const NzString& separation, int start, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return 0;
	else if (separation.m_sharedString->size == 0)
	{
		result.reserve(m_sharedString->size);
		for (unsigned int i = 0; i < m_sharedString->size; ++i)
			result.push_back(NzString(m_sharedString->string[i]));

		return m_sharedString->size;
	}
	else if (separation.m_sharedString->size > m_sharedString->size)
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
		result.push_back(Substr(0, lastSep-1));

	unsigned int sep;
	while ((sep = Find(separation, lastSep+separation.m_sharedString->size, flags)) != npos)
	{
		if (sep-lastSep > separation.m_sharedString->size)
			result.push_back(Substr(lastSep+separation.m_sharedString->size, sep-1));

		lastSep = sep;
	}

	if (lastSep != m_sharedString->size-separation.m_sharedString->size)
		result.push_back(Substr(lastSep+separation.m_sharedString->size));

	return result.size()-oldSize;
}

unsigned int NzString::SplitAny(std::vector<NzString>& result, const char* separations, int start, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return 0;

	unsigned int lastSep = FindAny(separations, start, flags);
	unsigned int oldSize = result.size();
	if (lastSep == npos)
	{
		result.push_back(*this);
		return 1;
	}
	else if (lastSep != 0)
		result.push_back(Substr(0, lastSep-1));

	unsigned int sep;
	while ((sep = FindAny(separations, lastSep+1, flags)) != npos)
	{
		if (sep-lastSep > 1)
			result.push_back(Substr(lastSep+1, sep-1));

		lastSep = sep;
	}

	if (lastSep != m_sharedString->size-1)
		result.push_back(Substr(lastSep+1));

	return result.size()-oldSize;
}

unsigned int NzString::SplitAny(std::vector<NzString>& result, const NzString& separations, int start, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return 0;

	unsigned int lastSep = FindAny(separations, start, flags);
	unsigned int oldSize = result.size();
	if (lastSep == npos)
	{
		result.push_back(*this);
		return 1;
	}
	else if (lastSep != 0)
		result.push_back(Substr(0, lastSep-1));

	unsigned int sep;
	while ((sep = FindAny(separations, lastSep+1, flags)) != npos)
	{
		if (sep-lastSep > 1)
			result.push_back(Substr(lastSep+1, sep-1));

		lastSep = sep;
	}

	if (lastSep != m_sharedString->size-1)
		result.push_back(Substr(lastSep+1));

	return result.size()-oldSize;
}

bool NzString::StartsWith(char character, nzUInt32 flags) const
{
	if (character == '\0' || m_sharedString->size == 0)
		return false;

	if (flags & CaseInsensitive)
		return nzToLower(m_sharedString->string[0]) == nzToLower(character);
	else
		return m_sharedString->string[0] == character;
}

bool NzString::StartsWith(const char* string, nzUInt32 flags) const
{
	if (!string || !string[0] || m_sharedString->size == 0)
		return false;

	if (flags & CaseInsensitive)
	{
		if (flags & HandleUtf8)
		{
			utf8::unchecked::iterator<const char*> it(m_sharedString->string);
			utf8::unchecked::iterator<const char*> it2(string);
			do
			{
				if (*it2 == '\0')
					return true;

				if (NzUnicode::GetLowercase(*it) != NzUnicode::GetLowercase(*it2))
					return false;

				++it2;
			}
			while (*it++);
		}
		else
		{
			char* ptr = m_sharedString->string;
			const char* s = string;
			do
			{
				if (*s == '\0')
					return true;

				if (nzToLower(*ptr) != nzToLower(*s))
					return false;

				s++;
			}
			while (*ptr++);
		}
	}
	else
	{
		char* ptr = m_sharedString->string;
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

bool NzString::StartsWith(const NzString& string, nzUInt32 flags) const
{
	if (string.m_sharedString->size == 0)
		return false;

	if (m_sharedString->size < string.m_sharedString->size)
		return false;

	if (flags & CaseInsensitive)
	{
		if (flags & HandleUtf8)
		{
			utf8::unchecked::iterator<const char*> it(m_sharedString->string);
			utf8::unchecked::iterator<const char*> it2(string.m_sharedString->string);
			do
			{
				if (*it2 == '\0')
					return true;

				if (NzUnicode::GetLowercase(*it) != NzUnicode::GetLowercase(*it2))
					return false;

				++it2;
			}
			while (*it++);
		}
		else
		{
			char* ptr = m_sharedString->string;
			const char* s = string.m_sharedString->string;
			do
			{
				if (*s == '\0')
					return true;

				if (nzToLower(*ptr) != nzToLower(*s))
					return false;

				s++;
			}
			while (*ptr++);
		}
	}
	else
		return std::memcmp(m_sharedString->string, string.m_sharedString->string, string.m_sharedString->size*sizeof(char)) == 0;

	return false;
}

NzString NzString::Substr(int startPos, int endPos) const
{
	if (startPos < 0)
		startPos = std::max(m_sharedString->size+startPos, 0U);

	unsigned int start = static_cast<unsigned int>(startPos);

	if (endPos < 0)
	{
		endPos = m_sharedString->size+endPos;
		if (endPos < 0)
			return NzString();
	}

	unsigned int end = std::min(static_cast<unsigned int>(endPos), m_sharedString->size-1);

	if (start > end || start >= m_sharedString->size)
		return NzString();

	unsigned int size = end-start+1;
	char* str = new char[size+1];
	std::memcpy(str, &m_sharedString->string[start], size*sizeof(char));
	str[size] = '\0';

	return NzString(new SharedString(size, size, 1, str));
}

NzString NzString::SubstrFrom(char character, int startPos, bool fromLast, bool include, nzUInt32 flags) const
{
	if (character == '\0')
		return *this;

	unsigned int pos;
	if (fromLast)
		pos = FindLast(character, startPos, flags);
	else
		pos = Find(character, startPos, flags);

	if (pos == 0 and include)
		return *this;
	else if (pos == npos)
		return NzString();

	return Substr(pos+((include) ? 0 : 1));
}

NzString NzString::SubstrFrom(const char* string, int startPos, bool fromLast, bool include, nzUInt32 flags) const
{
	unsigned int pos;
	if (fromLast)
		pos = FindLast(string, startPos, flags);
	else
		pos = Find(string, startPos, flags);

	if (pos == 0 && include)
		return *this;
	else if (pos == npos)
		return NzString();

	return Substr(pos+((include) ? 0 : std::strlen(string)));
}

NzString NzString::SubstrFrom(const NzString& string, int startPos, bool fromLast, bool include, nzUInt32 flags) const
{
	unsigned int pos;
	if (fromLast)
		pos = FindLast(string, startPos, flags);
	else
		pos = Find(string, startPos, flags);

	if (pos == 0 && include)
		return *this;
	else if (pos == npos)
		return NzString();

	return Substr(pos+((include) ? 0 : string.m_sharedString->size));
}

NzString NzString::SubstrTo(char character, int startPos, bool toLast, bool include, nzUInt32 flags) const
{
	if (character == '\0')
		return *this;

	unsigned int pos;
	if (toLast)
		pos = FindLast(character, startPos);
	else
		pos = Find(character, startPos, flags);

	if (pos == 0)
		return (include) ? character : NzString();
	else if (pos == npos)
		return *this;

	return Substr(0, pos+((include) ? 1 : 0)-1);
}

NzString NzString::SubstrTo(const char* string, int startPos, bool toLast, bool include, nzUInt32 flags) const
{
	unsigned int pos;
	if (toLast)
		pos = FindLast(string, startPos, flags);
	else
		pos = Find(string, startPos, flags);

	if (pos == 0)
		return (include) ? string : NzString();
	else if (pos == npos)
		return *this;

	return Substr(0, pos+((include) ? std::strlen(string) : 0)-1);
}

NzString NzString::SubstrTo(const NzString& string, int startPos, bool toLast, bool include, nzUInt32 flags) const
{
	unsigned int pos;
	if (toLast)
		pos = FindLast(string, startPos, flags);
	else
		pos = Find(string, startPos, flags);

	if (pos == 0)
		return (include) ? string : NzString();
	else if (pos == npos)
		return *this;

	return Substr(0, pos+((include) ? string.m_sharedString->size : 0)-1);
}

void NzString::Swap(NzString& str)
{
	std::swap(m_sharedString, str.m_sharedString);
}

bool NzString::ToBool(bool* value, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return false;

	NzString word = GetWord(0);

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

bool NzString::ToDouble(double* value) const
{
	if (m_sharedString->size)
		return false;

	if (value)
		*value = std::atof(m_sharedString->string);

	return true;
}

bool NzString::ToInteger(long long* value, nzUInt8 base) const
{
	if (!IsNumber(base))
		return false;

	if (value)
		*value = NzStringToNumber(*this, base);

	return true;
}

NzString NzString::ToLower(nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return *this;

	if (flags & HandleUtf8)
	{
		NzString lower;
		lower.Reserve(m_sharedString->size);
		utf8::unchecked::iterator<const char*> it(m_sharedString->string);
		do
			utf8::append(NzUnicode::GetLowercase(*it), std::back_inserter(lower));
		while (*++it);

		return lower;
	}
	else
	{
		char* str = new char[m_sharedString->size+1];

		char* ptr = m_sharedString->string;
		char* s = str;
		do
			*s++ = nzToLower(*ptr);
		while (*++ptr);

		*s = '\0';

		return NzString(new SharedString(m_sharedString->size, m_sharedString->size, 1, str));
	}
}

NzString NzString::ToUpper(nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return *this;

	if (flags & HandleUtf8)
	{
		NzString upper;
		upper.Reserve(m_sharedString->size);
		utf8::unchecked::iterator<const char*> it(m_sharedString->string);
		do
			utf8::append(NzUnicode::GetUppercase(*it), std::back_inserter(upper));
		while (*++it);

		return upper;
	}
	else
	{
		char* str = new char[m_sharedString->size+1];

		char* ptr = m_sharedString->string;
		char* s = str;
		do
			*s++ = nzToUpper(*ptr);
		while (*++ptr);

		*s = '\0';

		return NzString(new SharedString(m_sharedString->size, m_sharedString->size, 1, str));
	}
}

NzString& NzString::Trim(nzUInt32 flags)
{
	return operator=(Trimmed(flags));
}

NzString& NzString::Trim(char character, nzUInt32 flags)
{
	return operator=(Trimmed(character, flags));
}

NzString NzString::Trimmed(nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return *this;

	unsigned int startPos = 0;
	unsigned int endPos = m_sharedString->size-1;
	if (flags & HandleUtf8)
	{
		utf8::unchecked::iterator<const char*> it(m_sharedString->string);
		do
		{
			if (NzUnicode::GetCategory(*it) & NzUnicode::Category_Separator)
				break;
		}
		while (*++it);
		startPos = it.base() - m_sharedString->string;

		utf8::unchecked::iterator<const char*> itR(&m_sharedString->string[m_sharedString->size]);
		while ((itR--).base() != m_sharedString->string)
		{
			if (NzUnicode::GetCategory(*itR) & NzUnicode::Category_Separator)
				break;
		}
		endPos = itR.base() - m_sharedString->string;
	}
	else
	{
		for (; startPos < m_sharedString->size; ++startPos)
		{
			if (!std::isspace(m_sharedString->string[startPos]))
				break;
		}

		for (; endPos > 0; --endPos)
		{
			if (!std::isspace(m_sharedString->string[endPos]))
				break;
		}
	}

	return Substr(startPos, endPos);
}

NzString NzString::Trimmed(char character, nzUInt32 flags) const
{
	if (m_sharedString->size == 0)
		return *this;

	unsigned int startPos = 0;
	unsigned int endPos = m_sharedString->size-1;
	if (flags & CaseInsensitive)
	{
		char ch = nzToLower(character);
		for (; startPos < m_sharedString->size; ++startPos)
		{
			if (nzToLower(m_sharedString->string[startPos]) != ch)
				break;
		}

		for (; endPos > 0; --endPos)
		{
			if (nzToLower(m_sharedString->string[startPos]) != ch)
				break;
		}
	}
	else
	{
		for (; startPos < m_sharedString->size; ++startPos)
		{
			if (m_sharedString->string[startPos] != character)
				break;
		}

		for (; endPos > 0; --endPos)
		{
			if (m_sharedString->string[startPos] != character)
				break;
		}
	}

	return Substr(startPos, endPos);
}

char* NzString::begin()
{
	return m_sharedString->string;
}

const char* NzString::begin() const
{
	return m_sharedString->string;
}

char* NzString::end()
{
	return &m_sharedString->string[m_sharedString->size];
}

const char* NzString::end() const
{
	return &m_sharedString->string[m_sharedString->size];
}

void NzString::push_front(char c)
{
	operator=(c + *this);
}

void NzString::push_back(char c)
{
	operator+=(c);
}
/*
char* NzString::rbegin()
{
	return &m_sharedString->string[m_sharedString->size-1];
}

const char* NzString::rbegin() const
{
	return &m_sharedString->string[m_sharedString->size-1];
}

char* NzString::rend()
{
	return &m_sharedString->string[-1];
}

const char* NzString::rend() const
{
	return &m_sharedString->string[-1];
}
*/

NzString::operator std::string() const
{
	return std::string(m_sharedString->string, m_sharedString->size);
}

char& NzString::operator[](unsigned int pos)
{
	EnsureOwnership();

	if (pos >= m_sharedString->size)
		Resize(pos+1);

	return m_sharedString->string[pos];
}

char NzString::operator[](unsigned int pos) const
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

NzString& NzString::operator=(char character)
{
	if (character != '\0')
	{
		if (m_sharedString->allocatedSize >= 1)
			EnsureOwnership();
		else
		{
			ReleaseString();
			m_sharedString = new SharedString;
			m_sharedString->allocatedSize = 1;
			m_sharedString->string = new char[2];
		}

		m_sharedString->size = 1;
		m_sharedString->string[0] = character;
		m_sharedString->string[1] = '\0';
	}
	else
		ReleaseString();

	return *this;
}

NzString& NzString::operator=(const char* string)
{
	if (string && string[0] != '\0')
	{
		unsigned int size = std::strlen(string);
		if (m_sharedString->allocatedSize >= size)
			EnsureOwnership();
		else
		{
			ReleaseString();

			m_sharedString = new SharedString;
			m_sharedString->allocatedSize = size;
			m_sharedString->string = new char[size+1];
		}

		m_sharedString->size = size;
		std::strcpy(m_sharedString->string, string);
	}
	else
		ReleaseString();

	return *this;
}

NzString& NzString::operator=(const std::string& string)
{
	if (string.size() > 0)
	{
		if (m_sharedString->allocatedSize >= string.size())
			EnsureOwnership();
		else
		{
			ReleaseString();

			m_sharedString = new SharedString;
			m_sharedString->allocatedSize = string.size();
			m_sharedString->string = new char[string.size()+1];
		}

		m_sharedString->size = string.size();
		std::strcpy(m_sharedString->string, string.c_str());
	}
	else
		ReleaseString();

	return *this;
}

NzString& NzString::operator=(const NzString& string)
{
	ReleaseString();

	m_sharedString = string.m_sharedString;
	NazaraMutexLock(m_sharedString->mutex);
	m_sharedString->refCount++;
	NazaraMutexUnlock(m_sharedString->mutex);

	return *this;
}

NzString& NzString::operator=(NzString&& string)
{
	std::swap(m_sharedString, string.m_sharedString);

	return *this;
}

NzString NzString::operator+(char character) const
{
	if (character == '\0')
		return *this;

	unsigned int totalSize = m_sharedString->size+1;
	char* str = new char[totalSize+1];
	std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));

	str[m_sharedString->size] = character;
	str[totalSize] = '\0';

	return NzString(new SharedString(totalSize, totalSize, 1, str));
}

NzString NzString::operator+(const char* string) const
{
	if (!string || !string[0])
		return *this;

	if (m_sharedString->size == 0)
		return string;

	unsigned int length = std::strlen(string);
	if (length == 0)
		return *this;

	unsigned int totalSize = m_sharedString->size + length;
	char* str = new char[totalSize+1];
	std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
	std::strcpy(&str[m_sharedString->size], string);

	return NzString(new SharedString(totalSize, totalSize, 1, str));
}

NzString NzString::operator+(const std::string& string) const
{
	if (string.empty())
		return *this;

	if (m_sharedString->size == 0)
		return string;

	unsigned int totalSize = m_sharedString->size + string.size();
	char* str = new char[totalSize+1];
	std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
	std::strcpy(&str[m_sharedString->size], string.c_str());

	return NzString(new SharedString(totalSize, totalSize, 1, str));
}

NzString NzString::operator+(const NzString& string) const
{
	if (string.m_sharedString->size == 0)
		return *this;

	if (m_sharedString->size == 0)
		return string;

	unsigned int totalSize = m_sharedString->size + string.m_sharedString->size;
	char* str = new char[totalSize+1];
	std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
	std::strcpy(&str[m_sharedString->size], string.m_sharedString->string);

	return NzString(new SharedString(totalSize, totalSize, 1, str));
}

NzString& NzString::operator+=(char character)
{
	if (character == '\0')
		return *this;

	if (m_sharedString->size == 0)
		return operator=(character);

	if (m_sharedString->allocatedSize > m_sharedString->size)
	{
		EnsureOwnership();

		m_sharedString->string[m_sharedString->size] = character;
		m_sharedString->string[m_sharedString->size+1] = '\0';
		m_sharedString->size++;
	}
	else
	{
		unsigned int newSize = m_sharedString->size+1;
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		str[m_sharedString->size] = character;
		str[newSize] = '\0';

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

NzString& NzString::operator+=(const char* string)
{
	if (!string || !string[0])
		return *this;

	if (m_sharedString->size == 0)
		return operator=(string);

	unsigned int length = std::strlen(string);
	if (length == 0)
		return *this;

	if (m_sharedString->allocatedSize >= m_sharedString->size + length)
	{
		EnsureOwnership();

		std::strcpy(&m_sharedString->string[m_sharedString->size], string);
		m_sharedString->size += length;
	}
	else
	{
		unsigned int newSize = m_sharedString->size + length;
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		std::strcpy(&str[m_sharedString->size], string);

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

NzString& NzString::operator+=(const std::string& string)
{
	if (string.empty())
		return *this;

	if (m_sharedString->size == 0)
		return operator=(string);

	if (m_sharedString->allocatedSize >= m_sharedString->size + string.size())
	{
		EnsureOwnership();

		std::strcpy(&m_sharedString->string[m_sharedString->size], string.c_str());
		m_sharedString->size += string.size();
	}
	else
	{
		unsigned int newSize = m_sharedString->size + string.size();
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		std::strcpy(&str[m_sharedString->size], string.c_str());

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

NzString& NzString::operator+=(const NzString& string)
{
	if (string.m_sharedString->size == 0)
		return *this;

	if (m_sharedString->size == 0)
		return operator=(string);

	if (m_sharedString->allocatedSize >= m_sharedString->size + string.m_sharedString->size)
	{
		EnsureOwnership();

		std::strcpy(&m_sharedString->string[m_sharedString->size], string.m_sharedString->string);
		m_sharedString->size += string.m_sharedString->size;
	}
	else
	{
		unsigned int newSize = m_sharedString->size + string.m_sharedString->size;
		unsigned int bufferSize = nzGetNewSize(newSize);

		char* str = new char[bufferSize+1];
		std::memcpy(str, m_sharedString->string, m_sharedString->size*sizeof(char));
		std::strcpy(&str[m_sharedString->size], string.m_sharedString->string);

		ReleaseString();
		m_sharedString = new SharedString;
		m_sharedString->allocatedSize = bufferSize;
		m_sharedString->size = newSize;
		m_sharedString->string = str;
	}

	return *this;
}

bool NzString::operator==(char character) const
{
	if (m_sharedString->size == 0)
		return character == '\0';

	if (m_sharedString->size > 1)
		return false;

	return m_sharedString->string[0] == character;
}

bool NzString::operator==(const char* string) const
{
	if (m_sharedString->size == 0)
		return !string || !string[0];

	if (!string || !string[0])
		return false;

	return std::strcmp(m_sharedString->string, string) == 0;
}

bool NzString::operator==(const std::string& string) const
{
	if (m_sharedString->size == 0 || string.empty())
		return m_sharedString->size == string.size();

	if (m_sharedString->size != string.size())
		return false;

	return std::strcmp(m_sharedString->string, string.c_str()) == 0;
}

bool NzString::operator!=(char character) const
{
	if (m_sharedString->size == 0)
		return character != '\0';

	if (character == '\0' || m_sharedString->size != 1)
		return true;

	if (m_sharedString->size != 1)
		return true;

	return m_sharedString->string[0] != character;
}

bool NzString::operator!=(const char* string) const
{
	if (m_sharedString->size == 0)
		return string && string[0];

	if (!string || !string[0])
		return true;

	return std::strcmp(m_sharedString->string, string) != 0;
}

bool NzString::operator!=(const std::string& string) const
{
	if (m_sharedString->size == 0 || string.empty())
		return m_sharedString->size == string.size();

	if (m_sharedString->size != string.size())
		return false;

	return std::strcmp(m_sharedString->string, string.c_str()) != 0;
}

bool NzString::operator<(char character) const
{
	if (character == '\0')
		return false;

	if (m_sharedString->size == 0)
		return true;

	return m_sharedString->string[0] < character;
}

bool NzString::operator<(const char* string) const
{
	if (!string || !string[0])
		return false;

	if (m_sharedString->size == 0)
		return true;

	return std::strcmp(m_sharedString->string, string) < 0;
}

bool NzString::operator<(const std::string& string) const
{
	if (string.empty())
		return false;

	if (m_sharedString->size == 0)
		return true;

	return std::strcmp(m_sharedString->string, string.c_str()) < 0;
}

bool NzString::operator<=(char character) const
{
	if (m_sharedString->size == 0)
		return true;

	if (character == '\0')
		return false;

	return m_sharedString->string[0] < character || (m_sharedString->string[0] == character && m_sharedString->size == 1);
}

bool NzString::operator<=(const char* string) const
{
	if (m_sharedString->size == 0)
		return true;

	if (!string || !string[0])
		return false;

	return std::strcmp(m_sharedString->string, string) <= 0;
}

bool NzString::operator<=(const std::string& string) const
{
	if (m_sharedString->size == 0)
		return true;

	if (string.empty())
		return false;

	return std::strcmp(m_sharedString->string, string.c_str()) <= 0;
}

bool NzString::operator>(char character) const
{
	if (m_sharedString->size == 0)
		return false;

	if (character == '\0')
		return true;

	return m_sharedString->string[0] > character;
}

bool NzString::operator>(const char* string) const
{
	if (m_sharedString->size == 0)
		return false;

	if (!string || !string[0])
		return true;

	return std::strcmp(m_sharedString->string, string) > 0;
}

bool NzString::operator>(const std::string& string) const
{
	if (m_sharedString->size == 0)
		return false;

	if (string.empty())
		return true;

	return std::strcmp(m_sharedString->string, string.c_str()) > 0;
}

bool NzString::operator>=(char character) const
{
	if (character == '\0')
		return true;

	if (m_sharedString->size == 0)
		return false;

	return m_sharedString->string[0] > character || (m_sharedString->string[0] == character && m_sharedString->size == 1);
}

bool NzString::operator>=(const char* string) const
{
	if (!string || !string[0])
		return true;

	if (m_sharedString->size == 0)
		return false;

	return std::strcmp(m_sharedString->string, string) >= 0;
}

bool NzString::operator>=(const std::string& string) const
{
	if (string.empty())
		return true;

	if (m_sharedString->size == 0)
		return false;

	return std::strcmp(m_sharedString->string, string.c_str()) >= 0;
}

NzString NzString::Boolean(bool boolean)
{
	unsigned int size = (boolean) ? 4 : 5;
	char* str = new char[size+1];
	std::strcpy(str, (boolean) ? "true" : "false");

	return NzString(new SharedString(size, size, 1, str));
}

int NzString::Compare(const NzString& first, const NzString& second)
{
	if (first.m_sharedString->size == 0)
		return (second.m_sharedString->size == 0) ? 0 : -1;

	if (second.m_sharedString->size == 0)
		return 1;

	return std::strcmp(first.m_sharedString->string, second.m_sharedString->string);
}

NzString NzString::Number(float number)
{
	std::ostringstream oss;
	oss.precision(NAZARA_CORE_REAL_PRECISION);
	oss << number;

	return NzString(oss.str());
}

NzString NzString::Number(double number)
{
	std::ostringstream oss;
	oss.precision(NAZARA_CORE_REAL_PRECISION);
	oss << number;

	return NzString(oss.str());
}

NzString NzString::Number(long double number)
{
	std::ostringstream oss;
	oss.precision(NAZARA_CORE_REAL_PRECISION);
	oss << number;

	return NzString(oss.str());
}

NzString NzString::Number(signed char number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(unsigned char number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(short number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(unsigned short number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(int number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(unsigned int number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(long number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(unsigned long number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(long long number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Number(unsigned long long number, nzUInt8 radix)
{
	return NzNumberToString(number, radix);
}

NzString NzString::Pointer(const void* ptr)
{
	unsigned int size = sizeof(ptr)*2;
	char* str = new char[size+1];
	std::sprintf(str, "%p", ptr);

	return NzString(new SharedString(size, size, 1, str));
}

NzString NzString::Unicode(char32_t character)
{
	if (character == '\0')
		return NzString();

	unsigned int count = 0;
	if (character < 0x80)
		count = 1;
	else if (character < 0x800)
		count = 2;
	else if (character < 0x10000)
		count = 3;
    else
		count = 4;

	char* str = new char[count+1];
	utf8::append(character, str);
	str[count] = '\0';

	return NzString(new SharedString(count, count, 1, str));
}

NzString NzString::Unicode(const char* u8String)
{
	return NzString(u8String);
}

NzString NzString::Unicode(const char16_t* u16String)
{
	if (!u16String || !u16String[0])
		return NzString();

	const char16_t* ptr = u16String;
	unsigned int count = 0;
	do
		count++;
	while (*++ptr);

	count *= 2; // On s'assure d'avoir la place suffisante

	char* str = new char[count+1];
	char* r = utf8::utf16to8(u16String, ptr, str);
	*r = '\0';

	return NzString(new SharedString(count, r-str, 1, str));
}

NzString NzString::Unicode(const char32_t* u32String)
{
	if (!u32String || !u32String[0])
		return NzString();

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

	char* str = new char[count+1];
	char* r = utf8::utf32to8(u32String, ptr, str);
	*r = '\0';

	return NzString(new SharedString(count, count, 1, str));
}

NzString NzString::Unicode(const wchar_t* wString)
{
	if (!wString || !wString[0])
		return NzString();

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

	char* str = new char[count+1];
	char* r = utf8::utf32to8(wString, ptr, str);
	*r = '\0';

	return NzString(new SharedString(count, count, 1, str));
}

std::istream& operator>>(std::istream& is, NzString& str)
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

std::ostream& operator<<(std::ostream& os, const NzString& str)
{
	if (str.IsEmpty())
		return os;

	return operator<<(os, str.m_sharedString->string);
}

NzString operator+(char character, const NzString& string)
{
	if (character == '\0')
		return string;

	if (string.IsEmpty())
		return character;

	unsigned int totalSize = string.m_sharedString->size+1;
	char* str = new char[totalSize+1];
	str[0] = character;
	std::strcpy(str, string.m_sharedString->string);

	return NzString(new NzString::SharedString(totalSize, totalSize, 1, str));
}

NzString operator+(const char* string, const NzString& nstring)
{
	if (!string || !string[0])
		return nstring;

	if (nstring.IsEmpty())
		return string;

	unsigned int size = std::strlen(string);
	unsigned int totalSize = size + nstring.m_sharedString->size;
	char* str = new char[totalSize+1];
	std::memcpy(str, string, size*sizeof(char));
	std::strcpy(&str[size], nstring.m_sharedString->string);

	return NzString(new NzString::SharedString(totalSize, totalSize, 1, str));
}

NzString operator+(const std::string& string, const NzString& nstring)
{
	if (string.empty())
		return nstring;

	if (nstring.m_sharedString->size == 0)
		return string;

	unsigned int totalSize = string.size() + nstring.m_sharedString->size;
	char* str = new char[totalSize+1];
	std::memcpy(str, string.c_str(), string.size()*sizeof(char));
	std::strcpy(&str[string.size()], nstring.m_sharedString->string);

	return NzString(new NzString::SharedString(totalSize, totalSize, 1, str));
}

bool operator==(const NzString& first, const NzString& second)
{
	if (first.m_sharedString->size == 0 || second.m_sharedString->size == 0)
		return first.m_sharedString->size == second.m_sharedString->size;

	if (first.m_sharedString->size != second.m_sharedString->size)
		return false;

	return std::strcmp(first.m_sharedString->string, second.m_sharedString->string) == 0;
}

bool operator!=(const NzString& first, const NzString& second)
{
	return !operator==(first, second);
}

bool operator<(const NzString& first, const NzString& second)
{
	if (second.m_sharedString->size == 0)
		return false;

	if (first.m_sharedString->size == 0)
		return true;

	return std::strcmp(first.m_sharedString->string, second.m_sharedString->string) < 0;
}

bool operator<=(const NzString& first, const NzString& second)
{
	return !operator<(second, first);
}

bool operator>(const NzString& first, const NzString& second)
{
	return second < first;
}

bool operator>=(const NzString& first, const NzString& second)
{
	return !operator<(first, second);
}

bool operator==(char character, const NzString& nstring)
{
	return nstring == character;
}

bool operator==(const char* string, const NzString& nstring)
{
	return nstring == string;
}

bool operator==(const std::string& string, const NzString& nstring)
{
	return nstring == string;
}

bool operator!=(char character, const NzString& nstring)
{
	return !operator==(character, nstring);
}

bool operator!=(const char* string, const NzString& nstring)
{
	return !operator==(string, nstring);
}

bool operator!=(const std::string& string, const NzString& nstring)
{
	return !operator==(string, nstring);
}

bool operator<(char character, const NzString& nstring)
{
	return nstring > character;
}

bool operator<(const char* string, const NzString& nstring)
{
	return nstring > string;
}

bool operator<(const std::string& string, const NzString& nstring)
{
	return nstring > string;
}

bool operator<=(char character, const NzString& nstring)
{
	return !operator<(nstring, character);
}

bool operator<=(const char* string, const NzString& nstring)
{
	return !operator<(nstring, string);
}

bool operator<=(const std::string& string, const NzString& nstring)
{
	return !operator<(nstring, string);
}

bool operator>(char character, const NzString& nstring)
{
	return nstring < character;
}

bool operator>(const char* string, const NzString& nstring)
{
	return nstring < string;
}

bool operator>(const std::string& string, const NzString& nstring)
{
	return nstring < string;
}

bool operator>=(char character, const NzString& nstring)
{
	return !operator<(character, nstring);
}

bool operator>=(const char* string, const NzString& nstring)
{
	return !operator<(string, nstring);
}

bool operator>=(const std::string& string, const NzString& nstring)
{
	return !operator<(string, nstring);
}

void NzString::EnsureOwnership()
{
	NazaraLock(m_sharedString->mutex);
	if (m_sharedString->refCount > 1)
	{
		m_sharedString->refCount--;

		char* string = new char[m_sharedString->allocatedSize+1];
		std::strcpy(string, m_sharedString->string);

		m_sharedString = new SharedString(m_sharedString->allocatedSize, m_sharedString->size, 1, string);
	}
}

bool NzString::FillHash(NzHashImpl* hazh) const
{
	hazh->Append(reinterpret_cast<const nzUInt8*>(m_sharedString->string), m_sharedString->size);

	return true;
}

void NzString::ReleaseString()
{
	if (m_sharedString == &emptyString)
		return;

	NazaraMutexLock(m_sharedString->mutex);
	if (--m_sharedString->refCount == 0)
	{
		NazaraMutexUnlock(m_sharedString->mutex);
		delete[] m_sharedString->string;
		delete m_sharedString;
	}
	else
	{
		NazaraMutexUnlock(m_sharedString->mutex);
	}

	m_sharedString = &emptyString;
}

NzString::SharedString NzString::emptyString(0, 0, 0, nullptr);
unsigned int NzString::npos(static_cast<unsigned int>(-1));

namespace std
{
	istream& getline(istream& is, NzString& str)
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

	istream& getline(istream& is, NzString& str, char delim)
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

	void swap(NzString& lhs, NzString& rhs)
	{
		lhs.Swap(rhs);
	}
}
