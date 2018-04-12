// Copyright (C) 2017 Jérôme Leclercq
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

	SoftwareBuffer::~SoftwareBuffer()
	{
	}

	bool SoftwareBuffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		NazaraAssert(!m_mapped, "Buffer is already mapped");

		std::memcpy(&m_buffer[offset], data, size);
		return true;
	}

	bool SoftwareBuffer::Initialize(UInt32 size, BufferUsageFlags /*usage*/)
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

	DataStorage SoftwareBuffer::GetStorage() const
	{
		return DataStorage_Software;
	}

	void* SoftwareBuffer::Map(BufferAccess /*access*/, UInt32 offset, UInt32 /*size*/)
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
