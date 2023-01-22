// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
		Cone,
		Compound,
		ConvexHull,
		Cylinder,
		Heightfield,
		Null,
		Scene,
		Sphere,
		Tree,

		Max = Tree
	};
}

#endif // NAZARA_PHYSICS3D_ENUMS_HPP
