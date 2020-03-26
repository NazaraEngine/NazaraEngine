// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <vma/vk_mem_alloc.h>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanBuffer::~VulkanBuffer()
	{
		vmaDestroyBuffer(m_device.GetMemoryAllocator(), m_buffer, m_allocation);
	}

	bool VulkanBuffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		void* ptr = Map(BufferAccess_WriteOnly, offset, size);
		if (!ptr)
			return false;

		Nz::CallOnExit unmapOnExit([this]() { Unmap(); });

		std::memcpy(ptr, data, size);

		return true;
	}

	bool VulkanBuffer::Initialize(UInt32 size, BufferUsageFlags usage)
	{
		m_size = size;
		m_usage = usage;

		VkBufferUsageFlags bufferUsage = ToVulkan(m_type);

		if ((usage & BufferUsage_DirectMapping) == 0)
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = bufferUsage;

		VmaAllocationCreateInfo allocInfo = {};
		if (usage & BufferUsage_DeviceLocal)
		{
			if (usage & BufferUsage_DirectMapping)
				allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			else
				allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		}
		else
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		if (usage & BufferUsage_PersistentMapping)
			allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VkResult result = vmaCreateBuffer(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);
		if (result != VK_SUCCESS)
		{
			NazaraError("Failed to allocate buffer: " + TranslateVulkanError(result));
			return false;
		}

		return true;
	}

	DataStorage VulkanBuffer::GetStorage() const
	{
		return DataStorage_Hardware;
	}

	void* VulkanBuffer::Map(BufferAccess /*access*/, UInt32 offset, UInt32 size)
	{
		if (m_usage & BufferUsage_DirectMapping)
		{
			void* mappedPtr;
			VkResult result = vmaMapMemory(m_device.GetMemoryAllocator(), m_allocation, &mappedPtr);
			if (result != VK_SUCCESS)
			{
				NazaraError("Failed to map buffer: " + TranslateVulkanError(result));
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
				NazaraError("Failed to allocate staging buffer: " + TranslateVulkanError(result));
				return nullptr;
			}

			return allocationInfo.pMappedData;
		}
	}

	bool VulkanBuffer::Unmap()
	{
		if (m_usage & BufferUsage_DirectMapping)
		{
			vmaUnmapMemory(m_device.GetMemoryAllocator(), m_allocation);
			return true;
		}
		else
		{
			Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateTransferCommandBuffer();
			if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
				return false;

			copyCommandBuffer->CopyBuffer(m_stagingBuffer, m_buffer, m_size);
			if (!copyCommandBuffer->End())
				return false;

			Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetTransferQueueFamilyIndex(), 0);
			if (!transferQueue.Submit(copyCommandBuffer))
				return false;

			transferQueue.WaitIdle();

			vmaDestroyBuffer(m_device.GetMemoryAllocator(), m_stagingBuffer, m_stagingAllocation);
			return true;
		}
	}
}
