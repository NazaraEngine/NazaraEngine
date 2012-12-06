// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Config.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)
#define F2(a) static_cast<T2>(a)

template<typename T>
T NzApproach(T value, T objective, T increment)
{
	if (value < objective)
		return std::min(value + increment, objective);
	else if (value > objective)
		return std::max(value - increment, objective);
	else
		return value;
}

template<typename T>
T NzClamp(T value, T min, T max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

template<typename T>
T NzDegrees(T degrees)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return NzDegreeToRadian(degrees);
	#else
	return degrees;
	#endif
}

template<typename T>
T NzDegreeToRadian(T degrees)
{
	return degrees * F(M_PI/180.0);
}

unsigned int NzGetNumberLength(signed char number)
{
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

unsigned int NzGetNumberLength(unsigned char number)
{
	// Le standard définit le char comme étant codé sur un octet
	static_assert(sizeof(number) == 1, "Unsigned char must be one byte-sized");

	if (number >= 100)
		return 3;
	else if (number >= 10)
		return 2;
	else
		return 1;
}

unsigned int NzGetNumberLength(int number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned int number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(long long number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned long long number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(float number, nzUInt8 precision)
{
	// L'imprécision des flottants nécessite un cast (log10(9.99999) = 1)
	return NzGetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
}

unsigned int NzGetNumberLength(double number, nzUInt8 precision)
{
	// L'imprécision des flottants nécessite un cast (log10(9.99999) = 1)
	return NzGetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
}

unsigned int NzGetNumberLength(long double number, nzUInt8 precision)
{
	// L'imprécision des flottants nécessite un cast (log10(9.99999) = 1)
	return NzGetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
}

template<typename T, typename T2>
T NzLerp(T from, T to, T2 interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F2(0.0) || interpolation > F2(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return F(0.0);
	}
	#endif

	return from + interpolation*(to - from);
}

template<typename T>
T NzNormalizeAngle(T angle)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	const T limit = F(M_PI);
	#else
	const T limit = F(180.0);
	#endif

	while (angle > limit)
		angle -= limit;

	while (angle < limit)
		angle += limit;

	return angle;
}

template<typename T>
bool NzNumberEquals(T a, T b)
{
	return std::fabs(a-b) < std::numeric_limits<T>::epsilon();
}

NzString NzNumberToString(long long number, nzUInt8 radix)
{
	#if NAZARA_MATH_SAFE
	if (radix < 2 || radix > 36)
	{
		NazaraError("Base must be between 2 and 36");
		return NzString();
	}
	#endif

	if (number == 0)
		return '0';

	static const char* symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	bool negative;
	if (number < 0)
	{
		negative = true;
		number = -number;
	}
	else
		negative = false;

	NzString str;
	str.Reserve(NzGetNumberLength(number)); // Prends en compte le signe négatif

	do
	{
		str += symbols[number % radix];
		number /= radix;
	}
	while (number > 0);

	if (negative)
		str += '-';

	return str.Reversed();
}

template<typename T>
T NzRadians(T radians)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return radians;
	#else
	return NzRadianToDegree(radians);
	#endif
}

template<typename T>
T NzRadianToDegree(T radians)
{
	return radians * F(180.0/M_PI);
}

long long NzStringToNumber(NzString str, nzUInt8 radix, bool* ok)
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

	str.Simplify();
	if (radix > 10)
		str.ToUpper();

	bool negative = str.StartsWith('-');
	static const char* symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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
			NazaraError("str is not a valid number");

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

#undef F2
#undef F

#include <Nazara/Core/DebugOff.hpp>
