// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline Vk::RenderPass& VulkanRenderPass::GetRenderPass()
	{
		return m_renderPass;
	}

	inline const Vk::RenderPass& VulkanRenderPass::GetRenderPass() const
	{
		return m_renderPass;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
