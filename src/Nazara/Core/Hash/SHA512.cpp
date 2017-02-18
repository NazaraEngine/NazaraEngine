// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA512.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	HashSHA512::HashSHA512()
	{
		m_state = new SHA_CTX;
	}

	HashSHA512::~HashSHA512()
	{
		delete m_state;
	}

	void HashSHA512::Append(const UInt8* data, std::size_t len)
	{
		SHA512_Update(m_state, data, len);
	}

	void HashSHA512::Begin()
	{
		SHA512_Init(m_state);
	}

	ByteArray HashSHA512::End()
	{
		UInt8 digest[SHA512_DIGEST_LENGTH];

		SHA512_End(m_state, digest);

		return ByteArray(digest, SHA512_DIGEST_LENGTH);
	}

	std::size_t HashSHA512::GetDigestLength() const
	{
		return SHA512_DIGEST_LENGTH;
	}

	const char* HashSHA512::GetHashName() const
	{
		return "SHA512";
	}
}
