// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <exception>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SoftwareBuffer::SoftwareBuffer(Buffer* /*parent*/, BufferType /*type*/)
	{
	}

	bool SoftwareBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		NazaraAssert(!m_mapped, "Buffer is already mapped");

		std::memcpy(&m_buffer[offset], data, size);
		return true;
	}

	bool SoftwareBuffer::Initialize(UInt64 size, BufferUsageFlags /*usage*/)
	{
		// Protect the allocation to prevent a memory exception to escape the function
		try
		{
			m_buffer.resize(size);
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to allocate software buffer (" + String(e.what()) + ')');
			return false;
		}

		m_mapped = false;

		return true;
	}

	const UInt8* SoftwareBuffer::GetData() const
	{
		return m_buffer.data();
	}

	UInt64 SoftwareBuffer::GetSize() const
	{
		return UInt64(m_buffer.size());
	}

	DataStorage SoftwareBuffer::GetStorage() const
	{
		return DataStorage_Software;
	}

	void* SoftwareBuffer::Map(BufferAccess /*access*/, UInt64 offset, UInt64 /*size*/)
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
}
