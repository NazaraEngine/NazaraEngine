// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERDEVICE_HPP
#define NAZARA_RENDERDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <string>

namespace Nz
{
	struct RenderDeviceLimits
	{
		UInt64 minUniformBufferOffsetAlignment;
	};

	struct RenderDeviceInfo
	{
		RenderDeviceLimits limits;
		RenderDeviceType type;
		std::string name;
	};
}

#endif // NAZARA_RENDERER_HPP
