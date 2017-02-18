// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	HashSHA256::HashSHA256()
	{
		m_state = new SHA_CTX;
	}

	HashSHA256::~HashSHA256()
	{
		delete m_state;
	}

	void HashSHA256::Append(const UInt8* data, std::size_t len)
	{
		SHA256_Update(m_state, data, len);
	}

	void HashSHA256::Begin()
	{
		SHA256_Init(m_state);
	}

	ByteArray HashSHA256::End()
	{
		UInt8 digest[SHA256_DIGEST_LENGTH];

		SHA256_End(m_state, digest);

		return ByteArray(digest, SHA256_DIGEST_LENGTH);
	}

	std::size_t HashSHA256::GetDigestLength() const
	{
		return SHA256_DIGEST_LENGTH;
	}

	const char* HashSHA256::GetHashName() const
	{
		return "SHA256";
	}
}
