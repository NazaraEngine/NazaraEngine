// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <NazaraUtils/Endianness.hpp>

namespace Nz
{
	void Fletcher16Hasher::Append(const UInt8* data, std::size_t len)
	{
		while (len > 0)
		{
			std::size_t tlen = std::min<std::size_t>(len, 20U);
			len -= tlen;

			do
			{
				m_sum1 += *data++;
				m_sum2 += m_sum1;
			}
			while (--tlen);

			m_sum1 = (m_sum1 & 0xFF) + (m_sum1 >> 8);
			m_sum2 = (m_sum2 & 0xFF) + (m_sum2 >> 8);
		}
	}

	void Fletcher16Hasher::Begin()
	{
		m_sum1 = 0xFF;
		m_sum2 = 0xFF;
	}

	ByteArray Fletcher16Hasher::End()
	{
		m_sum1 = (m_sum1 & 0xFF) + (m_sum1 >> 8);
		m_sum2 = (m_sum2 & 0xFF) + (m_sum2 >> 8);

		UInt16 fletcher = BigEndianToHost(UInt16((m_sum2 << 8) | m_sum1));
		return ByteArray(reinterpret_cast<UInt8*>(&fletcher), 2);
	}

	std::size_t Fletcher16Hasher::GetDigestLength() const
	{
		return 2;
	}

	const char* Fletcher16Hasher::GetHashName() const
	{
		return "Fletcher16";
	}
}
