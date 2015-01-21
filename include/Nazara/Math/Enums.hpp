// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_MATH_HPP
#define NAZARA_ENUMS_MATH_HPP

enum nzBoxCorner
{
	nzBoxCorner_FarLeftBottom,
	nzBoxCorner_FarLeftTop,
	nzBoxCorner_FarRightBottom,
	nzBoxCorner_FarRightTop,
	nzBoxCorner_NearLeftBottom,
	nzBoxCorner_NearLeftTop,
	nzBoxCorner_NearRightBottom,
	nzBoxCorner_NearRightTop,

	nzBoxCorner_Max = nzBoxCorner_NearRightTop
};

enum nzExtend
{
	nzExtend_Finite,
	nzExtend_Infinite,
	nzExtend_Null,

	nzExtend_Max = nzExtend_Null
};

enum nzFrustumPlane
{
	nzFrustumPlane_Bottom,
	nzFrustumPlane_Far,
	nzFrustumPlane_Left,
	nzFrustumPlane_Near,
	nzFrustumPlane_Right,
	nzFrustumPlane_Top,

	nzFrustumPlane_Max = nzFrustumPlane_Top
};

enum nzIntersectionSide
{
	nzIntersectionSide_Inside,
	nzIntersectionSide_Intersecting,
	nzIntersectionSide_Outside,

	nzIntersectionSide_Max = nzIntersectionSide_Outside
};

enum nzRectCorner
{
	nzRectCorner_LeftBottom,
	nzRectCorner_LeftTop,
	nzRectCorner_RightBottom,
	nzRectCorner_RightTop,

	nzRectCorner_Max = nzRectCorner_RightTop
};

#endif // NAZARA_ENUMS_MATH_HPP
