// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/PoolByteStream.hpp>
#include <Nazara/Core/ByteArrayPool.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::PoolByteStream
	* \brief ByteStream allocated using a pool
	*/

	void PoolByteStream::Reset()
	{
		if (m_buffer.GetCapacity() > 0)
		{
			m_pool.ReturnByteArray(std::move(m_buffer));
			m_buffer.Clear(false);
		}

		SetStream(static_cast<Nz::Stream*>(nullptr));
	}

	void PoolByteStream::Reset(std::size_t capacity)
	{
		if (m_buffer.GetCapacity() < capacity)
		{
			if (m_buffer.GetCapacity() > 0)
				m_pool.ReturnByteArray(std::move(m_buffer));

			m_buffer = m_pool.GetByteArray(capacity);
			SetStream(&m_buffer, Nz::OpenMode_ReadWrite);
		}
	}

	void PoolByteStream::OnEmptyStream()
	{
		Reset(0);
	}
}
