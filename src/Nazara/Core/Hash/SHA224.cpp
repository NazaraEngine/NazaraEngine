// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	HashSHA224::HashSHA224()
	{
		m_state = new SHA_CTX;
	}

	HashSHA224::~HashSHA224()
	{
		delete m_state;
	}

	void HashSHA224::Append(const UInt8* data, std::size_t len)
	{
		SHA224_Update(m_state, data, len);
	}

	void HashSHA224::Begin()
	{
		SHA224_Init(m_state);
	}

	ByteArray HashSHA224::End()
	{
		UInt8 digest[SHA224_DIGEST_LENGTH];

		SHA224_End(m_state, digest);

		return ByteArray(digest, SHA224_DIGEST_LENGTH);
	}

	std::size_t HashSHA224::GetDigestLength() const
	{
		return SHA224_DIGEST_LENGTH;
	}

	const char* HashSHA224::GetHashName() const
	{
		return "SHA224";
	}
}
