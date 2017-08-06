// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanBuffer::~VulkanBuffer() = default;

	bool VulkanBuffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		return m_softwareData.Fill(data, offset, size);
	}

	bool VulkanBuffer::Initialize(UInt32 size, BufferUsageFlags usage)
	{
		m_usage = usage;
		return m_softwareData.Initialize(size, usage);
	}

	DataStorage VulkanBuffer::GetStorage() const
	{
		return DataStorage_Hardware;
	}

	void* VulkanBuffer::Map(BufferAccess access, UInt32 offset, UInt32 size)
	{
		return m_softwareData.Map(access, offset, size);
	}

	bool VulkanBuffer::Unmap()
	{
		return m_softwareData.Unmap();
	}
}
