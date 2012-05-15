// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstdio>
#include <cstring>
#include <utility>
#include <Nazara/Core/Debug.hpp>

NzHashDigest::NzHashDigest() :
m_digest(nullptr),
m_digestLength(0)
{
}

NzHashDigest::NzHashDigest(const NzString& hashName, const nzUInt8* digest, unsigned int length) :
m_hashName(hashName),
m_digestLength(length)
{
	if (m_digestLength > 0)
	{
		m_digest = new nzUInt8[length];
		std::memcpy(m_digest, digest, length);
	}
	else
		m_digest = nullptr;
}

NzHashDigest::NzHashDigest(const NzHashDigest& rhs) :
m_hashName(rhs.m_hashName),
m_digestLength(rhs.m_digestLength)
{
	if (m_digestLength > 0)
	{
		m_digest = new nzUInt8[m_digestLength];
		std::memcpy(m_digest, rhs.m_digest, m_digestLength);
	}
	else
		m_digest = nullptr;
}

NzHashDigest::NzHashDigest(NzHashDigest&& rhs) :
m_hashName(std::move(rhs.m_hashName)),
m_digest(rhs.m_digest),
m_digestLength(rhs.m_digestLength)
{
	rhs.m_digest = nullptr;
	rhs.m_digestLength = 0;
}

NzHashDigest::~NzHashDigest()
{
	delete[] m_digest;
}

bool NzHashDigest::IsValid() const
{
	return m_digestLength > 0;
}

const nzUInt8* NzHashDigest::GetDigest() const
{
	return m_digest;
}

unsigned int NzHashDigest::GetDigestLength() const
{
	return m_digestLength;
}

NzString NzHashDigest::GetHashName() const
{
	return m_hashName;
}

NzString NzHashDigest::ToHex() const
{
	if (m_digestLength == 0)
		return NzString();

	unsigned int length = m_digestLength*2;
	char* hex_output = new char[length+1];
	for (unsigned int i = 0; i < m_digestLength; ++i)
		std::sprintf(hex_output + i*2, "%02x", m_digest[i]);

	return NzString(new NzString::SharedString(length, length, 1, hex_output));
}

nzUInt8 NzHashDigest::operator[](unsigned short pos) const
{
	#if NAZARA_CORE_SAFE
	if (pos >= m_digestLength)
	{
		NazaraError("Position out of range");
		return 0;
	}
	#endif

	return m_digest[pos];
}

NzHashDigest& NzHashDigest::operator=(const NzHashDigest& rhs)
{
	m_hashName = rhs.m_hashName;

	m_digestLength = rhs.m_digestLength;
	if (m_digestLength > 0)
	{
		m_digest = new nzUInt8[m_digestLength];
		std::memcpy(m_digest, rhs.m_digest, m_digestLength);
	}
	else
		m_digest = nullptr;

	return *this;
}

NzHashDigest& NzHashDigest::operator=(NzHashDigest&& rhs)
{
	std::swap(m_hashName, rhs.m_hashName);
	std::swap(m_digest, rhs.m_digest);
	std::swap(m_digestLength, rhs.m_digestLength);

	return *this;
}

bool NzHashDigest::operator==(const NzHashDigest& rhs) const
{
	if (m_digest == nullptr || rhs.m_digest == nullptr)
		return m_digest == rhs.m_digest;

	if (m_digestLength != rhs.m_digestLength)
		return false;

	return m_hashName == rhs.m_hashName && std::memcmp(m_digest, rhs.m_digest, m_digestLength) == 0;
}

bool NzHashDigest::operator!=(const NzHashDigest& rhs) const
{
	return !operator==(rhs);
}

bool NzHashDigest::operator<(const NzHashDigest& rhs) const
{
	if (rhs.m_digest == nullptr)
		return false;

	if (m_digest == nullptr)
		return true;

	int cmp = NzString::Compare(m_hashName, rhs.m_hashName);
	if (cmp == 0)
	{
		cmp = std::memcmp(m_digest, rhs.m_digest, std::min(m_digestLength, rhs.m_digestLength));

		if (cmp == 0)
			return m_digestLength < rhs.m_digestLength;
		else
			return cmp < 0;
	}
	else
		return cmp < 0;
}

bool NzHashDigest::operator<=(const NzHashDigest& rhs) const
{
	return !rhs.operator<(*this);
}

bool NzHashDigest::operator>(const NzHashDigest& rhs) const
{
	return rhs.operator<(*this);
}

bool NzHashDigest::operator>=(const NzHashDigest& rhs) const
{
	return !operator<(rhs);
}

std::ostream& operator<<(std::ostream& out, const NzHashDigest& hashstring)
{
	out << hashstring.ToHex();
	return out;
}
