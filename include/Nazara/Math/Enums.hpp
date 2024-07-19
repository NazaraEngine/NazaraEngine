// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_ENUMS_HPP
#define NAZARA_MATH_ENUMS_HPP

#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	enum class AngleUnit
	{
		Degree,
		Radian,
		Turn
	};

	enum class BoxCorner
	{
		LeftBottomFar,
		LeftBottomNear,
		LeftTopFar,
		LeftTopNear,
		RightBottomFar,
		RightBottomNear,
		RightTopFar,
		RightTopNear,

		Max = RightTopNear
	};

	constexpr std::size_t BoxCornerCount = UnderlyingCast(BoxCorner::Max) + 1;

	enum class CoordinateSystem
	{
		Cartesian,
		Screen,
		UV = Screen,
		World = Cartesian
	};

	enum class Extent
	{
		Finite,
		Infinite,
		Null,

		Max = Null
	};

	enum class FrustumPlane
	{
		Bottom,
		Far,
		Left,
		Near,
		Right,
		Top,

		Max = Top
	};

	constexpr std::size_t FrustumPlaneCount = UnderlyingCast(FrustumPlane::Max) + 1;

	enum class IntersectionSide
	{
		Inside,
		Intersecting,
		Outside,

		Max = Outside
	};

	enum class RectCorner
	{
		LeftBottom,
		LeftTop,
		RightBottom,
		RightTop,

		Max = RightTop
	};

	constexpr std::size_t RectCornerCount = UnderlyingCast(RectCorner::Max) + 1;
}

#endif // NAZARA_MATH_ENUMS_HPP
