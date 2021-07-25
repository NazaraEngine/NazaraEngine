// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_GRAPHICS_HPP
#define NAZARA_ENUMS_GRAPHICS_HPP

namespace Nz
{
	enum class BasicRenderElement
	{
		Submesh = 0,
	};

	enum class CullTest
	{
		Box,
		NoTest,
		Sphere,
		Volume
	};

	enum class ProjectionType
	{
		Orthographic,
		Perspective
	};
}

#endif // NAZARA_ENUMS_GRAPHICS_HPP
