// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>

namespace Nz
{
	RenderDevice::~RenderDevice() = default;

	std::shared_ptr<ShaderModule> RenderDevice::InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const std::filesystem::path& sourcePath, const nzsl::BackendParameters& states)
	{
		File file(sourcePath);
		if (!file.Open(OpenMode::Read | OpenMode::Text))
		{
			NazaraError("failed to open \"{0}\"", sourcePath);
			return {};
		}

		std::size_t length = static_cast<std::size_t>(file.GetSize());

		std::vector<Nz::UInt8> source(length);
		if (file.Read(&source[0], length) != length)
		{
			NazaraError("failed to read program file");
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
		NzValidateFeature(computeShaders, "compute shaders feature")
		NzValidateFeature(depthClamping, "depth clamping feature")
		NzValidateFeature(nonSolidFaceFilling, "non-solid face filling feature")
		NzValidateFeature(storageBuffers, "storage buffers support")
		NzValidateFeature(textureReadWithoutFormat, "texture read without format")
		NzValidateFeature(textureReadWrite, "texture read/write")
		NzValidateFeature(textureWriteWithoutFormat, "texture write without format")
		NzValidateFeature(unrestrictedTextureViews, "unrestricted texture view support")

#undef NzValidateFeature
	}
}
