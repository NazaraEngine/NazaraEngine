// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSet.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSetLayout.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PipelineLayout.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderPipelineLayout : public RenderPipelineLayout
	{
		public:
			VulkanRenderPipelineLayout() = default;
			~VulkanRenderPipelineLayout() = default;

			Vk::DescriptorSet AllocateDescriptorSet();

			bool Create(Vk::Device& device, RenderPipelineLayoutInfo layoutInfo);

			inline const Vk::DescriptorSetLayout& GetDescriptorSetLayout() const;
			inline const Vk::PipelineLayout& GetPipelineLayout() const;

		private:
			struct DescriptorPool
			{
				Vk::DescriptorPool descriptorPool;
			};

			MovablePtr<Vk::Device> m_device;
			std::vector<DescriptorPool> m_descriptorPools;
			Vk::DescriptorSetLayout m_descriptorSetLayout;
			Vk::PipelineLayout m_pipelineLayout;
			RenderPipelineLayoutInfo m_layoutInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPIPELINE_HPP
