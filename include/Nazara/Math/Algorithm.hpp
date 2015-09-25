// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_MATH_HPP
#define NAZARA_ALGORITHM_MATH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

#ifndef M_PI_2
#define M_PI_2  1.5707963267948966192313217
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730950488016887
#endif

#ifndef M_SQRT3
#define M_SQRT3 1.7320508075688772935274463
#endif

namespace Nz
{
	template<typename T> T Approach(T value, T objective, T increment);
	template<typename T> constexpr T Clamp(T value, T min, T max);
	template<typename T> T CountBits(T value);
	template<typename T> constexpr T FromDegrees(T degrees);
	template<typename T> constexpr T FromRadians(T radians);
	template<typename T> constexpr T DegreeToRadian(T degrees);
	unsigned int GetNearestPowerOfTwo(unsigned int number);
	unsigned int GetNumberLength(signed char number);
	unsigned int GetNumberLength(unsigned char number);
	unsigned int GetNumberLength(int number);
	unsigned int GetNumberLength(unsigned int number);
	unsigned int GetNumberLength(long long number);
	unsigned int GetNumberLength(unsigned long long number);
	unsigned int GetNumberLength(float number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	unsigned int GetNumberLength(double number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	unsigned int GetNumberLength(long double number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	template<typename T> unsigned int IntegralLog2(T number);
	template<typename T> unsigned int IntegralLog2Pot(T pot);
	unsigned int IntegralPow(unsigned int base, unsigned int exponent);
	template<typename T, typename T2> T Lerp(T from, T to, T2 interpolation);
	template<typename T> T MultiplyAdd(T x, T y, T z);
	template<typename T> T NormalizeAngle(T angle);
	template<typename T> bool NumberEquals(T a, T b);
	template<typename T> bool NumberEquals(T a, T b, T maxDifference);
	String NumberToString(long long number, UInt8 radix = 10);
	template<typename T> T RadianToDegree(T radians);
	long long StringToNumber(String str, UInt8 radix = 10, bool* ok = nullptr);
	template<typename T> constexpr T ToDegrees(T angle);
	template<typename T> constexpr T ToRadians(T angle);
}

#include <Nazara/Math/Algorithm.inl>

#endif // NAZARA_ALGORITHM_MATH_HPP
