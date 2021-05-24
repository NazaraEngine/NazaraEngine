// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	Buffer::Buffer(BufferType type) :
	m_type(type),
	m_usage(0),
	m_size(0)
	{
	}

	Buffer::Buffer(BufferType type, UInt32 size, DataStorage storage, BufferUsageFlags usage) :
	Buffer(type)
	{
		ErrorFlags flags(ErrorMode::ThrowException, true);

		Create(size, storage, usage);
	}

	bool Buffer::CopyContent(const Buffer& buffer)
	{
		NazaraAssert(m_impl, "Invalid buffer");
		NazaraAssert(buffer.IsValid(), "Invalid source buffer");

		BufferMapper<Buffer> mapper(buffer, BufferAccess::ReadOnly);
		return Fill(mapper.GetPointer(), 0, buffer.GetSize());
	}

	bool Buffer::Create(UInt32 size, DataStorage storage, BufferUsageFlags usage)
	{
		Destroy();

		// Notre buffer est-il supporté ?
		if (!IsStorageSupported(storage))
		{
			NazaraError("Buffer storage not supported");
			return false;
		}

		std::unique_ptr<AbstractBuffer> impl = s_bufferFactories[UnderlyingCast(storage)](this, m_type);
		if (!impl->Initialize(size, usage))
		{
			NazaraError("Failed to create buffer");
			return false;
		}

		m_impl = std::move(impl);
		m_size = size;
		m_usage = usage;

		return true; // Si on arrive ici c'est que tout s'est bien passé.
	}

	void Buffer::Destroy()
	{
		m_impl.reset();
	}

	bool Buffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_impl, "Invalid buffer");
		NazaraAssert(offset + size <= m_size, "Exceeding buffer size");

		return m_impl->Fill(data, offset, (size == 0) ? m_size - offset : size);
	}

	void* Buffer::Map(BufferAccess access, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_impl, "Invalid buffer");
		NazaraAssert(offset + size <= m_size, "Exceeding buffer size");

		return m_impl->Map(access, offset, (size == 0) ? m_size - offset : size);
	}

	void* Buffer::Map(BufferAccess access, UInt32 offset, UInt32 size) const
	{
		NazaraAssert(m_impl, "Invalid buffer");
		NazaraAssert(access == BufferAccess::ReadOnly, "Buffer access must be read-only when used const");
		NazaraAssert(offset + size <= m_size, "Exceeding buffer size");

		return m_impl->Map(access, offset, (size == 0) ? m_size - offset : size);
	}

	bool Buffer::SetStorage(DataStorage storage)
	{
		NazaraAssert(m_impl, "Invalid buffer");

		if (HasStorage(storage))
			return true;

		if (!IsStorageSupported(storage))
		{
			NazaraError("Storage not supported");
			return false;
		}

		void* ptr = m_impl->Map(BufferAccess::ReadOnly, 0, m_size);
		if (!ptr)
		{
			NazaraError("Failed to map buffer");
			return false;
		}

		CallOnExit unmapMyImpl([this]()
		{
			m_impl->Unmap();
		});

		std::unique_ptr<AbstractBuffer> impl(s_bufferFactories[UnderlyingCast(storage)](this, m_type));
		if (!impl->Initialize(m_size, m_usage))
		{
			NazaraError("Failed to create buffer");
			return false;
		}

		if (!impl->Fill(ptr, 0, m_size))
		{
			NazaraError("Failed to fill buffer");
			return false;
		}

		unmapMyImpl.CallAndReset();

		m_impl = std::move(impl);

		return true;
	}

	void Buffer::Unmap() const
	{
		NazaraAssert(m_impl, "Invalid buffer");

		if (!m_impl->Unmap())
			NazaraWarning("Failed to unmap buffer (it's content may be undefined)"); ///TODO: Unexpected ?
	}

	bool Buffer::IsStorageSupported(DataStorage storage)
	{
		return s_bufferFactories[UnderlyingCast(storage)] != nullptr;
	}

	void Buffer::SetBufferFactory(DataStorage storage, BufferFactory func)
	{
		s_bufferFactories[UnderlyingCast(storage)] = func;
	}

	bool Buffer::Initialize()
	{
		SetBufferFactory(DataStorage::Software, [](Buffer* parent, BufferType type) -> std::unique_ptr<AbstractBuffer>
		{
			return std::make_unique<SoftwareBuffer>(parent, type);
		});

		return true;
	}

	void Buffer::Uninitialize()
	{
		std::fill(s_bufferFactories.begin(), s_bufferFactories.end(), nullptr);
	}

	std::array<Buffer::BufferFactory, DataStorageCount> Buffer::s_bufferFactories;
}
