// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	HashSHA384::HashSHA384()
	{
		m_state = new SHA_CTX;
	}

	HashSHA384::~HashSHA384()
	{
		delete m_state;
	}

	void HashSHA384::Append(const UInt8* data, std::size_t len)
	{
		SHA384_Update(m_state, data, len);
	}

	void HashSHA384::Begin()
	{
		SHA384_Init(m_state);
	}

	ByteArray HashSHA384::End()
	{
		UInt8 digest[SHA384_DIGEST_LENGTH];

		SHA384_End(m_state, digest);

		return ByteArray(digest, SHA384_DIGEST_LENGTH);
	}

	std::size_t HashSHA384::GetDigestLength() const
	{
		return SHA384_DIGEST_LENGTH;
	}

	const char* HashSHA384::GetHashName() const
	{
		return "SHA384";
	}
}
