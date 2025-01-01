// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>

namespace Nz
{
	SHA224Hasher::SHA224Hasher()
	{
		m_state = new SHA_CTX;
	}

	SHA224Hasher::~SHA224Hasher()
	{
		delete m_state;
	}

	void SHA224Hasher::Append(const UInt8* data, std::size_t len)
	{
		SHA224_Update(m_state, data, len);
	}

	void SHA224Hasher::Begin()
	{
		SHA224_Init(m_state);
	}

	ByteArray SHA224Hasher::End()
	{
		UInt8 digest[SHA224_DIGEST_LENGTH];

		SHA224_End(m_state, digest);

		return ByteArray(digest, SHA224_DIGEST_LENGTH);
	}

	std::size_t SHA224Hasher::GetDigestLength() const
	{
		return SHA224_DIGEST_LENGTH;
	}

	const char* SHA224Hasher::GetHashName() const
	{
		return "SHA224";
	}
}
