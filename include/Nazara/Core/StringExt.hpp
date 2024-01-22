// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STRINGEXT_HPP
#define NAZARA_CORE_STRINGEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <string>

namespace Nz
{
	struct CaseIndependent {};
	struct UnicodeAware {};

	// std::string is assumed to contains UTF-8
	NAZARA_CORE_API std::size_t ComputeCharacterCount(std::string_view str);

	inline bool EndsWith(std::string_view str, std::string_view s);
	NAZARA_CORE_API bool EndsWith(std::string_view lhs, std::string_view rhs, CaseIndependent);
	NAZARA_CORE_API bool EndsWith(std::string_view lhs, std::string_view rhs, UnicodeAware);
	NAZARA_CORE_API bool EndsWith(std::string_view lhs, std::string_view rhs, CaseIndependent, UnicodeAware);

	NAZARA_CORE_API std::string FromUtf16String(std::u16string_view u16str);
	NAZARA_CORE_API std::string FromUtf32String(std::u32string_view u32str);
	NAZARA_CORE_API std::string FromWideString(std::wstring_view str);

	NAZARA_CORE_API std::size_t GetCharacterPosition(std::string_view str, std::size_t characterIndex);
	NAZARA_CORE_API std::string_view GetWord(std::string_view str, std::size_t wordIndex);
	NAZARA_CORE_API std::string_view GetWord(std::string_view str, std::size_t wordIndex, UnicodeAware);

	inline bool IsNumber(std::string_view str);

	NAZARA_CORE_API void IterateOnCodepoints(std::string_view str, FunctionRef<bool(std::u32string_view characters)> callback);

	NAZARA_CORE_API bool MatchPattern(std::string_view str, std::string_view pattern);

	inline std::string NumberToString(long long number, UInt8 radix = 10);

	NAZARA_CORE_API std::string PointerToString(const void* ptr);

	inline std::string& ReplaceStr(std::string& str, std::string_view from, std::string_view to);

	inline bool StartsWith(std::string_view str, std::string_view s);
	NAZARA_CORE_API bool StartsWith(std::string_view lhs, std::string_view rhs, CaseIndependent);
	NAZARA_CORE_API bool StartsWith(std::string_view lhs, std::string_view rhs, UnicodeAware);
	NAZARA_CORE_API bool StartsWith(std::string_view lhs, std::string_view rhs, CaseIndependent, UnicodeAware);

	template<typename F> bool SplitString(std::string_view str, std::string_view token, F&& func);
	template<typename F> bool SplitStringAny(std::string_view str, std::string_view token, F&& func);

	inline bool StringEqual(std::string_view lhs, std::string_view rhs);
	inline bool StringEqual(std::string_view lhs, std::string_view rhs, CaseIndependent);
	NAZARA_CORE_API bool StringEqual(std::string_view lhs, std::string_view rhs, UnicodeAware);
	NAZARA_CORE_API bool StringEqual(std::string_view lhs, std::string_view rhs, CaseIndependent, UnicodeAware);

	inline long long StringToNumber(std::string_view str, UInt8 radix = 10, bool* ok = nullptr);

	NAZARA_CORE_API std::string ToLower(std::string_view str);
	NAZARA_CORE_API std::string ToLower(std::string_view str, UnicodeAware);

	NAZARA_CORE_API std::string ToUpper(std::string_view str);
	NAZARA_CORE_API std::string ToUpper(std::string_view str, UnicodeAware);

	NAZARA_CORE_API std::u16string ToUtf16String(std::string_view str);
	NAZARA_CORE_API std::u32string ToUtf32String(std::string_view str);
	NAZARA_CORE_API std::wstring ToWideString(std::string_view str);

	inline std::string_view Trim(std::string_view str);
	inline std::string_view Trim(std::string_view str, char c);
	inline std::string_view Trim(std::string_view str, char c, CaseIndependent);
	inline std::string_view Trim(std::string_view str, Unicode::Category category);
	inline std::string_view Trim(std::string_view str, UnicodeAware);
	inline std::string_view Trim(std::string_view str, char32_t c, UnicodeAware);
	inline std::string_view Trim(std::string_view str, char32_t c, CaseIndependent, UnicodeAware);
	inline std::string_view Trim(std::string_view str, Unicode::Category category, UnicodeAware);

	NAZARA_CORE_API std::string_view TrimLeft(std::string_view str);
	inline std::string_view TrimLeft(std::string_view str, char c);
	inline std::string_view TrimLeft(std::string_view str, char c, CaseIndependent);
	inline std::string_view TrimLeft(std::string_view str, Unicode::Category category);
	NAZARA_CORE_API std::string_view TrimLeft(std::string_view str, UnicodeAware);
	NAZARA_CORE_API std::string_view TrimLeft(std::string_view str, char32_t c, UnicodeAware);
	NAZARA_CORE_API std::string_view TrimLeft(std::string_view str, char32_t c, CaseIndependent, UnicodeAware);
	NAZARA_CORE_API std::string_view TrimLeft(std::string_view str, Unicode::Category category, UnicodeAware);

	NAZARA_CORE_API std::string_view TrimRight(std::string_view str);
	inline std::string_view TrimRight(std::string_view str, char c);
	inline std::string_view TrimRight(std::string_view str, char c, CaseIndependent);
	inline std::string_view TrimRight(std::string_view str, Unicode::Category category);
	NAZARA_CORE_API std::string_view TrimRight(std::string_view str, UnicodeAware);
	NAZARA_CORE_API std::string_view TrimRight(std::string_view str, char32_t c, UnicodeAware);
	NAZARA_CORE_API std::string_view TrimRight(std::string_view str, char32_t c, CaseIndependent, UnicodeAware);
	NAZARA_CORE_API std::string_view TrimRight(std::string_view str, Unicode::Category category, UnicodeAware);
}

#include <Nazara/Core/StringExt.inl>

#endif // NAZARA_CORE_STRINGEXT_HPP
