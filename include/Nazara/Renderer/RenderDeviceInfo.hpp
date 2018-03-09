// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERDEVICE_HPP
#define NAZARA_RENDERDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	struct RenderDeviceInfo
	{
		RenderDeviceType type;
		String name;
	};
}

#endif // NAZARA_RENDERER_HPP
