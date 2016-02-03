// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ByteStream::ByteStream(ByteArray* byteArray, UInt32 openMode) :
	ByteStream()
	{
		SetStream(byteArray, openMode);
	}

	ByteStream::ByteStream(void* ptr, Nz::UInt64 size) :
	ByteStream()
	{
		SetStream(ptr, size);
	}

	ByteStream::ByteStream(const void* ptr, Nz::UInt64 size) :
	ByteStream()
	{
		SetStream(ptr, size);
	}

	void ByteStream::SetStream(ByteArray* byteArray, UInt32 openMode)
	{
		std::unique_ptr<Stream> stream(new MemoryStream(byteArray, openMode));

		SetStream(m_ownedStream.get());
		// SetStream reset our smart pointer, set it after calling it
		m_ownedStream = std::move(stream);
	}

	void ByteStream::SetStream(void* ptr, Nz::UInt64 size)
	{
		std::unique_ptr<Stream> stream(new MemoryView(ptr, size));

		SetStream(m_ownedStream.get());
		// SetStream reset our smart pointer, set it after calling it
		m_ownedStream = std::move(stream);
	}

	void ByteStream::SetStream(const void* ptr, Nz::UInt64 size)
	{
		std::unique_ptr<Stream> stream(new MemoryView(ptr, size));

		SetStream(m_ownedStream.get());
		// SetStream reset our smart pointer, set it after calling it
		m_ownedStream = std::move(stream);
	}

	void ByteStream::OnEmptyStream()
	{
		NazaraError("No stream");
	}
}
