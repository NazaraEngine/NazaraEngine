// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderStage.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	bool VulkanShaderStage::Create(const Vk::DeviceHandle& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		if (lang != ShaderLanguage::SpirV)
		{
			NazaraError("Only Spir-V is supported for now");
			return false;
		}

		if (!m_shaderModule.Create(device, reinterpret_cast<const Nz::UInt32*>(source), sourceSize))
		{
			NazaraError("Failed to create shader module");
			return false;
		}

		m_stage = type;
		return true;
	}
}
