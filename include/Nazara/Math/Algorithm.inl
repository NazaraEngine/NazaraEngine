// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Config.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup math
	* \brief Clamps an angle value between min and max and returns the expected value
	* \return If value is not in the interval of min..max, value obtained is the nearest limit of this interval
	*
	* \param value Value to clamp
	* \param min Minimum of the interval
	* \param max Maximum of the interval
	*/
	template<typename T, AngleUnit Unit>
	constexpr Angle<Unit, T> Clamp(Angle<Unit, T> value, T min, T max)
	{
		return std::max(std::min(value.value, max), min);
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits
	*
	* \param number Number to get number of digits
	*/
	constexpr inline unsigned int GetNumberLength(signed char number)
	{
		// Char is expected to be 1 byte
		static_assert(sizeof(number) == 1, "Signed char must be one byte-sized");

		if (number >= 100)
			return 3;
		else if (number >= 10)
			return 2;
		else if (number >= 0)
			return 1;
		else if (number > -10)
			return 2;
		else if (number > -100)
			return 3;
		else
			return 4;
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits
	*
	* \param number Number to get number of digits
	*/
	constexpr inline unsigned int GetNumberLength(unsigned char number)
	{
		// Char is expected to be 1 byte
		static_assert(sizeof(number) == 1, "Unsigned char must be one byte-sized");

		if (number >= 100)
			return 3;
		else if (number >= 10)
			return 2;
		else
			return 1;
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits
	*
	* \param number Number to get number of digits
	*/
	inline unsigned int GetNumberLength(int number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(std::abs(number))) + (number < 0 ? 2 : 1);
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits
	*
	* \param number Number to get number of digits
	*/
	//TODO: Mark as constexpr when supported by all major compilers
	/*constexpr*/ inline unsigned int GetNumberLength(unsigned int number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(number))+1;
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits
	*
	* \param number Number to get number of digits
	*/
	inline unsigned int GetNumberLength(long long number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(std::abs(number))) + (number < 0 ? 2 : 1);
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits
	*
	* \param number Number to get number of digits
	*/
	//TODO: Mark as constexpr when supported by all major compilers
	/*constexpr*/ inline unsigned int GetNumberLength(unsigned long long number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(number)) + 1;
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits + 1 for the dot
	*
	* \param number Number to get number of digits
	* \param precision Number of digit after the dot
	*/
	inline unsigned int GetNumberLength(float number, UInt8 precision)
	{
		// The imprecision of floats need a cast (log10(9.99999) = 0.99999)
		return GetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus one for the dot
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits + 1 for the dot
	*
	* \param number Number to get number of digits
	* \param precision Number of digit after the dot
	*/
	inline unsigned int GetNumberLength(double number, UInt8 precision)
	{
		// The imprecision of floats need a cast (log10(9.99999) = 0.99999)
		return GetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus one for the dot
	}

	/*!
	* \ingroup math
	* \brief Gets the number of digits to represent the number in base 10
	* \return Number of digits + 1 for the dot
	*
	* \param number Number to get number of digits
	* \param precision Number of digit after the dot
	*/
	inline unsigned int GetNumberLength(long double number, UInt8 precision)
	{
		// The imprecision of floats need a cast (log10(9.99999) = 0.99999)
		return GetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus one for the dot
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
		#if NAZARA_MATH_SAFE
		if (radix < 2 || radix > 36)
		{
			NazaraError("Base must be between 2 and 36");
			return {};
		}
		#endif

		if (number == 0)
			return "0";

		bool negative;
		if (number < 0)
		{
			negative = true;
			number = -number;
		}
		else
			negative = false;

		std::string str;

		const char symbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		do
		{
			str.push_back(symbols[number % radix]);
			number /= radix;
		}
		while (number > 0);

		if (negative)
			str.push_back('-');

		std::reverse(str.begin(), str.end());

		return str;
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
	inline long long StringToNumber(const std::string_view& str, UInt8 radix, bool* ok)
	{
		#if NAZARA_MATH_SAFE
		if (radix < 2 || radix > 36)
		{
			NazaraError("Radix must be between 2 and 36");

			if (ok)
				*ok = false;

			return 0;
		}
		#endif

		if (str.empty())
		{
			if (ok)
				*ok = false;

			return 0;
		}

		const char symbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		bool negative = (str.front() == '-');

		const char* digit = &str[(negative) ? 1 : 0];
		unsigned long long total = 0;
		do
		{
			if (*digit == ' ')
				continue;

			total *= radix;
			const char* c = std::strchr(symbols, *digit);
			if (c && c-symbols < radix)
				total += c-symbols;
			else
			{
				if (ok)
					*ok = false;

				return 0;
			}
		}
		while (*++digit);

		if (ok)
			*ok = true;

		return (negative) ? -static_cast<long long>(total) : total;
	}
}

#include <Nazara/Core/DebugOff.hpp>
