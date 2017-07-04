// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENEDATA_HPP
#define NAZARA_SCENEDATA_HPP

#include <Nazara/Core/Color.hpp>

namespace Nz
{
	class AbstractBackground;
	class AbstractViewer;

	struct SceneData
	{
		Color ambientColor;
		const AbstractBackground* background;
		const AbstractViewer* viewer;
	};
}

#endif // NAZARA_SCENEDATA_HPP
