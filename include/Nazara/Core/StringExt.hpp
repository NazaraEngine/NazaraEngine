// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STRING_EXT_HPP
#define NAZARA_CORE_STRING_EXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <string>

namespace Nz
{
	struct CaseIndependent {};
	struct UnicodeAware {};

	// std::string is assumed to contains UTF-8
	NAZARA_CORE_API std::string FromUtf16String(const char16_t* u16str);
	NAZARA_CORE_API std::string FromUtf16String(const std::u16string_view& u16str);

	NAZARA_CORE_API std::string FromUtf32String(const char32_t* u32str);
	NAZARA_CORE_API std::string FromUtf32String(const std::u32string_view& u32str);

	NAZARA_CORE_API std::string FromWideString(const wchar_t* wstr);
	NAZARA_CORE_API std::string FromWideString(const std::wstring_view& str);

	inline bool IsNumber(const char* str);
	inline bool IsNumber(const std::string_view& str);

	template<typename... Args> bool StartsWith(const std::string_view& str, const char* s, Args&&... args);
	inline bool StartsWith(const std::string_view& str, const std::string_view& s);
	NAZARA_CORE_API bool StartsWith(const std::string_view& str, const std::string_view& s, CaseIndependent);
	NAZARA_CORE_API bool StartsWith(const std::string_view& str, const std::string_view& s, CaseIndependent, UnicodeAware);

	inline std::string ToLower(const char* str);
	NAZARA_CORE_API std::string ToLower(const std::string_view& str);

	inline std::string ToLower(const char* str, UnicodeAware);
	NAZARA_CORE_API std::string ToLower(const std::string_view& str, UnicodeAware);

	inline std::string ToUpper(const char* str);
	NAZARA_CORE_API std::string ToUpper(const std::string_view& str);

	inline std::string ToUpper(const char* str, UnicodeAware);
	NAZARA_CORE_API std::string ToUpper(const std::string_view& str, UnicodeAware);

	inline std::u16string ToUtf16String(const char* str);
	NAZARA_CORE_API std::u16string ToUtf16String(const std::string_view& str);

	inline std::u32string ToUtf32String(const char* str);
	NAZARA_CORE_API std::u32string ToUtf32String(const std::string_view& str);

	inline std::wstring ToWideString(const char* str);
	NAZARA_CORE_API std::wstring ToWideString(const std::string_view& str);
}

#include <Nazara/Core/StringExt.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
