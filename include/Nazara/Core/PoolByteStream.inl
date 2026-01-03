// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <cstring>

namespace Nz
{
	inline PoolByteStream::PoolByteStream(ByteArrayPool& pool) :
	m_pool(pool)
	{
	}

	inline PoolByteStream::PoolByteStream(ByteArrayPool& pool, std::size_t capacity) :
	PoolByteStream(pool)
	{
		Reset(capacity);
	}

	inline PoolByteStream::~PoolByteStream()
	{
		FlushBits(); //< Needs to be done here as the stream will be freed before ByteStream calls it
		Reset(); //< Returns the byte array (if any) to the pool
	}
}
