// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	HashSHA1::HashSHA1()
	{
		m_state = new SHA_CTX;
	}

	HashSHA1::~HashSHA1()
	{
		delete m_state;
	}

	void HashSHA1::Append(const UInt8* data, std::size_t len)
	{
		SHA1_Update(m_state, data, len);
	}

	void HashSHA1::Begin()
	{
		SHA1_Init(m_state);
	}

	ByteArray HashSHA1::End()
	{
		UInt8 digest[SHA1_DIGEST_LENGTH];

		SHA1_End(m_state, digest);

		return ByteArray(digest, SHA1_DIGEST_LENGTH);
	}

	std::size_t HashSHA1::GetDigestLength() const
	{
		return SHA1_DIGEST_LENGTH;
	}

	const char* HashSHA1::GetHashName() const
	{
		return "SHA1";
	}
}
