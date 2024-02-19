// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>

namespace Nz
{
	SHA1Hasher::SHA1Hasher()
	{
		m_state = new SHA_CTX;
	}

	SHA1Hasher::~SHA1Hasher()
	{
		delete m_state;
	}

	void SHA1Hasher::Append(const UInt8* data, std::size_t len)
	{
		SHA1_Update(m_state, data, len);
	}

	void SHA1Hasher::Begin()
	{
		SHA1_Init(m_state);
	}

	ByteArray SHA1Hasher::End()
	{
		UInt8 digest[SHA1_DIGEST_LENGTH];

		SHA1_End(m_state, digest);

		return ByteArray(digest, SHA1_DIGEST_LENGTH);
	}

	std::size_t SHA1Hasher::GetDigestLength() const
	{
		return SHA1_DIGEST_LENGTH;
	}

	const char* SHA1Hasher::GetHashName() const
	{
		return "SHA1";
	}
}
