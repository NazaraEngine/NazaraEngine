// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderDevice::~RenderDevice() = default;

	std::shared_ptr<ShaderModule> RenderDevice::InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const std::filesystem::path& sourcePath, const nzsl::ShaderWriter::States& states)
	{
		File file(sourcePath);
		if (!file.Open(OpenMode::ReadOnly | OpenMode::Text))
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

		return InstantiateShaderModule(shaderStages, lang, source.data(), source.size(), states);
	}

	void RenderDevice::ValidateFeatures(const RenderDeviceFeatures& supportedFeatures, RenderDeviceFeatures& enabledFeatures)
	{
#define NzValidateFeature(field, name) \
		if (enabledFeatures.field && !supportedFeatures.field) \
		{ \
			NazaraWarning(name " was enabled but device doesn't support it, disabling..."); \
			enabledFeatures.field = false; \
		}

		NzValidateFeature(anisotropicFiltering, "anistropic filtering feature")
		NzValidateFeature(depthClamping, "depth clamping feature")
		NzValidateFeature(nonSolidFaceFilling, "non-solid face filling feature")
		NzValidateFeature(storageBuffers, "storage buffers support")
		NzValidateFeature(unrestrictedTextureViews, "unrestricted texture view support")

#undef NzValidateFeature
	}
}
