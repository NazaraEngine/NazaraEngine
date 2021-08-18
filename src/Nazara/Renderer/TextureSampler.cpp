// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	TextureSampler::~TextureSampler() = default;

	void TextureSampler::ValidateSamplerInfo(const RenderDevice& device, TextureSamplerInfo& samplerInfo)
	{
		const RenderDeviceFeatures& deviceFeatures = device.GetEnabledFeatures();
		if (samplerInfo.anisotropyLevel > 1.f && !deviceFeatures.anisotropicFiltering)
		{
			NazaraWarning("texture sampler has anistropy level > 1.0 but anistropic filtering is not enabled on the device, disabling...");
			samplerInfo.anisotropyLevel = 0.f;
		}
	}
}
