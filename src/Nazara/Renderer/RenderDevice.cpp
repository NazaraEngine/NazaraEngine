// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderDevice::~RenderDevice() = default;

	std::shared_ptr<ShaderModule> RenderDevice::InstantiateShaderModule(ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const std::filesystem::path& sourcePath)
	{
		File file(sourcePath);
		if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
		{
			NazaraError("Failed to open \"" + sourcePath.generic_u8string() + '"');
			return {};
		}

		std::size_t length = static_cast<std::size_t>(file.GetSize());

		std::vector<Nz::UInt8> source(length);
		if (file.Read(&source[0], length) != length)
		{
			NazaraError("Failed to read program file");
			return {};
		}

		return InstantiateShaderModule(shaderStages, lang, source.data(), source.size());
	}
}
