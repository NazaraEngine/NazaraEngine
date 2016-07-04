// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/RenderTarget.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	RenderTarget::~RenderTarget()
	{
		OnRenderTargetRelease(this);
	}
}
