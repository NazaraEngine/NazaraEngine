// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <fmt/format.h>
#include <stdexcept>

namespace Nz
{
	OpenGLBuffer::OpenGLBuffer(OpenGLDevice& device, BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData) :
	RenderBuffer(device, type, size, usage)
	{
		if (!m_buffer.Create(device))
			throw std::runtime_error("failed to create buffer"); //< TODO: Handle OpenGL error

		GL::BufferTarget target;
		switch (type)
		{
			case BufferType::Index:   target = GL::BufferTarget::ElementArray; break;
			case BufferType::Storage: target = GL::BufferTarget::Storage; break;
			case BufferType::Uniform: target = GL::BufferTarget::Uniform; break;
			case BufferType::Vertex:  target = GL::BufferTarget::Array; break;

			default:
				throw std::runtime_error(fmt::format("unknown buffer type {0:#x}", UnderlyingCast(type)));
		}

		const GL::Context& context = m_buffer.EnsureDeviceContext();
		if (context.IsExtensionSupported(GL::Extension::BufferStorage))
		{
			GLbitfield bitfield = GL_DYNAMIC_STORAGE_BIT;
			if (!usage.Test(BufferUsage::DeviceLocal))
				bitfield |= GL_CLIENT_STORAGE_BIT;

			if (usage.Test(BufferUsage::Read))
				bitfield |= GL_MAP_READ_BIT;

			if (usage.Test(BufferUsage::PersistentMapping))
				bitfield |= GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

			if (usage.Test(BufferUsage::Write))
				bitfield |= GL_MAP_WRITE_BIT;

			m_buffer.Storage(target, size, initialData, bitfield);
		}
		else
		{
			GLenum hint = GL_STREAM_COPY;

			if (usage.Test(BufferUsage::Dynamic))
				hint = GL_DYNAMIC_DRAW;
			else if (usage.Test(BufferUsage::DeviceLocal))
				hint = GL_STATIC_DRAW;

			if (usage.Test(BufferUsage::DirectMapping))
				hint = GL_DYNAMIC_COPY;

			m_buffer.Data(target, size, initialData, hint);
		}
	}

	bool OpenGLBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		m_buffer.SubData(GLintptr(offset), GLsizeiptr(size), data);
		return true;
	}

	void* OpenGLBuffer::Map(UInt64 offset, UInt64 size)
	{
		GLbitfield accessBit = 0;
		if (GetUsageFlags() & BufferUsage::Read)
			accessBit |= GL_MAP_READ_BIT;
		else
			accessBit |= GL_MAP_UNSYNCHRONIZED_BIT;

		if (GetUsageFlags() & BufferUsage::Write)
			accessBit |= GL_MAP_WRITE_BIT;

		if (GetUsageFlags() & BufferUsage::PersistentMapping)
			accessBit |= GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

		return m_buffer.MapRange(offset, size, accessBit);
	}

	bool OpenGLBuffer::Unmap()
	{
		return m_buffer.Unmap();
	}

	void OpenGLBuffer::UpdateDebugName(std::string_view name)
	{
		m_buffer.SetDebugName(name);
	}
}
