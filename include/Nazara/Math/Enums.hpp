// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_MATH_HPP
#define NAZARA_ENUMS_MATH_HPP

enum nzCorner
{
	nzCorner_FarLeftBottom,
	nzCorner_FarLeftTop,
	nzCorner_FarRightBottom,
	nzCorner_FarRightTop,
	nzCorner_NearLeftBottom,
	nzCorner_NearLeftTop,
	nzCorner_NearRightBottom,
	nzCorner_NearRightTop,

	nzCorner_Max = nzCorner_NearRightTop
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

#endif // NAZARA_ENUMS_MATH_HPP
