// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_3D_HPP
#define NAZARA_ENUMS_3D_HPP

enum nzLightType
{
	nzLightType_Directional,
	nzLightType_Point,
	nzLightType_Spot,

	nzLightType_Max = nzLightType_Spot
};

enum nzSceneNodeType
{
	nzSceneNodeType_Light,
	nzSceneNodeType_Model,

	nzSceneNodeType_Max = nzSceneNodeType_Model
};

#endif // NAZARA_ENUMS_3D_HPP
