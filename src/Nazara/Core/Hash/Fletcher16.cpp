// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Debug.hpp>

struct NzHashFletcher16_state
{
	nzUInt16 sum1;
	nzUInt16 sum2;
};

NzHashFletcher16::NzHashFletcher16()
{
	m_state = new NzHashFletcher16_state;
}

NzHashFletcher16::~NzHashFletcher16()
{
	delete m_state;
}

void NzHashFletcher16::Append(const nzUInt8* data, unsigned int len)
{
	while (len)
	{
		unsigned int tlen = std::min(len, 21U);
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

void NzHashFletcher16::Begin()
{
	m_state->sum1 = 0xff;
	m_state->sum2 = 0xff;
}

NzHashDigest NzHashFletcher16::End()
{
	m_state->sum1 = (m_state->sum1 & 0xff) + (m_state->sum1 >> 8);
	m_state->sum2 = (m_state->sum2 & 0xff) + (m_state->sum2 >> 8);

	nzUInt32 fletcher = (m_state->sum2 << 8) | m_state->sum1;

	#ifdef NAZARA_BIG_ENDIAN
	NzByteSwap(&fletcher, sizeof(nzUInt32));
	#endif

	return NzHashDigest(GetHashName(), reinterpret_cast<nzUInt8*>(&fletcher), 2);
}

unsigned int NzHashFletcher16::GetDigestLength()
{
	return 2;
}

NzString NzHashFletcher16::GetHashName()
{
	static NzString hashName = "Fletcher16";
	return hashName;
}
