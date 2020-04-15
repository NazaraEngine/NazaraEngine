// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	bool OpenGLShaderStage::Create(Vk::Device& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
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

#endif
