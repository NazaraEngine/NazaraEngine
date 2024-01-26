// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline bool EndsWith(std::string_view str, std::string_view s)
	{
#if NAZARA_CPP_VER >= NAZARA_CPP20
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
		if (!str.empty() && str.front() == '-')
			str.remove_prefix(1);

		if (str.empty())
			return false;

		return std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
	}


	/*!
	* \ingroup math
	* \brief Converts the number to String
	* \return String representation of the number
	*
	* \param number Number to represent
	* \param radix Base of the number
	*
	* \remark radix is meant to be between 2 and 36, other values are potentially undefined behavior
	* \remark With NAZARA_MATH_SAFE, a NazaraError is produced and String() is returned
	*/
	inline std::string NumberToString(long long number, UInt8 radix)
	{
		NazaraAssert(radix >= 2 && radix <= 36, "base must be between 2 and 36");

		bool negative;
		if (number < 0)
		{
			negative = true;
			number = -number;
		}
		else
			negative = false;

		std::string str;

		constexpr char symbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		do
		{
			str.push_back(symbols[number % radix]);
			number /= radix;
		} while (number > 0);

		if (negative)
			str.push_back('-');

		std::reverse(str.begin(), str.end());

		return str;
	}

	template<typename T>
	std::basic_string<T>& ReplaceStr(std::basic_string<T>& str, T from, T to)
	{
		std::size_t startPos = 0;
		while ((startPos = str.find(from, startPos)) != std::string::npos)
		{
			str[startPos] = to;
			startPos++;
		}

		return str;
	}

	template<typename T>
	std::basic_string<T>& ReplaceStr(std::basic_string<T>& str, const T* from, const T* to)
	{
		return ReplaceStr(str, std::basic_string_view<T>(from), std::basic_string_view<T>(to));
	}

	template<typename T>
	std::basic_string<T>& ReplaceStr(std::basic_string<T>& str, std::basic_string_view<T> from, std::basic_string_view<T> to)
	{
		std::size_t startPos = 0;
		while ((startPos = str.find(from, startPos)) != std::string::npos)
		{
			str.replace(startPos, from.length(), to);
			startPos += to.length();
		}

		return str;
	}

	std::string_view Substring(std::string_view str, std::size_t index, UnicodeAware)
	{
		return Substring(str, index, std::numeric_limits<std::size_t>::max(), UnicodeAware{});
	}

	inline bool StartsWith(std::string_view str, std::string_view s)
	{
#if NAZARA_CPP_VER >= NAZARA_CPP20
		// C++20
		return str.starts_with(s);
#else
		return str.compare(0, s.size(), s.data()) == 0;
#endif
	}

	template<typename F>
	bool SplitString(std::string_view str, std::string_view token, F&& func)
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
	bool SplitStringAny(std::string_view str, std::string_view token, F&& func)
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

	inline bool StringEqual(std::string_view lhs, std::string_view rhs)
	{
		return lhs == rhs;
	}

	inline bool StringEqual(std::string_view lhs, std::string_view rhs, CaseIndependent)
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

	/*!
	* \ingroup math
	* \brief Converts the string to number
	* \return Number which is represented by the string
	*
	* \param str String representation
	* \param radix Base of the number
	* \param ok Optional argument to know if convertion is correct
	*
	* \remark radix is meant to be between 2 and 36, other values are potentially undefined behavior
	* \remark With NAZARA_MATH_SAFE, a NazaraError is produced and 0 is returned
	*/
	inline long long StringToNumber(std::string_view str, UInt8 radix, bool* ok)
	{
		NazaraAssert(radix >= 2 && radix <= 36, "base must be between 2 and 36");

		if (str.empty())
		{
			if (ok)
				*ok = false;

			return 0;
		}

		constexpr char symbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		bool negative = (str.front() == '-');

		const char* digit = &str[(negative) ? 1 : 0];
		unsigned long long total = 0;
		do
		{
			if (*digit == ' ')
				continue;

			total *= radix;
			const char* c = std::strchr(symbols, *digit);
			if (c && c - symbols < radix)
				total += c - symbols;
			else
			{
				if (ok)
					*ok = false;

				return 0;
			}
		} while (*++digit);

		if (ok)
			*ok = true;

		return (negative) ? -static_cast<long long>(total) : total;
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
