// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <exception>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SoftwareBuffer::SoftwareBuffer(BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData) :
	Buffer(DataStorage::Software, type, size, usage | BufferUsage::DirectMapping | BufferUsage::Dynamic | BufferUsage::PersistentMapping | BufferUsage::Read | BufferUsage::Write),
	m_mapped(false)
	{
		m_buffer = std::make_unique<UInt8[]>(size);
		if (initialData)
			std::memcpy(&m_buffer[0], initialData, size);
	}

	bool SoftwareBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		NazaraAssert(!m_mapped, "Buffer is already mapped");

		std::memcpy(&m_buffer[offset], data, size);
		return true;
	}

	const UInt8* SoftwareBuffer::GetData() const
	{
		return &m_buffer[0];
	}

	void* SoftwareBuffer::Map(UInt64 offset, UInt64 /*size*/)
	{
		NazaraAssert(!m_mapped, "Buffer is already mapped");

		m_mapped = true;

		return &m_buffer[offset];
	}

	bool SoftwareBuffer::Unmap()
	{
		NazaraAssert(m_mapped, "Buffer is not mapped");

		m_mapped = false;

		return true;
	}

	std::shared_ptr<Buffer> SoftwareBufferFactory(BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData)
	{
		return std::make_shared<SoftwareBuffer>(type, size, usage, initialData);
	}
}
