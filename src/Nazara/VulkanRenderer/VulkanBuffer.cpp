// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <vma/vk_mem_alloc.h>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanBuffer::VulkanBuffer(VulkanDevice& device, BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData) :
	RenderBuffer(device, type, size, usage),
	m_device(device)
	{
		VkBufferUsageFlags bufferUsage = ToVulkan(type);

		if ((usage & BufferUsage::DirectMapping) == 0)
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = bufferUsage;

		//TODO: Update for VMA 3.0
		VmaAllocationCreateInfo allocInfo = {};
		if (type == BufferType::Upload)
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		else if (usage & BufferUsage::DeviceLocal)
		{
			if (usage & BufferUsage::DirectMapping)
				allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			else
				allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		}
		else
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		if (usage & BufferUsage::PersistentMapping)
			allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VkResult result = vmaCreateBuffer(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);
		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to allocate buffer: " + TranslateVulkanError(result));

		if (initialData)
		{
			if (!Fill(initialData, 0, size))
				throw std::runtime_error("failed to fill buffer");
		}
	}

	VulkanBuffer::~VulkanBuffer()
	{
		vmaDestroyBuffer(m_device.GetMemoryAllocator(), m_buffer, m_allocation);
	}

	bool VulkanBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		void* ptr = Map(offset, size);
		if (!ptr)
			return false;

		CallOnExit unmapOnExit([this]() { Unmap(); });

		std::memcpy(ptr, data, size);
		return true;
	}

	void* VulkanBuffer::Map(UInt64 offset, UInt64 size)
	{
		if (GetUsageFlags() & BufferUsage::DirectMapping)
		{
			void* mappedPtr;
			VkResult result = vmaMapMemory(m_device.GetMemoryAllocator(), m_allocation, &mappedPtr);
			if (result != VK_SUCCESS)
			{
				NazaraErrorFmt("failed to map buffer: {0}", TranslateVulkanError(result));
				return nullptr;
			}

			return static_cast<UInt8*>(mappedPtr) + offset;
		}
		else
		{
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.size = size;
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

			VmaAllocationInfo allocationInfo;

			VkResult result = vmaCreateBuffer(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &m_stagingBuffer, &m_stagingAllocation, &allocationInfo);
			if (result != VK_SUCCESS)
			{
				NazaraErrorFmt("failed to allocate staging buffer: {0}", TranslateVulkanError(result));
				return nullptr;
			}

			m_stagingBufferSize = size;

			return allocationInfo.pMappedData;
		}
	}

	bool VulkanBuffer::Unmap()
	{
		if (GetUsageFlags() & BufferUsage::DirectMapping)
		{
			vmaUnmapMemory(m_device.GetMemoryAllocator(), m_allocation);
			return true;
		}
		else
		{
			Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Transfer);
			if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
				return false;

			copyCommandBuffer->CopyBuffer(m_stagingBuffer, m_buffer, m_stagingBufferSize);
			if (!copyCommandBuffer->End())
				return false;

			Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetDefaultFamilyIndex(QueueType::Transfer), 0);
			if (!transferQueue.Submit(copyCommandBuffer))
				return false;

			transferQueue.WaitIdle();

			vmaDestroyBuffer(m_device.GetMemoryAllocator(), m_stagingBuffer, m_stagingAllocation);
			return true;
		}
	}

	void VulkanBuffer::UpdateDebugName(std::string_view name)
	{
		return m_device.SetDebugName(VK_OBJECT_TYPE_BUFFER, VulkanHandleToInteger(m_buffer), name);
	}
}

// vma includes vulkan.h which includes system headers
#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
#include <Nazara/Core/AntiX11.hpp>
#endif
