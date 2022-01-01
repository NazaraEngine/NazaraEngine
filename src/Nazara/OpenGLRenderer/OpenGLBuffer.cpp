// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
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
		m_buffer.SubData(GLintptr(offset), GLsizeiptr(size), data);
		return true;
	}

	bool OpenGLBuffer::Initialize(UInt64 size, BufferUsageFlags usage)
	{
		m_size = size;
		m_usage = usage;

		GL::BufferTarget target;
		switch (m_type)
		{
			case BufferType::Index: target = GL::BufferTarget::ElementArray; break;
			case BufferType::Uniform: target = GL::BufferTarget::Uniform; break;
			case BufferType::Vertex: target = GL::BufferTarget::Array; break;

			default:
				throw std::runtime_error("unknown buffer type 0x" + NumberToString(UnderlyingCast(m_type), 16));
		}

		GLenum hint = GL_STREAM_COPY;

		if (usage & BufferUsage::Dynamic)
			hint = GL_DYNAMIC_DRAW;
		else if (usage & BufferUsage::DeviceLocal)
			hint = GL_STATIC_DRAW;

		if (usage & BufferUsage::DirectMapping)
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
		return DataStorage::Hardware;
	}

	void* OpenGLBuffer::Map(BufferAccess access, UInt64 offset, UInt64 size)
	{
		GLbitfield accessBit = 0;
		switch (access)
		{
			case BufferAccess::DiscardAndWrite:
				accessBit |= GL_MAP_WRITE_BIT;
				if (offset == 0 && size == m_size)
					accessBit |= GL_MAP_INVALIDATE_BUFFER_BIT;
				else
					accessBit |= GL_MAP_INVALIDATE_RANGE_BIT;

				break;

			case BufferAccess::ReadOnly:
				accessBit |= GL_MAP_READ_BIT;
				break;

			case BufferAccess::ReadWrite:
				accessBit |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
				break;

			case BufferAccess::WriteOnly:
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
