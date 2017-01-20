// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	struct HashFletcher16_state
	{
		UInt16 sum1;
		UInt16 sum2;
	};

	HashFletcher16::HashFletcher16()
	{
		m_state = new HashFletcher16_state;
	}

	HashFletcher16::~HashFletcher16()
	{
		delete m_state;
	}

	void HashFletcher16::Append(const UInt8* data, std::size_t len)
	{
		while (len)
		{
			std::size_t tlen = std::min<std::size_t>(len, 21U);
			len -= tlen;
			do
			{
				m_state->sum1 += *data++;
				m_state->sum2 += m_state->sum1;
			}
			while (--tlen);

			m_state->sum1 = (m_state->sum1 & 0xff) + (m_state->sum1 >> 8);
			m_state->sum2 = (m_state->sum2 & 0xff) + (m_state->sum2 >> 8);
		}
	}

	void HashFletcher16::Begin()
	{
		m_state->sum1 = 0xff;
		m_state->sum2 = 0xff;
	}

	ByteArray HashFletcher16::End()
	{
		m_state->sum1 = (m_state->sum1 & 0xff) + (m_state->sum1 >> 8);
		m_state->sum2 = (m_state->sum2 & 0xff) + (m_state->sum2 >> 8);

		UInt32 fletcher = (m_state->sum2 << 8) | m_state->sum1;

		#ifdef NAZARA_BIG_ENDIAN
		SwapBytes(&fletcher, sizeof(UInt32));
		#endif

		return ByteArray(reinterpret_cast<UInt8*>(&fletcher), 2);
	}

	std::size_t HashFletcher16::GetDigestLength() const
	{
		return 2;
	}

	const char* HashFletcher16::GetHashName() const
	{
		return "Fletcher16";
	}
}
