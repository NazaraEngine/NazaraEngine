// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PHYSICS_HPP
#define NAZARA_ENUMS_PHYSICS_HPP

enum GeomType
{
	GeomType_Box,
	GeomType_Capsule,
	GeomType_Cone,
	GeomType_Compound,
	GeomType_ConvexHull,
	GeomType_Cylinder,
	GeomType_Heightfield,
	GeomType_Null,
	GeomType_Scene,
	GeomType_Sphere,
	GeomType_Tree,

	GeomType_Max = GeomType_Tree
};

#endif // NAZARA_ENUMS_PHYSICS_HPP
