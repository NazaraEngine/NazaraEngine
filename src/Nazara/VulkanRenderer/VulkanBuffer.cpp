// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/String.hpp>
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
		VkBufferUsageFlags type;
		switch (m_type)
		{
			case BufferType_Index:
				type = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				break;

			case BufferType_Vertex:
				type = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				break;

			case BufferType_Uniform:
				type = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				break;

			default:
				NazaraError("Unhandled buffer usage 0x" + String::Number(m_type, 16));
				return false;
		}

		if (!m_buffer.Create(m_device, 0, size, type))
		{
			NazaraError("Failed to create vertex buffer");
			return false;
		}

		VkMemoryRequirements memRequirement = m_buffer.GetMemoryRequirements();

		if (!m_memory.Create(m_device, memRequirement.size, memRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			NazaraError("Failed to allocate vertex buffer memory");
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
		if (!m_memory.Map(offset, size))
			return nullptr;

		return m_memory.GetMappedPointer();
	}

	bool VulkanBuffer::Unmap()
	{
		m_memory.Unmap();
		return true;
	}
}
