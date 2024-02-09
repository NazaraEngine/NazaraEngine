// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_ENUMS_HPP
#define NAZARA_PHYSICS2D_ENUMS_HPP

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

#endif // NAZARA_PHYSICS2D_ENUMS_HPP
