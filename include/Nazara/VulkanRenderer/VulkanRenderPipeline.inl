// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline std::size_t VulkanRenderPipeline::PipelineHasher::operator()(const std::pair<VkRenderPass, std::size_t>& renderPass) const
	{
		std::size_t seed = 0;
		HashCombine(seed, renderPass.first);
		HashCombine(seed, renderPass.second);

		return seed;
	}

	inline const RenderPipelineInfo& VulkanRenderPipeline::GetPipelineInfo() const
	{
		return m_pipelineInfo;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
