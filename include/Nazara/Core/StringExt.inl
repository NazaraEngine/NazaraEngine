// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringExt.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline bool EndsWith(const std::string_view& str, const std::string_view& s)
	{
		//FIXME: Replace with proper C++20 value once it's available
#if NAZARA_CPP_VER > 201703L
		// C++20
		return str.ends_with(s);
#else
		if (s.size() > str.size())
			return false;

		return str.compare(str.size() - s.size(), s.size(), s.data()) == 0;
#endif
	}

	inline bool IsNumber(std::string_view str)
	{
		if (str.empty())
			return false;

		if (str.front() == '-')
			str.remove_prefix(1);

		return std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
	}

	inline std::string& ReplaceStr(std::string& str, const std::string_view& from, const std::string_view& to)
	{
		if (str.empty())
			return str;

		std::size_t startPos = 0;
		while ((startPos = str.find(from, startPos)) != std::string::npos)
		{
			str.replace(startPos, from.length(), to);
			startPos += to.length();
		}

		return str;
	}

	inline bool StartsWith(const std::string_view& str, const std::string_view& s)
	{
		//FIXME: Replace with proper C++20 value once it's available
#if NAZARA_CPP_VER > 201703L
		// C++20
		return str.starts_with(s);
#else
		return str.compare(0, s.size(), s.data()) == 0;
#endif
	}

	template<typename F>
	bool SplitString(const std::string_view& str, const std::string_view& token, F&& func)
	{
		std::size_t pos = 0;
		std::size_t previousPos = 0;
		while ((pos = str.find(token, previousPos)) != std::string::npos)
		{
			std::size_t splitPos = previousPos;
			previousPos = pos + token.size();

			if (!func(str.substr(splitPos, pos - splitPos)))
				return false;
		}

		if (previousPos < str.size())
			return func(str.substr(previousPos));
		else
			return true;
	}

	template<typename F>
	bool SplitStringAny(const std::string_view& str, const std::string_view& token, F&& func)
	{
		std::size_t pos = 0;
		std::size_t previousPos = 0;
		while ((pos = str.find_first_of(token, previousPos)) != std::string::npos)
		{
			std::size_t splitPos = previousPos;
			previousPos = pos + 1;

			if (!func(str.substr(splitPos, pos - splitPos)))
				return false;
		}

		return func(str.substr(previousPos));
	}

	inline bool StringEqual(const std::string_view& lhs, const std::string_view& rhs)
	{
		return lhs == rhs;
	}

	inline bool StringEqual(const std::string_view& lhs, const std::string_view& rhs, CaseIndependent)
	{
		if (lhs.size() != rhs.size())
			return false;

		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			if (std::tolower(lhs[i]) != std::tolower(rhs[i]))
				return false;
		}

		return true;
	}

	inline std::string_view Trim(std::string_view str)
	{
		return TrimRight(TrimLeft(str));
	}

	inline std::string_view Trim(std::string_view str, char c)
	{
		return TrimRight(TrimLeft(str, c), c);
	}

	inline std::string_view Trim(std::string_view str, char c, CaseIndependent)
	{
		return TrimRight(TrimLeft(str, c, CaseIndependent{}), c, CaseIndependent{});
	}

	inline std::string_view Trim(std::string_view str, Unicode::Category category)
	{
		return TrimRight(TrimLeft(str, category), category);
	}

	inline std::string_view Trim(std::string_view str, UnicodeAware)
	{
		return TrimRight(TrimLeft(str, UnicodeAware{}), UnicodeAware{});
	}

	inline std::string_view Trim(std::string_view str, char32_t c, UnicodeAware)
	{
		return TrimRight(TrimLeft(str, c, UnicodeAware{}), c, UnicodeAware{});
	}

	inline std::string_view Trim(std::string_view str, char32_t c, CaseIndependent, UnicodeAware)
	{
		return TrimRight(TrimLeft(str, c, CaseIndependent{}, UnicodeAware{}), c, CaseIndependent{}, UnicodeAware{});
	}

	inline std::string_view Trim(std::string_view str, Unicode::Category category, UnicodeAware)
	{
		return TrimRight(TrimLeft(str, category, UnicodeAware{}), category, UnicodeAware{});
	}

	inline std::string_view TrimLeft(std::string_view str, char c)
	{
		while (!str.empty() && str.front() == c)
			str.remove_prefix(1);

		return str;
	}

	inline std::string_view TrimLeft(std::string_view str, char c, CaseIndependent)
	{
		c = char(std::tolower(c));

		while (!str.empty() && std::tolower(str.front()) == c)
			str.remove_prefix(1);

		return str;
	}

	inline std::string_view TrimLeft(std::string_view str, Unicode::Category category)
	{
		while (!str.empty() && (Unicode::GetCategory(str.front()) & category) == category)
			str.remove_prefix(1);

		return str;
	}

	inline std::string_view TrimRight(std::string_view str, char c)
	{
		while (!str.empty() && str.back() == c)
			str.remove_suffix(1);

		return str;
	}

	inline std::string_view TrimRight(std::string_view str, char c, CaseIndependent)
	{
		c = char(std::tolower(c));

		while (!str.empty() && std::tolower(str.back()) == c)
			str.remove_suffix(1);

		return str;
	}

	inline std::string_view TrimRight(std::string_view str, Unicode::Category category)
	{
		while (!str.empty() && (Unicode::GetCategory(str.back()) & category) == category)
			str.remove_suffix(1);

		return str;
	}
}

#include <Nazara/Core/DebugOff.hpp>
