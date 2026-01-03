// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
