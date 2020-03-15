// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanBuffer::~VulkanBuffer() = default;

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
		VkMemoryPropertyFlags memoryProperties = 0;
		if (usage & BufferUsage_DeviceLocal)
			memoryProperties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (usage & BufferUsage_DirectMapping)
			memoryProperties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		else
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if (!m_buffer.Create(m_device, 0, size, bufferUsage))
		{
			NazaraError("Failed to create vulkan buffer");
			return false;
		}

		VkMemoryRequirements memRequirement = m_buffer.GetMemoryRequirements();

		if (!m_memory.Create(m_device, memRequirement.size, memRequirement.memoryTypeBits, memoryProperties))
		{
			NazaraError("Failed to allocate buffer memory");
			return false;
		}

		if (!m_buffer.BindBufferMemory(m_memory))
		{
			NazaraError("Failed to bind vertex buffer to its memory");
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
			if (!m_memory.Map(offset, size))
				return nullptr;

			return m_memory.GetMappedPointer();
		}
		else
		{
			if (!m_stagingBuffer.Create(m_device, 0, m_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT))
			{
				NazaraError("Failed to create staging buffer");
				return nullptr;
			}

			VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

			VkMemoryRequirements memRequirement = m_stagingBuffer.GetMemoryRequirements();
			if (!m_stagingMemory.Create(m_device, memRequirement.size, memRequirement.memoryTypeBits, memoryProperties))
			{
				NazaraError("Failed to allocate vertex buffer memory");
				return nullptr;
			}

			if (!m_stagingBuffer.BindBufferMemory(m_stagingMemory))
			{
				NazaraError("Failed to bind vertex buffer to its memory");
				return nullptr;
			}

			if (!m_stagingMemory.Map(offset, size))
				return nullptr;

			return m_stagingMemory.GetMappedPointer();
		}
	}

	bool VulkanBuffer::Unmap()
	{
		if (m_usage & BufferUsage_DirectMapping)
		{
			m_memory.Unmap();
			return true;
		}
		else
		{
			m_stagingMemory.FlushMemory();
			m_stagingMemory.Unmap();

			Vk::CommandBuffer copyCommandBuffer = m_device.AllocateTransferCommandBuffer();
			if (!copyCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
				return false;

			copyCommandBuffer.CopyBuffer(m_stagingBuffer, m_buffer, m_size);
			if (!copyCommandBuffer.End())
				return false;

			Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetTransferQueueFamilyIndex(), 0);
			if (!transferQueue.Submit(copyCommandBuffer))
				return false;

			transferQueue.WaitIdle();

			m_stagingBuffer.Destroy();
			m_stagingMemory.Destroy();
			return true;
		}
	}
}
