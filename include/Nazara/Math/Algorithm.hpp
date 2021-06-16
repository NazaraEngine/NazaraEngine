
// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_MATH_HPP
#define NAZARA_ALGORITHM_MATH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Enums.hpp>
#include <cmath>
#include <limits>
#include <string>

namespace Nz
{
	template<typename T> constexpr T HalfPi = T(1.5707963267948966192313216916398);
	template<typename T> constexpr T Pi     = T(3.1415926535897932384626433832795);
	template<typename T> constexpr T Sqrt2  = T(1.4142135623730950488016887242097);
	template<typename T> constexpr T Sqrt3  = T(1.7320508075688772935274463415059);
	template<typename T> constexpr T Sqrt5  = T(2.2360679774997896964091736687313);

	template<AngleUnit Unit, typename T> class Angle;

	template<typename T> constexpr T Approach(T value, T objective, T increment);
	template<typename T> constexpr T Clamp(T value, T min, T max);
	template<typename T, AngleUnit Unit> constexpr Angle<Unit, T> Clamp(Angle<Unit, T> value, T min, T max);
	template<typename T> T ClearBit(T number, T bit);
	template<typename T> constexpr std::size_t CountBits(T value);
	template<typename T> constexpr T DegreeToRadian(T degrees);
	template<typename T> constexpr T GetNearestPowerOfTwo(T number);
	constexpr unsigned int GetNumberLength(signed char number);
	constexpr unsigned int GetNumberLength(unsigned char number);
	unsigned int GetNumberLength(int number);
	/*constexpr*/ unsigned int GetNumberLength(unsigned int number);
	unsigned int GetNumberLength(long long number);
	/*constexpr*/ unsigned int GetNumberLength(unsigned long long number);
	unsigned int GetNumberLength(float number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	unsigned int GetNumberLength(double number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	unsigned int GetNumberLength(long double number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	template<typename T> /*constexpr*/ unsigned int IntegralLog2(T number);
	template<typename T> /*constexpr*/ unsigned int IntegralLog2Pot(T pot);
	template<typename T> constexpr T IntegralPow(T base, unsigned int exponent);
	template<typename T, typename T2> constexpr T Lerp(const T& from, const T& to, const T2& interpolation);
	template<typename T> constexpr T MultiplyAdd(T x, T y, T z);
	template<typename T> constexpr bool NumberEquals(T a, T b);
	template<typename T> constexpr bool NumberEquals(T a, T b, T maxDifference);
	inline std::string NumberToString(long long number, UInt8 radix = 10);
	template<typename T> constexpr T RadianToDegree(T radians);
	template<typename T> T SetBit(T number, T bit);
	inline long long StringToNumber(const std::string_view& str, UInt8 radix = 10, bool* ok = nullptr);
	template<typename T> bool TestBit(T number, T bit);
	template<typename T> T ToggleBit(T number, T bit);
}

#include <Nazara/Math/Algorithm.inl>

#endif // NAZARA_ALGORITHM_MATH_HPP
