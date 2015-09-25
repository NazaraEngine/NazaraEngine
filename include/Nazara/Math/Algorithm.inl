// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Config.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		namespace
		{
			// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
			static const unsigned int MultiplyDeBruijnBitPosition[32] =
			{
				0,  9,  1, 10, 13, 21,  2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
				8, 12, 20, 28, 15, 17, 24,  7, 19, 27, 23,  6, 26,  5, 4, 31
			};

			static const unsigned int MultiplyDeBruijnBitPosition2[32] =
			{
				 0,  1, 28,  2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17,  4, 8,
				31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18,  6, 11,  5, 10, 9
			};
		}

		template<typename T>
		typename std::enable_if<sizeof(T) <= sizeof(UInt32), unsigned int>::type IntegralLog2(T number)
		{
			// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
			number |= number >> 1; // first round down to one less than a power of 2
			number |= number >> 2;
			number |= number >> 4;
			number |= number >> 8;
			number |= number >> 16;

			return MultiplyDeBruijnBitPosition[static_cast<UInt32>(number * 0x07C4ACDDU) >> 27];
		}

		template<typename T>
		// Les parenthèses autour de la condition sont nécesaires pour que GCC compile ça
		typename std::enable_if<(sizeof(T) > sizeof(UInt32)), unsigned int>::type IntegralLog2(T number)
		{
			static_assert(sizeof(T) % sizeof(UInt32) == 0, "Assertion failed");

			// L'algorithme pour le logarithme base 2 (au dessus) ne fonctionne qu'avec des nombres au plus 32bits
			// ce code décompose les nombres plus grands en nombres 32 bits par masquage et bit shifting
			for (int i = sizeof(T)-sizeof(UInt32); i >= 0; i -= sizeof(UInt32))
			{
				// Le masque 32 bits sur la partie du nombre qu'on traite actuellement
				T mask = T(std::numeric_limits<UInt32>::max()) << i*8;
				T val = (number & mask) >> i*8; // Masquage et shifting des bits vers la droite (pour le ramener sur 32bits)

				// Appel de la fonction avec le nombre 32bits, si le résultat est non-nul nous avons la réponse
				unsigned int log2 = IntegralLog2<UInt32>(val);
				if (log2)
					return log2 + i*8;
			}

			return 0;
		}

		template<typename T>
		typename std::enable_if<sizeof(T) <= sizeof(UInt32), unsigned int>::type IntegralLog2Pot(T number)
		{
			// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
			return MultiplyDeBruijnBitPosition2[static_cast<UInt32>(number * 0x077CB531U) >> 27];
		}

		template<typename T>
		// Les parenthèses autour de la condition sont nécesaires pour que GCC compile ça
		typename std::enable_if<(sizeof(T) > sizeof(UInt32)), unsigned int>::type IntegralLog2Pot(T number)
		{
			static_assert(sizeof(T) % sizeof(UInt32) == 0, "Assertion failed");

			// L'algorithme pour le logarithme base 2 (au dessus) ne fonctionne qu'avec des nombres au plus 32bits
			// ce code décompose les nombres plus grands en nombres 32 bits par masquage et bit shifting
			for (int i = sizeof(T)-sizeof(UInt32); i >= 0; i -= sizeof(UInt32))
			{
				// Le masque 32 bits sur la partie du nombre qu'on traite actuellement
				T mask = T(std::numeric_limits<UInt32>::max()) << i*8;
				UInt32 val = UInt32((number & mask) >> i*8); // Masquage et shifting des bits vers la droite (pour le ramener sur 32bits)

				// Appel de la fonction avec le nombre 32bits, si le résultat est non-nul nous avons la réponse
				unsigned int log2 = IntegralLog2Pot<UInt32>(val);
				if (log2)
					return log2 + i*8;
			}

			return 0;
		}
	}

	template<typename T>
	T Approach(T value, T objective, T increment)
	{
		///TODO: Marquer comme constexpr en C++14
		if (value < objective)
			return std::min(value + increment, objective);
		else if (value > objective)
			return std::max(value - increment, objective);
		else
			return value;
	}

	template<typename T>
	constexpr T Clamp(T value, T min, T max)
	{
		return std::max(std::min(value, max), min);
	}

	template<typename T>
	T CountBits(T value)
	{
		// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
		unsigned int count = 0;
		while (value)
		{
			value &= value - 1;
			count++;
		}

		return count;
	}

	template<typename T>
	constexpr T DegreeToRadian(T degrees)
	{
		return degrees * T(M_PI/180.0);
	}

	template<typename T>
	constexpr T FromDegrees(T degrees)
	{
		#if NAZARA_MATH_ANGLE_RADIAN
		return DegreeToRadian(degrees);
		#else
		return degrees;
		#endif
	}

	template<typename T>
	constexpr T FromRadians(T radians)
	{
		#if NAZARA_MATH_ANGLE_RADIAN
		return radians;
		#else
		return RadianToDegree(radians);
		#endif
	}

	inline unsigned int GetNearestPowerOfTwo(unsigned int number)
	{
		///TODO: Marquer comme constexpr en C++14
		unsigned int x = 1;
		// Tant que x est plus petit que n, on décale ses bits vers la gauche, ce qui revient à multiplier par deux
		while (x < number)
			x <<= 1;

		return x;
	}

	inline unsigned int GetNumberLength(signed char number)
	{
		///TODO: Marquer comme constexpr en C++14
		// Le standard définit le char comme étant codé sur un octet
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

	inline unsigned int GetNumberLength(unsigned char number)
	{
		///TODO: Marquer comme constexpr en C++14
		// Le standard définit le char comme étant codé sur un octet
		static_assert(sizeof(number) == 1, "Unsigned char must be one byte-sized");

		if (number >= 100)
			return 3;
		else if (number >= 10)
			return 2;
		else
			return 1;
	}

	inline unsigned int GetNumberLength(int number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(std::abs(number))) + (number < 0 ? 2 : 1);
	}

	inline unsigned int GetNumberLength(unsigned int number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(number))+1;
	}

	inline unsigned int GetNumberLength(long long number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(std::abs(number))) + (number < 0 ? 2 : 1);
	}

	inline unsigned int GetNumberLength(unsigned long long number)
	{
		if (number == 0)
			return 1;

		return static_cast<unsigned int>(std::log10(number)) + 1;
	}

	inline unsigned int GetNumberLength(float number, UInt8 precision)
	{
		// L'imprécision des flottants nécessite un cast (log10(9.99999) = 0.99999)
		return GetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
	}

	inline unsigned int GetNumberLength(double number, UInt8 precision)
	{
		// L'imprécision des flottants nécessite un cast (log10(9.99999) = 0.99999)
		return GetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
	}

	inline unsigned int GetNumberLength(long double number, UInt8 precision)
	{
		// L'imprécision des flottants nécessite un cast (log10(9.99999) = 0.99999)
		return GetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
	}

	template<typename T>
	unsigned int IntegralLog2(T number)
	{
		// Proxy nécessaire pour éviter un problème de surcharge
		return Detail::IntegralLog2<T>(number);
	}

	template<typename T>
	unsigned int IntegralLog2Pot(T pot)
	{
		return Detail::IntegralLog2Pot<T>(pot);
	}

	inline unsigned int IntegralPow(unsigned int base, unsigned int exponent)
	{
		///TODO: Marquer comme constexpr en C++14
		unsigned int r = 1;
		for (unsigned int i = 0; i < exponent; ++i)
			r *= base;

		return r;
	}

	template<typename T, typename T2>
	T Lerp(T from, T to, T2 interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < T2(0.0) || interpolation > T2(1.0))
			NazaraWarning("Interpolation should be in range [0..1] (Got " + String::Number(interpolation) + ')');
		#endif

		return from + interpolation * (to - from);
	}

	template<typename T>
	T MultiplyAdd(T x, T y, T z)
	{
		return x*y + z;
	}

	#ifdef FP_FAST_FMAF
	template<>
	inline float MultiplyAdd(float x, float y, float z)
	{
		return std::fmaf(x, y, z);
	}
	#endif

	#ifdef FP_FAST_FMA
	template<>
	inline double MultiplyAdd(double x, double y, double z)
	{
		return std::fma(x, y, z);
	}
	#endif

	#ifdef FP_FAST_FMAL
	template<>
	inline long double MultiplyAdd(long double x, long double y, long double z)
	{
		return std::fmal(x, y, z);
	}
	#endif

	template<typename T>
	T NormalizeAngle(T angle)
	{
		#if NAZARA_MATH_ANGLE_RADIAN
		const T limit = T(M_PI);
		#else
		const T limit = T(180.0);
		#endif
		const T twoLimit = limit * T(2);

		angle = std::fmod(angle + limit, twoLimit);
		if (angle < T(0))
			angle += twoLimit;

		return angle - limit;
	}

	template<typename T>
	bool NumberEquals(T a, T b)
	{
		return NumberEquals(a, b, std::numeric_limits<T>::epsilon());
	}

	template<typename T>
	bool NumberEquals(T a, T b, T maxDifference)
	{
		if (b > a)
			std::swap(a, b);

		T diff = a - b;
		return diff <= maxDifference;
	}

	inline String NumberToString(long long number, UInt8 radix)
	{
		#if NAZARA_MATH_SAFE
		if (radix < 2 || radix > 36)
		{
			NazaraError("Base must be between 2 and 36");
			return String();
		}
		#endif

		if (number == 0)
			return String('0');

		static const char* symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		bool negative;
		if (number < 0)
		{
			negative = true;
			number = -number;
		}
		else
			negative = false;

		String str;
		str.Reserve(GetNumberLength(number)); // Prends en compte le signe négatif

		do
		{
			str.Append(symbols[number % radix]);
			number /= radix;
		}
		while (number > 0);

		if (negative)
			str.Append('-');

		return str.Reverse();
	}

	template<typename T>
	T RadianToDegree(T radians)
	{
		return radians * T(180.0/M_PI);
	}

	inline long long StringToNumber(String str, UInt8 radix, bool* ok)
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

		static const char* symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		str.Simplify();
		if (radix > 10)
			str = str.ToUpper();

		bool negative = str.StartsWith('-');

		char* digit = &str[(negative) ? 1 : 0];
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

	template<typename T>
	constexpr T ToDegrees(T angle)
	{
		#if NAZARA_MATH_ANGLE_RADIAN
		return RadianToDegree(angle);
		#else
		return angle;
		#endif
	}

	template<typename T>
	constexpr T ToRadians(T angle)
	{
		#if NAZARA_MATH_ANGLE_RADIAN
		return angle;
		#else
		return DegreeToRadian(angle);
	}
	#endif
}

#include <Nazara/Core/DebugOff.hpp>
