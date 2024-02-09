// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_ENUMS_HPP
#define NAZARA_PHYSICS3D_ENUMS_HPP

namespace Nz
{
	enum class ColliderType3D
	{
		Box,
		Capsule,
		Compound,
		Convex,
		Mesh,
		Sphere,

		ScaleDecoration,
		TranslatedRotatedDecoration,

		Max = TranslatedRotatedDecoration
	};

	enum class PhysMotionQuality3D
	{
		Discrete,
		LinearCast
	};
}

#endif // NAZARA_PHYSICS3D_ENUMS_HPP
