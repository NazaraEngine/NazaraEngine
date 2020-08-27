// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Utfcpp/utf8.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		bool IsSpace(char32_t character)
		{
			return character == '\t' || Unicode::GetCategory(character) & Unicode::Category_Separator;
		}

		char ToLower(char character)
		{
			if (character >= 'A' && character <= 'Z')
				return character + ('a' - 'A');
			else
				return character;
		}

		char ToUpper(char character)
		{
			if (character >= 'a' && character <= 'z')
				return character + ('A' - 'a');
			else
				return character;
		}

		template<std::size_t S>
		struct WideConverter
		{
			static std::string From(const wchar_t* wstr, std::size_t size)
			{
				if constexpr (S == 2)
				{
					// UTF-16 (Windows)
					return FromUtf16String(std::u16string_view(reinterpret_cast<const char16_t*>(wstr), size));
				}
				else if constexpr (S == 4)
				{
					// UTF-32 (Linux)
					return FromUtf32String(std::u32string_view(reinterpret_cast<const char32_t*>(wstr), size));
				}
				else
				{
					static_assert(AlwaysFalse<std::integral_constant<std::size_t, S>>::value, "Unsupported platform");
					return std::string("<platform error>");
				}
			}

			static std::wstring To(const std::string_view& str)
			{
				if constexpr (S == 2)
				{
					std::wstring result;
					utf8::utf8to16(str.begin(), str.end(), std::back_inserter(result));

					return result;
				}
				else if constexpr (S == 4)
				{
					std::wstring result;
					utf8::utf8to32(str.begin(), str.end(), std::back_inserter(result));

					return result;
				}
				else
				{
					static_assert(AlwaysFalse<std::integral_constant<std::size_t, S>>::value, "Unsupported platform");
					return std::wstring(L"<platform error>");
				}
			}
		};
	}

	std::string FromUtf16String(const char16_t* u16str)
	{
		std::size_t size = std::char_traits<char16_t>::length(u16str);
		return FromUtf16String(std::u16string_view(u16str, size));
	}

	std::string FromUtf16String(const std::u16string_view& u16str)
	{
		std::string result;
		utf8::utf16to8(u16str.begin(), u16str.end(), std::back_inserter(result));

		return result;
	}

	std::string FromUtf32String(const char32_t* u32str)
	{
		std::size_t size = std::char_traits<char32_t>::length(u32str);
		return FromUtf32String(std::u32string_view(u32str, size));
	}

	std::string FromUtf32String(const std::u32string_view& u32str)
	{
		std::string result;
		utf8::utf32to8(u32str.begin(), u32str.end(), std::back_inserter(result));

		return result;
	}

	std::string FromWideString(const wchar_t* wstr)
	{
		std::size_t size = std::char_traits<wchar_t>::length(wstr);
		return WideConverter<sizeof(wchar_t)>::From(wstr, size);
	}

	std::string FromWideString(const std::wstring_view& wstr)
	{
		return WideConverter<sizeof(wchar_t)>::From(wstr.data(), wstr.size());
	}

	bool MatchPattern(const std::string_view& str, const std::string_view& pattern)
	{
		if (str.empty() || pattern.empty())
			return false;

		// Par Jack Handy - akkhandy@hotmail.com
		// From : http://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
		const char* ptr = str.data();
		const char* ptrEnd = str.data() + str.size();

		const char* patternPtr = pattern.data();
		const char* patternPtrEnd = pattern.data() + pattern.size();

		while (ptr < ptrEnd && *patternPtr != '*')
		{
			if (patternPtr < patternPtrEnd && *patternPtr != *ptr && *patternPtr != '?')
				return false;

			patternPtr++;
			ptr++;
		}

		const char* cp = nullptr;
		const char* mp = nullptr;
		while (*ptr)
		{
			if (*patternPtr == '*')
			{
				if (patternPtr + 1 >= patternPtrEnd)
					return true;

				mp = ++patternPtr;
				cp = ptr + 1;
			}
			else if (*patternPtr == *ptr || *patternPtr == '?')
			{
				patternPtr++;
				ptr++;
			}
			else
			{
				patternPtr = mp;
				ptr = cp++;
			}
		}

		while (patternPtr < patternPtrEnd && *patternPtr == '*')
			patternPtr++;

		return patternPtr >= patternPtrEnd;
	}
	bool StartsWith(const std::string_view& str, const std::string_view& s, CaseIndependent)
	{
		if (s.size() > str.size())
			return false;

		return std::equal(str.begin(), str.begin() + s.size(), s.begin(), s.end(), [](char c1, char c2)
		{
			return ToLower(c1) == ToLower(c2);
		});
	}

	bool StartsWith(const std::string_view& str, const std::string_view& s, CaseIndependent, UnicodeAware)
	{
		if (str.empty() || s.empty())
			return str == s;

		utf8::iterator<const char*> it(str.data(), str.data(), str.data() + str.size());
		utf8::iterator<const char*> it2(s.data(), s.data(), s.data() + s.size());
		do
		{
			if (it2.base() >= s.data() + s.size())
				return true;

			if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
				return false;

			++it2;
		}
		while (*it++);

		return true;
	}

	std::string ToLower(const std::string_view& str)
	{
		std::string result;
		result.reserve(str.size());
		std::transform(str.begin(), str.end(), std::back_inserter(result), Overload<char>(ToLower));

		return result;
	}

	std::string ToLower(const std::string_view& str, UnicodeAware)
	{
		if (str.empty())
			return std::string();

		std::string result;
		result.reserve(str.size());

		utf8::unchecked::iterator<const char*> it(str.data());
		do
			utf8::append(Unicode::GetLowercase(*it), std::back_inserter(result));
		while (*++it);

		return result;
	}

	std::string ToUpper(const std::string_view& str)
	{
		std::string result;
		result.reserve(str.size());
		std::transform(str.begin(), str.end(), std::back_inserter(result), Overload<char>(ToUpper));

		return result;
	}

	std::string ToUpper(const std::string_view& str, UnicodeAware)
	{
		if (str.empty())
			return std::string();

		std::string result;
		result.reserve(str.size());

		utf8::iterator<const char*> it(str.data(), str.data(), str.data() + str.size());
		do
			utf8::append(Unicode::GetUppercase(*it), std::back_inserter(result));
		while (*++it);

		return result;
	}

	std::u16string ToUtf16String(const std::string_view& str)
	{
		std::u16string result;
		utf8::utf8to16(str.begin(), str.end(), std::back_inserter(result));

		return result;
	}

	std::u32string ToUtf32String(const std::string_view& str)
	{
		std::u32string result;
		utf8::utf8to32(str.begin(), str.end(), std::back_inserter(result));

		return result;
	}

	std::wstring ToWideString(const std::string_view& str)
	{
		return WideConverter<sizeof(wchar_t)>::To(str);
	}
}

#include <Nazara/Core/DebugOff.hpp>
