// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>

namespace Nz
{
	SHA256Hasher::SHA256Hasher()
	{
		m_state = new SHA_CTX;
	}

	SHA256Hasher::~SHA256Hasher()
	{
		delete m_state;
	}

	void SHA256Hasher::Append(const UInt8* data, std::size_t len)
	{
		SHA256_Update(m_state, data, len);
	}

	void SHA256Hasher::Begin()
	{
		SHA256_Init(m_state);
	}

	ByteArray SHA256Hasher::End()
	{
		UInt8 digest[SHA256_DIGEST_LENGTH];

		SHA256_End(m_state, digest);

		return ByteArray(digest, SHA256_DIGEST_LENGTH);
	}

	std::size_t SHA256Hasher::GetDigestLength() const
	{
		return SHA256_DIGEST_LENGTH;
	}

	const char* SHA256Hasher::GetHashName() const
	{
		return "SHA256";
	}
}
