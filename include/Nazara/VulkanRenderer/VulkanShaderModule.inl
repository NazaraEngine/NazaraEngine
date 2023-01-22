// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderModule.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline const Vk::ShaderModule& VulkanShaderModule::GetHandle() const
	{
		return m_shaderModule;
	}

	inline auto VulkanShaderModule::GetStages() const -> const std::vector<Stage>&
	{
		return m_stages;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
