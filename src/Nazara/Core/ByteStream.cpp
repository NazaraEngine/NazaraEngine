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
	/*!
	* \ingroup core
	* \class Nz::ByteStream
	* \brief Core class that represents a stream of bytes
	*/

	/*!
	* \brief Constructs a ByteStream object with a byte array
	*
	* \param byteArray Bytes to stream
	* \param openMode Reading/writing mode for the stream
	*/

	ByteStream::ByteStream(ByteArray* byteArray, UInt32 openMode) :
	ByteStream()
	{
		SetStream(byteArray, openMode);
	}

	/*!
	* \brief Constructs a ByteStream object with a raw memory and a size
	*
	* \param ptr Pointer to raw memory
	* \param size Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	ByteStream::ByteStream(void* ptr, Nz::UInt64 size) :
	ByteStream()
	{
		SetStream(ptr, size);
	}

	/*!
	* \brief Constructs a ByteStream object with a raw memory and a size
	*
	* \param ptr Constant pointer to raw memory
	* \param size Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	ByteStream::ByteStream(const void* ptr, Nz::UInt64 size) :
	ByteStream()
	{
		SetStream(ptr, size);
	}

	/*!
	* \brief Sets this with a byte array
	*
	* \param byteArray Bytes to stream
	* \param openMode Reading/writing mode for the stream
	*/

	void ByteStream::SetStream(ByteArray* byteArray, UInt32 openMode)
	{
		std::unique_ptr<Stream> stream(new MemoryStream(byteArray, openMode));

		SetStream(stream.get());
		// SetStream reset our smart pointer, set it after calling it
		m_ownedStream = std::move(stream);
	}

	/*!
	* \brief Sets this with a raw memory and a size
	*
	* \param ptr Pointer to raw memory
	* \param size Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	void ByteStream::SetStream(void* ptr, Nz::UInt64 size)
	{
		std::unique_ptr<Stream> stream(new MemoryView(ptr, size));

		SetStream(stream.get());
		// SetStream reset our smart pointer, set it after calling it
		m_ownedStream = std::move(stream);
	}

	/*!
	* \brief Sets this with a raw memory and a size
	*
	* \param ptr Constant pointer to raw memory
	* \param size Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	void ByteStream::SetStream(const void* ptr, Nz::UInt64 size)
	{
		std::unique_ptr<Stream> stream(new MemoryView(ptr, size));

		SetStream(stream.get());
		// SetStream reset our smart pointer, set it after calling it
		m_ownedStream = std::move(stream);
	}

	/*!
	* \brief Signal function (meant to be virtual)
	*
	* \remark Produces a NazaraError
	*/

	void ByteStream::OnEmptyStream()
	{
		NazaraError("No stream");
	}
}
