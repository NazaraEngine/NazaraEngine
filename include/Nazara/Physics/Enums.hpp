// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PHYSICS_HPP
#define NAZARA_ENUMS_PHYSICS_HPP

enum nzGeomType
{
	nzGeomType_Box,
	nzGeomType_Capsule,
	nzGeomType_Cone,
	nzGeomType_Compound,
	nzGeomType_ConvexHull,
	nzGeomType_Cylinder,
	nzGeomType_Heightfield,
	nzGeomType_Null,
	nzGeomType_Scene,
	nzGeomType_Sphere,
	nzGeomType_Tree,

	nzGeomType_Max = nzGeomType_Tree
};

#endif // NAZARA_ENUMS_PHYSICS_HPP
