// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PHYSICS3D_HPP
#define NAZARA_ENUMS_PHYSICS3D_HPP

namespace Nz
{
	enum ColliderType3D
	{
		ColliderType3D_Box,
		ColliderType3D_Capsule,
		ColliderType3D_Cone,
		ColliderType3D_Compound,
		ColliderType3D_ConvexHull,
		ColliderType3D_Cylinder,
		ColliderType3D_Heightfield,
		ColliderType3D_Null,
		ColliderType3D_Scene,
		ColliderType3D_Sphere,
		ColliderType3D_Tree,

		ColliderType3D_Max = ColliderType3D_Tree
	};
}

#endif // NAZARA_ENUMS_PHYSICS3D_HPP
