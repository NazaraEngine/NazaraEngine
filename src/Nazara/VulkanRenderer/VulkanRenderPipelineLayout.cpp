// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	bool VulkanRenderPipelineLayout::Create(Vk::DeviceHandle device, RenderPipelineLayoutInfo layoutInfo)
	{
		m_device = std::move(device);
		m_layoutInfo = std::move(layoutInfo);

		StackVector<VkDescriptorSetLayoutBinding> layoutBindings = NazaraStackVector(VkDescriptorSetLayoutBinding, m_layoutInfo.bindings.size());

		for (const auto& bindingInfo : m_layoutInfo.bindings)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.binding = bindingInfo.index;
			layoutBinding.descriptorCount = 1U;
			layoutBinding.descriptorType = ToVulkan(bindingInfo.type);
			layoutBinding.stageFlags = ToVulkan(bindingInfo.shaderStageFlags);
		}

		if (!m_descriptorSetLayout.Create(m_device, layoutBindings.size(), layoutBindings.data()))
			return false;

		if (!m_pipelineLayout.Create(m_device, m_descriptorSetLayout))
			return false;

		return true;
	}
}
