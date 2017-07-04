// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_MATH_HPP
#define NAZARA_ENUMS_MATH_HPP

namespace Nz
{
	enum BoxCorner
	{
		BoxCorner_FarLeftBottom,
		BoxCorner_FarLeftTop,
		BoxCorner_FarRightBottom,
		BoxCorner_FarRightTop,
		BoxCorner_NearLeftBottom,
		BoxCorner_NearLeftTop,
		BoxCorner_NearRightBottom,
		BoxCorner_NearRightTop,

		BoxCorner_Max = BoxCorner_NearRightTop
	};

	enum Extend
	{
		Extend_Finite,
		Extend_Infinite,
		Extend_Null,

		Extend_Max = Extend_Null
	};

	enum FrustumPlane
	{
		FrustumPlane_Bottom,
		FrustumPlane_Far,
		FrustumPlane_Left,
		FrustumPlane_Near,
		FrustumPlane_Right,
		FrustumPlane_Top,

		FrustumPlane_Max = FrustumPlane_Top
	};

	enum IntersectionSide
	{
		IntersectionSide_Inside,
		IntersectionSide_Intersecting,
		IntersectionSide_Outside,

		IntersectionSide_Max = IntersectionSide_Outside
	};

	enum RectCorner
	{
		RectCorner_LeftBottom,
		RectCorner_LeftTop,
		RectCorner_RightBottom,
		RectCorner_RightTop,

		RectCorner_Max = RectCorner_RightTop
	};
}

#endif // NAZARA_ENUMS_MATH_HPP
