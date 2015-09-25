// Copyright (C) 2015 Jérôme Leclercq
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

	void HashSHA1::Append(const UInt8* data, unsigned int len)
	{
		SHA1_Update(m_state, data, len);
	}

	void HashSHA1::Begin()
	{
		SHA1_Init(m_state);
	}

	HashDigest HashSHA1::End()
	{
		UInt8 digest[SHA1_DIGEST_LENGTH];

		SHA1_End(m_state, digest);

		return HashDigest(GetHashName(), digest, SHA1_DIGEST_LENGTH);
	}

	unsigned int HashSHA1::GetDigestLength()
	{
		return SHA1_DIGEST_LENGTH;
	}

	String HashSHA1::GetHashName()
	{
		static String hashName = "SHA1";
		return hashName;
	}
}
