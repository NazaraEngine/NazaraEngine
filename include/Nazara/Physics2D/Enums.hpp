// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PHYSICS2D_HPP
#define NAZARA_ENUMS_PHYSICS2D_HPP

namespace Nz
{
	enum ColliderType2D
	{
		ColliderType2D_Box,
		ColliderType2D_Convex,
		ColliderType2D_Circle,
		ColliderType2D_Null,
		ColliderType2D_Segment,

		ColliderType2D_Max = ColliderType2D_Segment
	};
}

#endif // NAZARA_ENUMS_PHYSICS2D_HPP
