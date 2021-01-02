// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderStage.hpp>
#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/ShaderAstSerializer.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	bool VulkanShaderStage::Create(Vk::Device& device, const ShaderAst& shader, const ShaderWriter::States& states)
	{
		m_stage = shader.GetStage();

		SpirvWriter::Environment env;

		SpirvWriter writer;
		writer.SetEnv(env);

		std::vector<UInt32> code = writer.Generate(shader, states);

		if (!m_shaderModule.Create(device, code.data(), code.size() * sizeof(UInt32)))
		{
			NazaraError("Failed to create shader module");
			return false;
		}

		return true;
	}

	bool VulkanShaderStage::Create(Vk::Device& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		m_stage = type;

		switch (lang)
		{
			case ShaderLanguage::NazaraBinary:
			{
				auto shader = UnserializeShader(source, sourceSize);
				if (shader.GetStage() != type)
					throw std::runtime_error("incompatible shader stage");

				if (!Create(device, shader, {}))
					return false;

				break;
			}

			case ShaderLanguage::SpirV:
			{
				if (!m_shaderModule.Create(device, reinterpret_cast<const Nz::UInt32*>(source), sourceSize))
				{
					NazaraError("Failed to create shader module");
					return false;
				}

				break;
			}

			default:
			{
				NazaraError("this language is not supported");
				return false;
			}
		}

		return true;
	}
}
