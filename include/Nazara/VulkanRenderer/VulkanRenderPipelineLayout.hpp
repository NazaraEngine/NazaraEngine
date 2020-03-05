// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSetLayout.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderPipelineLayout : public RenderPipelineLayout
	{
		public:
			VulkanRenderPipelineLayout() = default;
			~VulkanRenderPipelineLayout() = default;

			bool Create(Vk::DeviceHandle device, RenderPipelineLayoutInfo layoutInfo);

			inline const Vk::DescriptorSetLayout& GetDescriptorSetLayout() const;
			inline const Vk::PipelineLayout& GetPipelineLayout() const;

		private:
			Vk::DeviceHandle m_device;
			Vk::DescriptorSetLayout m_descriptorSetLayout;
			Vk::PipelineLayout m_pipelineLayout;
			RenderPipelineLayoutInfo m_layoutInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPIPELINE_HPP
