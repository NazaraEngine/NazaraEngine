// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPIPELINE_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderPipeline : public RenderPipeline
	{
		public:
			VulkanRenderPipeline(Vk::DeviceHandle device, RenderPipelineInfo pipelineInfo);
			~VulkanRenderPipeline() = default;

			static VkPipelineColorBlendAttachmentState BuildColorBlendState(const RenderPipelineInfo& pipelineInfo);
			static VkPipelineDepthStencilStateCreateInfo BuildDepthStencilInfo(const RenderPipelineInfo& pipelineInfo);
			static VkPipelineInputAssemblyStateCreateInfo BuildInputAssemblyInfo(const RenderPipelineInfo& pipelineInfo);
			static VkPipelineRasterizationStateCreateInfo BuildRasterizationInfo(const RenderPipelineInfo& pipelineInfo);
			static VkStencilOpState BuildStencilOp(const RenderPipelineInfo& pipelineInfo, bool front);

		private:
			Vk::DeviceHandle m_device;
			RenderPipelineInfo m_pipelineInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPipeline.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPIPELINE_HPP
