// Copyright (C) 2012 Jérôme Leclercq
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

	nzCorner_Max = nzCorner_FarRightTop
};

#endif // NAZARA_ENUMS_MATH_HPP
