// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	RenderPipeline::~RenderPipeline() = default;

	void RenderPipeline::ValidatePipelineInfo(const RenderDevice& device, RenderPipelineInfo& pipelineInfo)
	{
		const RenderDeviceFeatures& deviceFeatures = device.GetEnabledFeatures();
		if (pipelineInfo.faceFilling != FaceFilling::Fill && !deviceFeatures.nonSolidFaceFilling)
		{
			NazaraWarning("pipeline has face filling set to non-solid but non-solid face filling is not enabled on the device, disabling...");
			pipelineInfo.faceFilling = FaceFilling::Fill;
		}

		if (pipelineInfo.depthClamp && !deviceFeatures.depthClamping)
		{
			NazaraWarning("pipeline has depth clamp enabled but depth clamping is not enabled on the device, disabling...");
			pipelineInfo.depthClamp = false;
		}
	}
}
