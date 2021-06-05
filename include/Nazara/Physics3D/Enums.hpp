// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PHYSICS3D_HPP
#define NAZARA_ENUMS_PHYSICS3D_HPP

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

#endif // NAZARA_ENUMS_PHYSICS3D_HPP
