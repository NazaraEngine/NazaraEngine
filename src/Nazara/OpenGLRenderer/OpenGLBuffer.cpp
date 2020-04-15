// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/QueueHandle.hpp>
#include <vma/vk_mem_alloc.h>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLBuffer::~OpenGLBuffer()
	{
		vmaDestroyBuffer(m_device.GetMemoryAllocator(), m_buffer, m_allocation);
	}

	bool OpenGLBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		void* ptr = Map(BufferAccess_WriteOnly, offset, size);
		if (!ptr)
			return false;

		Nz::CallOnExit unmapOnExit([this]() { Unmap(); });

		std::memcpy(ptr, data, size);

		return true;
	}

	bool OpenGLBuffer::Initialize(UInt64 size, BufferUsageFlags usage)
	{
		m_size = size;
		m_usage = usage;

		VkBufferUsageFlags bufferUsage = ToOpenGL(m_type);

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
			NazaraError("Failed to allocate buffer: " + TranslateOpenGLError(result));
			return false;
		}

		return true;
	}

	UInt64 OpenGLBuffer::GetSize() const
	{
		return m_size;
	}

	DataStorage OpenGLBuffer::GetStorage() const
	{
		return DataStorage_Hardware;
	}

	void* OpenGLBuffer::Map(BufferAccess /*access*/, UInt64 offset, UInt64 size)
	{
		if (m_usage & BufferUsage_DirectMapping)
		{
			void* mappedPtr;
			VkResult result = vmaMapMemory(m_device.GetMemoryAllocator(), m_allocation, &mappedPtr);
			if (result != VK_SUCCESS)
			{
				NazaraError("Failed to map buffer: " + TranslateOpenGLError(result));
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
				NazaraError("Failed to allocate staging buffer: " + TranslateOpenGLError(result));
				return nullptr;
			}

			return allocationInfo.pMappedData;
		}
	}

	bool OpenGLBuffer::Unmap()
	{
		if (m_usage & BufferUsage_DirectMapping)
		{
			vmaUnmapMemory(m_device.GetMemoryAllocator(), m_allocation);
			return true;
		}
		else
		{
			Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Transfer);
			if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
				return false;

			copyCommandBuffer->CopyBuffer(m_stagingBuffer, m_buffer, m_size);
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
}

#endif
