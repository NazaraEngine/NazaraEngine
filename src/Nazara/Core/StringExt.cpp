// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/FunctionTraits.hpp>
#include <utf8cpp/utf8.h>
#include <cinttypes>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		bool IsSpace(char32_t character)
		{
			switch (character)
			{
				case '\f':
				case '\n':
				case '\r':
				case '\t':
				case '\v':
					return true;

				default:
					return Unicode::GetCategory(character) & Unicode::Category_Separator;
			}
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
			static_assert(AlwaysFalse<std::integral_constant<std::size_t, S>>(), "unsupported platform");
		};

#ifdef NAZARA_PLATFORM_WINDOWS
		template<>
		struct WideConverter<2>
		{
			// UTF-16 (Windows)
			static std::string From(const wchar_t* wstr, std::size_t size)
			{
				return FromUtf16String(std::u16string_view(reinterpret_cast<const char16_t*>(wstr), size));
			}

			static std::wstring To(std::string_view str)
			{
				std::wstring result;
				utf8::utf8to16(str.begin(), str.end(), std::back_inserter(result));

				return result;
			}
		};
#endif

#ifndef NAZARA_PLATFORM_WINDOWS
		template<>
		struct WideConverter<4>
		{
			// UTF-32 (POSIX)
			static std::string From(const wchar_t* wstr, std::size_t size)
			{
				return FromUtf32String(std::u32string_view(reinterpret_cast<const char32_t*>(wstr), size));
			}

			static std::wstring To(std::string_view str)
			{
				std::wstring result;
				utf8::utf8to32(str.begin(), str.end(), std::back_inserter(result));

				return result;
			}
		};
#endif
	}

	std::size_t ComputeCharacterCount(std::string_view str)
	{
		return utf8::distance(str.data(), str.data() + str.size());
	}
	
	bool EndsWith(std::string_view lhs, std::string_view rhs, CaseIndependent)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (rhs.size() > lhs.size())
			return false;

		return std::equal(lhs.end() - rhs.size(), lhs.end(), rhs.begin(), rhs.end(), [](char c1, char c2)
		{
			return ToLower(c1) == ToLower(c2);
		});
	}

	bool EndsWith(std::string_view lhs, std::string_view rhs, UnicodeAware)
	{
		if (lhs.empty())
			return lhs == rhs;
		else if (rhs.empty())
			return true;
		else if (rhs.size() > lhs.size())
			return false;

		utf8::iterator<const char*> it(lhs.data() + lhs.size() - rhs.size(), lhs.data() + lhs.size() - rhs.size(), lhs.data() + lhs.size());
		utf8::iterator<const char*> it2(rhs.data(), rhs.data(), rhs.data() + rhs.size());
		do
		{
			if (it2.base() >= rhs.data() + rhs.size())
				return true;

			if (*it != *it2)
				return false;

			++it2;
		}
		while (*it++);

		return true;
	}

	bool EndsWith(std::string_view lhs, std::string_view rhs, CaseIndependent, UnicodeAware)
	{
		if (lhs.empty())
			return lhs == rhs;
		else if (rhs.empty())
			return true;
		else if (rhs.size() > lhs.size())
			return false;

		utf8::iterator<const char*> it(lhs.data() + lhs.size() - rhs.size(), lhs.data() + lhs.size() - rhs.size(), lhs.data() + lhs.size());
		utf8::iterator<const char*> it2(rhs.data(), rhs.data(), rhs.data() + rhs.size());
		do
		{
			if (it2.base() >= rhs.data() + rhs.size())
				return true;

			if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
				return false;

			++it2;
		}
		while (*it++);

		return true;
	}

	std::string FromUtf16String(std::u16string_view u16str)
	{
		std::string result;
		utf8::utf16to8(u16str.begin(), u16str.end(), std::back_inserter(result));

		return result;
	}

	std::string FromUtf32String(std::u32string_view u32str)
	{
		std::string result;
		utf8::utf32to8(u32str.begin(), u32str.end(), std::back_inserter(result));

		return result;
	}

	std::string FromWideString(std::wstring_view wstr)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return WideConverter<sizeof(wchar_t)>::From(wstr.data(), wstr.size());
	}

	std::size_t GetCharacterPosition(std::string_view str, std::size_t characterIndex)
	{
		const char* ptr = str.data();
		const char* end = ptr + str.size();

		try
		{
			utf8::advance(ptr, characterIndex, end);

			return ptr - str.data();
		}
		catch (utf8::not_enough_room& /*e*/)
		{
			// Returns npos
		}
		catch (utf8::exception& e)
		{
			NazaraErrorFmt("UTF-8 error: {0}", e.what());
		}
		catch (std::exception& e)
		{
			NazaraError(e.what());
		}

		return std::string::npos;
	}

	std::string_view GetWord(std::string_view str, std::size_t wordIndex)
	{
		std::size_t pos = 0;
		std::size_t previousPos = 0;
		while ((pos = str.find_first_of(" \f\n\r\t\v", previousPos)) != std::string::npos)
		{
			std::size_t splitPos = previousPos;
			previousPos = pos + 1;

			if (pos != splitPos && wordIndex-- == 0)
				return str.substr(splitPos, pos - splitPos);
		}

		return {};
	}

	std::string_view GetWord(std::string_view str, std::size_t wordIndex, UnicodeAware)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());

		auto FindNextSeparator = [&]() -> std::size_t
		{
			for (; it != end; ++it)
			{
				if (IsSpace(*it))
					return true;
			}

			return false;
		};

		utf8::unchecked::iterator<const char*> lastSplit = it;
		while (FindNextSeparator())
		{
			if (it != lastSplit && wordIndex-- == 0)
				return std::string_view(lastSplit.base(), it.base() - lastSplit.base());

			++it;
			lastSplit = it;
		}

		return {};
	}

	void IterateOnCodepoints(std::string_view str, FunctionRef<bool(const char32_t* characters, std::size_t characterCount)> callback)
	{
		std::array<char32_t, 32> buffer;
		std::size_t charCount = 0;

		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());
		for (; it != end; ++it)
		{
			buffer[charCount++] = *it;
			if (charCount == buffer.size())
			{
				if (!callback(&buffer[0], charCount))
					return;

				charCount = 0;
			}
		}

		if (charCount != 0)
			callback(&buffer[0], charCount);
	}

	bool MatchPattern(std::string_view str, std::string_view pattern)
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
			if (patternPtr >= patternPtrEnd || (*patternPtr != *ptr && *patternPtr != '?'))
				return false;

			patternPtr++;
			ptr++;
		}

		const char* cp = nullptr;
		const char* mp = nullptr;
		while (ptr < ptrEnd)
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

	std::string PointerToString(const void* ptr)
	{
		constexpr int width = static_cast<int>(sizeof(uintptr_t) * 2);

		std::string str(width + 2, '\0');
		str.resize(std::sprintf(str.data(), "0x%0*" PRIXPTR, width, reinterpret_cast<uintptr_t>(ptr)));

		return str;
	}

	bool StartsWith(std::string_view lhs, std::string_view rhs, CaseIndependent)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (rhs.size() > lhs.size())
			return false;

		return std::equal(lhs.begin(), lhs.begin() + rhs.size(), rhs.begin(), rhs.end(), [](char c1, char c2)
		{
			return ToLower(c1) == ToLower(c2);
		});
	}

	bool StartsWith(std::string_view lhs, std::string_view rhs, UnicodeAware)
	{
		if (lhs.empty())
			return lhs == rhs;
		else if (rhs.empty())
			return true;

		utf8::iterator<const char*> it(lhs.data(), lhs.data(), lhs.data() + lhs.size());
		utf8::iterator<const char*> it2(rhs.data(), rhs.data(), rhs.data() + rhs.size());
		do
		{
			if (it2.base() >= rhs.data() + rhs.size())
				return true;

			if (*it != *it2)
				return false;

			++it2;
		}
		while (*it++);

		return true;
	}

	bool StartsWith(std::string_view lhs, std::string_view rhs, CaseIndependent, UnicodeAware)
	{
		if (lhs.empty())
			return lhs == rhs;
		else if (rhs.empty())
			return true;

		utf8::iterator<const char*> it(lhs.data(), lhs.data(), lhs.data() + lhs.size());
		utf8::iterator<const char*> it2(rhs.data(), rhs.data(), rhs.data() + rhs.size());
		do
		{
			if (it2.base() >= rhs.data() + rhs.size())
				return true;

			if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
				return false;

			++it2;
		}
		while (*it++);

		return true;
	}

	bool StringEqual(std::string_view lhs, std::string_view rhs, UnicodeAware)
	{
		if (lhs.empty() || rhs.empty())
			return lhs == rhs;

		utf8::iterator<const char*> it(lhs.data(), lhs.data(), lhs.data() + lhs.size());
		utf8::iterator<const char*> it2(rhs.data(), rhs.data(), rhs.data() + rhs.size());

		for (; it.base() < lhs.data() + lhs.size(); ++it, ++it2)
		{
			if (*it != *it2)
				return false;
		}

		return true;
	}

	bool StringEqual(std::string_view lhs, std::string_view rhs, CaseIndependent, UnicodeAware)
	{
		if (lhs.empty() || rhs.empty())
			return lhs == rhs;

		utf8::iterator<const char*> it(lhs.data(), lhs.data(), lhs.data() + lhs.size());
		utf8::iterator<const char*> it2(rhs.data(), rhs.data(), rhs.data() + rhs.size());

		for (; it.base() < lhs.data() + lhs.size(); ++it, ++it2)
		{
			if (Unicode::GetLowercase(*it) != Unicode::GetLowercase(*it2))
				return false;
		}

		return true;
	}

	std::string ToLower(std::string_view str)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::string result;
		result.reserve(str.size());
		std::transform(str.begin(), str.end(), std::back_inserter(result), Overload<char>(ToLower));

		return result;
	}

	std::string ToLower(std::string_view str, UnicodeAware)
	{
		if (str.empty())
			return std::string();

		std::string result;
		result.reserve(str.size());

		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());
		for (; it != end; ++it)
			utf8::append(Unicode::GetLowercase(*it), std::back_inserter(result));

		return result;
	}

	std::string ToUpper(std::string_view str)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::string result;
		result.reserve(str.size());
		std::transform(str.begin(), str.end(), std::back_inserter(result), Overload<char>(ToUpper));

		return result;
	}

	std::string ToUpper(std::string_view str, UnicodeAware)
	{
		if (str.empty())
			return std::string();

		std::string result;
		result.reserve(str.size());

		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());
		for (; it != end; ++it)
			utf8::append(Unicode::GetUppercase(*it), std::back_inserter(result));

		return result;
	}

	std::u16string ToUtf16String(std::string_view str)
	{
		std::u16string result;
		utf8::utf8to16(str.begin(), str.end(), std::back_inserter(result));

		return result;
	}

	std::u32string ToUtf32String(std::string_view str)
	{
		std::u32string result;
		utf8::utf8to32(str.begin(), str.end(), std::back_inserter(result));

		return result;
	}

	std::wstring ToWideString(std::string_view str)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return WideConverter<sizeof(wchar_t)>::To(str);
	}

	std::string_view TrimLeft(std::string_view str)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		while (!str.empty() && IsSpace(str.front()))
			str.remove_prefix(1);

		return str;
	}

	std::string_view TrimLeft(std::string_view str, UnicodeAware)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());
		while (it != end && IsSpace(*it))
			++it;

		return std::string_view(it.base(), end.base() - it.base());
	}

	std::string_view TrimLeft(std::string_view str, char32_t c, UnicodeAware)
	{
		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());
		while (it != end && *it == c)
			++it;

		return std::string_view(it.base(), end.base() - it.base());
	}

	std::string_view TrimLeft(std::string_view str, char32_t c, CaseIndependent, UnicodeAware)
	{
		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());

		c = Unicode::GetLowercase(c);

		while (it != end && Unicode::GetLowercase(*it) == c)
			++it;

		return std::string_view(it.base(), end.base() - it.base());
	}

	std::string_view TrimLeft(std::string_view str, Unicode::Category category, UnicodeAware)
	{
		utf8::unchecked::iterator<const char*> it(str.data());
		utf8::unchecked::iterator<const char*> end(str.data() + str.size());
		while (it != end && (Unicode::GetCategory(*it) & category) == category)
			++it;

		return std::string_view(it.base(), end.base() - it.base());
	}

	std::string_view TrimRight(std::string_view str)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		while (!str.empty() && IsSpace(str.back()))
			str.remove_suffix(1);

		return str;
	}

	std::string_view TrimRight(std::string_view str, UnicodeAware)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (str.empty())
			return str;

		// Find last character head
		const char* lastCharacter = str.data() + str.size() - 1;
		while (utf8::internal::is_trail(*lastCharacter) && lastCharacter != str.data())
			--lastCharacter;

		utf8::unchecked::iterator<const char*> start(str.data());
		utf8::unchecked::iterator<const char*> it(lastCharacter);

		while (it != start && IsSpace(*it))
			--it;

		++it;

		return std::string_view(start.base(), it.base() - start.base());
	}

	std::string_view TrimRight(std::string_view str, char32_t c, UnicodeAware)
	{
		if (str.empty())
			return str;

		// Find last character head
		const char* lastCharacter = str.data() + str.size() - 1;
		while (utf8::internal::is_trail(*lastCharacter) && lastCharacter != str.data())
			--lastCharacter;

		utf8::unchecked::iterator<const char*> start(str.data());
		utf8::unchecked::iterator<const char*> it(lastCharacter);

		while (it != start && *it == c)
			--it;

		++it;

		return std::string_view(start.base(), it.base() - start.base());
	}

	std::string_view TrimRight(std::string_view str, char32_t c, CaseIndependent, UnicodeAware)
	{
		if (str.empty())
			return str;

		// Find last character head
		const char* lastCharacter = str.data() + str.size() - 1;
		while (utf8::internal::is_trail(*lastCharacter) && lastCharacter != str.data())
			--lastCharacter;

		utf8::unchecked::iterator<const char*> start(str.data());
		utf8::unchecked::iterator<const char*> it(lastCharacter);

		c = Unicode::GetLowercase(c);

		while (it != start && Unicode::GetLowercase(*it) == c)
			--it;

		++it;

		return std::string_view(start.base(), it.base() - start.base());
	}

	std::string_view TrimRight(std::string_view str, Unicode::Category category, UnicodeAware)
	{
		if (str.empty())
			return str;

		// Find last character head
		const char* lastCharacter = str.data() + str.size() - 1;
		while (utf8::internal::is_trail(*lastCharacter) && lastCharacter != str.data())
			--lastCharacter;

		utf8::unchecked::iterator<const char*> start(str.data());
		utf8::unchecked::iterator<const char*> it(lastCharacter);

		while (it != start && (Unicode::GetCategory(*it) & category) == category)
			--it;

		++it;

		return std::string_view(start.base(), it.base() - start.base());
	}
}
