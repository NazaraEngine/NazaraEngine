// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	HashDigest::HashDigest() :
	m_digest(nullptr),
	m_digestLength(0)
	{
	}

	HashDigest::HashDigest(const String& hashName, const UInt8* digest, unsigned int length) :
	m_hashName(hashName),
	m_digestLength(length)
	{
		if (m_digestLength > 0)
		{
			m_digest = new UInt8[length];
			std::memcpy(m_digest, digest, length);
		}
		else
			m_digest = nullptr;
	}

	HashDigest::HashDigest(const HashDigest& rhs) :
	m_hashName(rhs.m_hashName),
	m_digestLength(rhs.m_digestLength)
	{
		if (m_digestLength > 0)
		{
			m_digest = new UInt8[m_digestLength];
			std::memcpy(m_digest, rhs.m_digest, m_digestLength);
		}
		else
			m_digest = nullptr;
	}

	HashDigest::HashDigest(HashDigest&& rhs) noexcept :
	m_hashName(std::move(rhs.m_hashName)),
	m_digest(rhs.m_digest),
	m_digestLength(rhs.m_digestLength)
	{
		rhs.m_digest = nullptr;
		rhs.m_digestLength = 0;
	}

	HashDigest::~HashDigest()
	{
		delete[] m_digest;
	}

	bool HashDigest::IsValid() const
	{
		return m_digestLength > 0;
	}

	const UInt8* HashDigest::GetDigest() const
	{
		return m_digest;
	}

	unsigned int HashDigest::GetDigestLength() const
	{
		return m_digestLength;
	}

	String HashDigest::GetHashName() const
	{
		return m_hashName;
	}

	String HashDigest::ToHex() const
	{
		if (m_digestLength == 0)
			return String();

		unsigned int length = m_digestLength*2;

		String hexOutput(length);
		for (unsigned int i = 0; i < m_digestLength; ++i)
			std::sprintf(&hexOutput[i*2], "%02x", m_digest[i]);

		return hexOutput;
	}

	UInt8 HashDigest::operator[](unsigned int pos) const
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

	HashDigest& HashDigest::operator=(const HashDigest& rhs)
	{
		if (this == &rhs)
			return *this;

		m_hashName = rhs.m_hashName;

		m_digestLength = rhs.m_digestLength;
		if (m_digestLength > 0)
		{
			m_digest = new UInt8[m_digestLength];
			std::memcpy(m_digest, rhs.m_digest, m_digestLength);
		}
		else
			m_digest = nullptr;

		return *this;
	}

	HashDigest& HashDigest::operator=(HashDigest&& rhs) noexcept
	{
		std::swap(m_hashName, rhs.m_hashName);
		std::swap(m_digest, rhs.m_digest);
		std::swap(m_digestLength, rhs.m_digestLength);

		return *this;
	}

	bool HashDigest::operator==(const HashDigest& rhs) const
	{
		if (m_digest == nullptr || rhs.m_digest == nullptr)
			return m_digest == rhs.m_digest;

		if (m_digestLength != rhs.m_digestLength)
			return false;

		return m_hashName == rhs.m_hashName && std::memcmp(m_digest, rhs.m_digest, m_digestLength) == 0;
	}

	bool HashDigest::operator!=(const HashDigest& rhs) const
	{
		return !operator==(rhs);
	}

	bool HashDigest::operator<(const HashDigest& rhs) const
	{
		if (rhs.m_digest == nullptr)
			return false;

		if (m_digest == nullptr)
			return true;

		int cmp = String::Compare(m_hashName, rhs.m_hashName);
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

	bool HashDigest::operator<=(const HashDigest& rhs) const
	{
		return !rhs.operator<(*this);
	}

	bool HashDigest::operator>(const HashDigest& rhs) const
	{
		return rhs.operator<(*this);
	}

	bool HashDigest::operator>=(const HashDigest& rhs) const
	{
		return !operator<(rhs);
	}
}

std::ostream& operator<<(std::ostream& out, const Nz::HashDigest& hashstring)
{
	out << hashstring.ToHex();
	return out;
}
