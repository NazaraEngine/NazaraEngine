// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERDEVICEINSTANCE_HPP
#define NAZARA_RENDERDEVICEINSTANCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	///TODO: Rename
	class NAZARA_RENDERER_API RenderDeviceInstance
	{
		public:
			RenderDeviceInstance() = default;
			virtual ~RenderDeviceInstance();
	};
}

#include <Nazara/Renderer/RenderDeviceInstance.inl>

#endif // NAZARA_RENDERDEVICEINSTANCE_HPP
