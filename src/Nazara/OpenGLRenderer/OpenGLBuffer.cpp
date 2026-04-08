// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <fmt/format.h>
#include <stdexcept>

namespace Nz
{
	OpenGLBuffer::OpenGLBuffer(OpenGLDevice& device, UInt64 size, BufferUsageFlags usage, const void* initialData) :
	RenderBuffer(device, size, usage)
	{
		if (!m_buffer.Create(device))
			throw std::runtime_error("failed to create buffer"); //< TODO: Handle OpenGL error

		// First binding may be used by driver as a hint of usage
		GL::BufferTarget target;
		if (usage & BufferUsage::IndexBuffer)
			target = GL::BufferTarget::ElementArray;
		else if (usage & BufferUsage::VertexBuffer)
			target = GL::BufferTarget::Array;
		else if (usage & BufferUsage::IndirectBuffer)
			target = GL::BufferTarget::DrawIndirect;
		else if (usage & BufferUsage::UniformBuffer)
			target = GL::BufferTarget::Uniform;
		else if (usage & BufferUsage::StorageBuffer)
			target = GL::BufferTarget::Storage;
		else if (usage & BufferUsage::TransferSource)
			target = GL::BufferTarget::CopyRead;
		else
			target = GL::BufferTarget::CopyWrite;

		const GL::Context& context = m_buffer.EnsureDeviceContext();
		if (context.IsExtensionSupported(GL::Extension::BufferStorage))
		{
			GLbitfield bitfield = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT;
			if (!usage.Test(BufferUsage::DeviceLocal))
				bitfield |= GL_CLIENT_STORAGE_BIT;

			if (!usage.Test(BufferUsage::MapSequentialWrite))
				bitfield |= GL_MAP_READ_BIT;

			if (usage.Test(BufferUsage::PersistentMapping))
				bitfield |= GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

			m_buffer.Storage(target, size, initialData, bitfield);
		}
		else
		{
			GLenum hint = GL_STREAM_COPY;

			if (usage.Test(BufferUsage::MapSequentialWrite))
				hint = GL_DYNAMIC_DRAW;
			else if (usage.Test(BufferUsage::DeviceLocal))
				hint = GL_STATIC_DRAW;

			if (usage.Test(BufferUsage::MemoryMapping))
				hint = GL_DYNAMIC_COPY;

			m_buffer.Data(target, size, initialData, hint);
		}
	}

	bool OpenGLBuffer::Fill(const void* data, UInt64 offset, UInt64 size)
	{
		m_buffer.SubData(GLintptr(offset), GLsizeiptr(size), data);
		return true;
	}

	bool OpenGLBuffer::Fill(AsyncRenderCommands& /*asyncTransfer*/, const void* data, UInt64 offset, UInt64 size)
	{
		return Fill(data, offset, size); //< TODO
	}

	void OpenGLBuffer::Flush(UInt64 /*offset*/, UInt64 /*size*/)
	{
		// When should we flush explicitly (since GL_MAP_FLUSH_EXPLICIT is set for persistent mapping)?
	}

	void* OpenGLBuffer::Map(UInt64 offset, UInt64 size)
	{
		GLbitfield accessBit = GL_MAP_WRITE_BIT;
		if (GetUsageFlags() & BufferUsage::MapSequentialWrite)
			accessBit |= GL_MAP_UNSYNCHRONIZED_BIT;
		else
			accessBit |= GL_MAP_READ_BIT;

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
