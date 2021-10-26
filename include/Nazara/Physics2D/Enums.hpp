// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PHYSICS2D_HPP
#define NAZARA_ENUMS_PHYSICS2D_HPP

namespace Nz
{
	enum class ColliderType2D
	{
		Box,
		Compound,
		Convex,
		Circle,
		Null,
		Segment,

		Max = Segment
	};
}

#endif // NAZARA_ENUMS_PHYSICS2D_HPP
