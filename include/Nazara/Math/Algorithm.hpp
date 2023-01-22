// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_ALGORITHM_HPP
#define NAZARA_MATH_ALGORITHM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <cmath>
#include <limits>
#include <string>

namespace Nz
{
	template<AngleUnit Unit, typename T> class Angle;

	template<typename T, AngleUnit Unit> constexpr Angle<Unit, T> Clamp(Angle<Unit, T> value, T min, T max);

	constexpr unsigned int GetNumberLength(signed char number);
	constexpr unsigned int GetNumberLength(unsigned char number);
	unsigned int GetNumberLength(int number);
	/*constexpr*/ unsigned int GetNumberLength(unsigned int number);
	unsigned int GetNumberLength(long long number);
	/*constexpr*/ unsigned int GetNumberLength(unsigned long long number);
	unsigned int GetNumberLength(float number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	unsigned int GetNumberLength(double number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	unsigned int GetNumberLength(long double number, UInt8 precision = NAZARA_CORE_DECIMAL_DIGITS);
	inline std::string NumberToString(long long number, UInt8 radix = 10);
	inline long long StringToNumber(const std::string_view& str, UInt8 radix = 10, bool* ok = nullptr);
}

#include <Nazara/Math/Algorithm.inl>

#endif // NAZARA_MATH_ALGORITHM_HPP
