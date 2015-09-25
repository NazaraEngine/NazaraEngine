// Copyright (C) 2015 Jérôme Leclercq
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

	void HashSHA512::Append(const UInt8* data, unsigned int len)
	{
		SHA512_Update(m_state, data, len);
	}

	void HashSHA512::Begin()
	{
		SHA512_Init(m_state);
	}

	HashDigest HashSHA512::End()
	{
		UInt8 digest[SHA512_DIGEST_LENGTH];

		SHA512_End(m_state, digest);

		return HashDigest(GetHashName(), digest, SHA512_DIGEST_LENGTH);
	}

	unsigned int HashSHA512::GetDigestLength()
	{
		return SHA512_DIGEST_LENGTH;
	}

	String HashSHA512::GetHashName()
	{
		static String hashName = "SHA512";
		return hashName;
	}
}
