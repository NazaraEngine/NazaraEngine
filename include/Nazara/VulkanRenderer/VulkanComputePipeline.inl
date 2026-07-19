// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline const Vk::Pipeline& VulkanComputePipeline::GetPipeline() const
	{
		return m_pipeline;
	}

	inline const GpuComputePipelineInfo& VulkanComputePipeline::GetPipelineInfo() const
	{
		return m_pipelineInfo;
	}
}
