// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENEDATA_HPP
#define NAZARA_SCENEDATA_HPP

#include <Nazara/Core/Color.hpp>

class NzAbstractBackground;

struct NzSceneData
{
	NzColor ambientColor;
	NzAbstractBackground* background;
};

#endif // NAZARA_SCENEDATA_HPP
