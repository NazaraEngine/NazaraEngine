// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STRINGEXT_HPP
#define NAZARA_CORE_STRINGEXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Math/Algorithm.hpp> //< FIXME
#include <Nazara/Utils/Algorithm.hpp>
#include <string>

namespace Nz
{
	struct CaseIndependent {};
	struct UnicodeAware {};

	// std::string is assumed to contains UTF-8
	NAZARA_CORE_API std::size_t ComputeCharacterCount(const std::string_view& str);

	inline bool EndsWith(const std::string_view& str, const std::string_view& s);
	NAZARA_CORE_API bool EndsWith(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent);
	NAZARA_CORE_API bool EndsWith(const std::string_view& lhs, const std::string_view& rhs, UnicodeAware);
	NAZARA_CORE_API bool EndsWith(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent, UnicodeAware);

	NAZARA_CORE_API std::string FromUtf16String(const std::u16string_view& u16str);
	NAZARA_CORE_API std::string FromUtf32String(const std::u32string_view& u32str);
	NAZARA_CORE_API std::string FromWideString(const std::wstring_view& str);

	NAZARA_CORE_API std::size_t GetCharacterPosition(const std::string_view& str, std::size_t characterIndex);
	NAZARA_CORE_API std::string_view GetWord(const std::string_view& str, std::size_t wordIndex);
	NAZARA_CORE_API std::string_view GetWord(const std::string_view& str, std::size_t wordIndex, UnicodeAware);

	inline bool IsNumber(std::string_view str);

	NAZARA_CORE_API bool MatchPattern(const std::string_view& str, const std::string_view& pattern);

	NAZARA_CORE_API std::string PointerToString(const void* ptr);

	inline std::string& ReplaceStr(std::string& str, const std::string_view& from, const std::string_view& to);

	inline bool StartsWith(const std::string_view& str, const std::string_view& s);
	NAZARA_CORE_API bool StartsWith(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent);
	NAZARA_CORE_API bool StartsWith(const std::string_view& lhs, const std::string_view& rhs, UnicodeAware);
	NAZARA_CORE_API bool StartsWith(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent, UnicodeAware);

	template<typename F> bool SplitString(const std::string_view& str, const std::string_view& token, F&& func);
	template<typename F> bool SplitStringAny(const std::string_view& str, const std::string_view& token, F&& func);

	inline bool StringEqual(const std::string_view& lhs, const std::string_view& rhs);
	inline bool StringEqual(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent);
	NAZARA_CORE_API bool StringEqual(const std::string_view& lhs, const std::string_view& rhs, UnicodeAware);
	NAZARA_CORE_API bool StringEqual(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent, UnicodeAware);

	NAZARA_CORE_API std::string ToLower(const std::string_view& str);
	NAZARA_CORE_API std::string ToLower(const std::string_view& str, UnicodeAware);

	NAZARA_CORE_API std::string ToUpper(const std::string_view& str);
	NAZARA_CORE_API std::string ToUpper(const std::string_view& str, UnicodeAware);

	NAZARA_CORE_API std::u16string ToUtf16String(const std::string_view& str);
	NAZARA_CORE_API std::u32string ToUtf32String(const std::string_view& str);
	NAZARA_CORE_API std::wstring ToWideString(const std::string_view& str);

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
