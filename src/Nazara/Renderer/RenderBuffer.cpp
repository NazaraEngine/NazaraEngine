// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	bool RenderBuffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		if (m_softwareBuffer.Fill(data, offset, size))
		{
			for (auto& bufferPair : m_hardwareBuffers)
				bufferPair.second.synchronized = false;

			return true;
		}
		else
			return false;
	}

	bool RenderBuffer::Initialize(UInt32 size, BufferUsageFlags usage)
	{
		m_size = size;
		m_softwareBuffer.Initialize(size, usage);

		return true;
	}

	AbstractBuffer* RenderBuffer::GetHardwareBuffer(RenderDevice* device)
	{
		auto it = m_hardwareBuffers.find(device);
		if (it == m_hardwareBuffers.end())
			return nullptr;

		return it->second.buffer.get();
	}

	DataStorage RenderBuffer::GetStorage() const
	{
		return DataStorage::DataStorage_Hardware;
	}

	void* RenderBuffer::Map(BufferAccess access, UInt32 offset, UInt32 size)
	{
		if (void* ptr = m_softwareBuffer.Map(access, offset, size))
		{
			if (access != BufferAccess_ReadOnly)
			{
				for (auto& bufferPair : m_hardwareBuffers)
					bufferPair.second.synchronized = false;
			}

			return ptr;
		}
		else
			return nullptr;
	}

	bool RenderBuffer::Unmap()
	{
		return m_softwareBuffer.Unmap();
	}

	bool RenderBuffer::Synchronize(RenderDevice* device)
	{
		auto it = m_hardwareBuffers.find(device);
		if (it == m_hardwareBuffers.end())
		{
			HardwareBuffer hwBuffer;
			hwBuffer.buffer = device->InstantiateBuffer(m_parent, m_type);
			if (!hwBuffer.buffer->Initialize(m_size, m_usage))
			{
				NazaraError("Failed to initialize hardware buffer");
				return false;
			}

			it = m_hardwareBuffers.emplace(device, std::move(hwBuffer)).first;
		}

		HardwareBuffer& hwBuffer = it->second;
		if (hwBuffer.synchronized)
			return true;

		return hwBuffer.buffer->Fill(m_softwareBuffer.GetData(), 0, m_size);
	}
}
