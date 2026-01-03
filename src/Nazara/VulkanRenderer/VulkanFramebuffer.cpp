// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanFramebuffer.hpp>

namespace Nz
{
	void VulkanFramebuffer::UpdateDebugName(std::string_view name)
	{
		GetFramebuffer().SetDebugName(name);
	}
}
