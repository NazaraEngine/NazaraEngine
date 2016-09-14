// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkRenderTarget.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VkRenderTarget::~VkRenderTarget()
	{
		OnRenderTargetRelease(this);
	}
}
