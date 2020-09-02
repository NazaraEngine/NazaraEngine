// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/String.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	
	OpenGLBuffer::OpenGLBuffer(OpenGLDevice& device, BufferType type) :
	m_type(type)
	{
		if (!m_buffer.Create(device))
			throw std::runtime_error("failed to create buffer"); //< TODO: Handle error
	}

	bool OpenGLBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		m_buffer.SubData(offset, size, data);
		return true;
	}

	bool OpenGLBuffer::Initialize(UInt64 size, BufferUsageFlags usage)
	{
		m_size = size;
		m_usage = usage;

		GL::BufferTarget target;
		switch (m_type)
		{
			case BufferType_Index: target = GL::BufferTarget::ElementArray; break;
			case BufferType_Uniform: target = GL::BufferTarget::Uniform; break;
			case BufferType_Vertex: target = GL::BufferTarget::Array; break;

			default:
				throw std::runtime_error("unknown buffer type 0x" + String::Number(UnderlyingCast(m_type), 16).ToStdString());
		}

		GLenum hint = GL_STREAM_COPY;

		if (usage & BufferUsage_Dynamic)
			hint = GL_DYNAMIC_DRAW;
		else if (usage & BufferUsage_DeviceLocal)
			hint = GL_STATIC_DRAW;

		if (usage & BufferUsage_DirectMapping)
			hint = GL_DYNAMIC_COPY;

		m_buffer.Reset(target, size, nullptr, hint);
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

	void* OpenGLBuffer::Map(BufferAccess access, UInt64 offset, UInt64 size)
	{
		GLbitfield accessBit = 0;
		switch (access)
		{
			case BufferAccess_DiscardAndWrite:
				accessBit |= GL_MAP_WRITE_BIT;
				if (offset == 0 && size == m_size)
					accessBit |= GL_MAP_INVALIDATE_BUFFER_BIT;
				else
					accessBit |= GL_MAP_INVALIDATE_RANGE_BIT;

				break;

			case BufferAccess_ReadOnly:
				accessBit |= GL_MAP_READ_BIT;
				break;

			case BufferAccess_ReadWrite:
				accessBit |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
				break;

			case BufferAccess_WriteOnly:
				accessBit |= GL_MAP_WRITE_BIT;
				break;

			default:
				break;
		}

		return m_buffer.MapRange(offset, size, accessBit);
	}

	bool OpenGLBuffer::Unmap()
	{
		return m_buffer.Unmap();
	}
}
