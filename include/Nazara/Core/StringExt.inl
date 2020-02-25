// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringExt.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool IsNumber(const char* str)
	{
		std::size_t size = std::strlen(str);
		return IsNumber(std::string_view(str, size));
	}

	bool IsNumber(const std::string_view& str)
	{
		return !str.empty() && std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
	}

	template<typename... Args> bool StartsWith(const std::string_view& str, const char* s, Args&&... args)
	{
		std::size_t size = std::strlen(s);
		return StartsWith(str, std::string_view(s, size), std::forward<Args>(args)...);
	}

	bool StartsWith(const std::string_view& str, const std::string_view& s)
	{
		//FIXME: Replace with proper C++20 value once it's available
#if __cplusplus > 201703L
		// C++20
		return str.starts_with(s);
#else
		return str.compare(0, s.size(), s.data()) == 0;
#endif
	}

	inline std::string ToLower(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToLower(std::string_view(str, size));
	}

	inline std::string ToLower(const char* str, UnicodeAware)
	{
		std::size_t size = std::strlen(str);
		return ToLower(std::string_view(str, size), UnicodeAware{});
	}

	inline std::string ToUpper(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToUpper(std::string_view(str, size));
	}

	inline std::string ToUpper(const char* str, UnicodeAware)
	{
		std::size_t size = std::strlen(str);
		return ToUpper(std::string_view(str, size), UnicodeAware{});
	}

	inline std::u16string ToUtf16String(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToUtf16String(std::string_view(str, size));
	}

	inline std::u32string ToUtf32String(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToUtf32String(std::string_view(str, size));
	}

	inline std::wstring ToWideString(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToWideString(std::string_view(str, size));
	}

}

#include <Nazara/Core/DebugOff.hpp>
