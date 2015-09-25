// Copyright (C) 2015 Jérôme Leclercq
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

	void HashSHA224::Append(const UInt8* data, unsigned int len)
	{
		SHA224_Update(m_state, data, len);
	}

	void HashSHA224::Begin()
	{
		SHA224_Init(m_state);
	}

	HashDigest HashSHA224::End()
	{
		UInt8 digest[SHA224_DIGEST_LENGTH];

		SHA224_End(m_state, digest);

		return HashDigest(GetHashName(), digest, SHA224_DIGEST_LENGTH);
	}

	unsigned int HashSHA224::GetDigestLength()
	{
		return SHA224_DIGEST_LENGTH;
	}

	String HashSHA224::GetHashName()
	{
		static String hashName = "SHA224";
		return hashName;
	}
}
