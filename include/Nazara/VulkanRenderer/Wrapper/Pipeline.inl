// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Utils.hpp>
#include <NazaraUtils/StackArray.hpp>

namespace Nz::Vk
{
	inline Pipeline::Pipeline() :
	m_handle(VK_NULL_HANDLE)
	{
	}

	inline Pipeline::Pipeline(Pipeline&& object) noexcept :
	m_device(std::move(object.m_device)),
	m_allocator(object.m_allocator),
	m_handle(object.m_handle),
	m_lastErrorCode(object.m_lastErrorCode)
	{
		object.m_handle = VK_NULL_HANDLE;
	}

	inline Pipeline::~Pipeline()
	{
		Destroy();
	}

	inline bool Pipeline::CreateCompute(Device& device, const VkComputePipelineCreateInfo& createInfo, VkPipelineCache cache, const VkAllocationCallbacks* allocator)
	{
		return Create(device, device.vkCreateComputePipelines(device, cache, 1U, &createInfo, allocator, &m_handle), allocator);
	}

	inline bool Pipeline::CreateGraphics(Device& device, const VkGraphicsPipelineCreateInfo& createInfo, VkPipelineCache cache, const VkAllocationCallbacks* allocator)
	{
		return Create(device, device.vkCreateGraphicsPipelines(device, cache, 1U, &createInfo, allocator, &m_handle), allocator);
	}

	inline void Pipeline::Destroy()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			m_device->vkDestroyPipeline(*m_device, m_handle, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
			m_handle = VK_NULL_HANDLE;
		}
	}

	inline Device& Pipeline::GetDevice() const
	{
		return *m_device;
	}

	inline VkResult Pipeline::GetLastErrorCode() const
	{
		return m_lastErrorCode;
	}

	inline void Pipeline::SetDebugName(std::string_view name)
	{
		return m_device->SetDebugName(VK_OBJECT_TYPE_PIPELINE, VulkanHandleToInteger(m_handle), name);
	}

	inline Pipeline::operator VkPipeline() const
	{
		return m_handle;
	}

	inline bool Pipeline::Create(Device& device, VkResult result, const VkAllocationCallbacks* allocator)
	{
		m_device = &device;
		m_lastErrorCode = result;
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraErrorFmt("failed to create Vulkan pipeline: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		// Store the allocator to access them when needed
		if (allocator)
			m_allocator = *allocator;
		else
			m_allocator.pfnAllocation = nullptr;

		return true;
	}
}

