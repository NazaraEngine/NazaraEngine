// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void Fletcher16Hash::Append(const UInt8* data, std::size_t len)
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

	void Fletcher16Hash::Begin()
	{
		m_sum1 = 0xFF;
		m_sum2 = 0xFF;
	}

	ByteArray Fletcher16Hash::End()
	{
		m_sum1 = (m_sum1 & 0xFF) + (m_sum1 >> 8);
		m_sum2 = (m_sum2 & 0xFF) + (m_sum2 >> 8);

		UInt16 fletcher = (m_sum2 << 8) | m_sum1;

		#ifdef NAZARA_LITTLE_ENDIAN
		fletcher = SwapBytes(fletcher);
		#endif

		return ByteArray(reinterpret_cast<UInt8*>(&fletcher), 2);
	}

	std::size_t Fletcher16Hash::GetDigestLength() const
	{
		return 2;
	}

	const char* Fletcher16Hash::GetHashName() const
	{
		return "Fletcher16";
	}
}
