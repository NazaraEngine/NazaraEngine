// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANCOMPUTEPIPELINE_HPP
#define NAZARA_VULKANRENDERER_VULKANCOMPUTEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Renderer/ComputePipeline.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Pipeline.hpp>

namespace Nz
{
	class VulkanDevice;

	class NAZARA_VULKANRENDERER_API VulkanComputePipeline : public ComputePipeline
	{
		public:
			VulkanComputePipeline(VulkanDevice& device, ComputePipelineInfo pipelineInfo);
			VulkanComputePipeline(const VulkanComputePipeline&) = delete;
			VulkanComputePipeline(VulkanComputePipeline&&) = delete;
			~VulkanComputePipeline() = default;

			inline const Vk::Pipeline& GetPipeline() const;
			inline const ComputePipelineInfo& GetPipelineInfo() const override;

			void UpdateDebugName(std::string_view name) override;

			VulkanComputePipeline& operator=(const VulkanComputePipeline&) = delete;
			VulkanComputePipeline& operator=(VulkanComputePipeline&&) = delete;

		private:
			Vk::Pipeline m_pipeline;
			ComputePipelineInfo m_pipelineInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanComputePipeline.inl>

#endif // NAZARA_VULKANRENDERER_VULKANCOMPUTEPIPELINE_HPP
