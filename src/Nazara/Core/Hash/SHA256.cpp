// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

NzHashSHA256::NzHashSHA256()
{
	m_state = new SHA_CTX;
}

NzHashSHA256::~NzHashSHA256()
{
	delete m_state;
}

void NzHashSHA256::Append(const nzUInt8* data, unsigned int len)
{
	SHA256_Update(m_state, data, len);
}

void NzHashSHA256::Begin()
{
	SHA256_Init(m_state);
}

NzHashDigest NzHashSHA256::End()
{
	nzUInt8 digest[SHA256_DIGEST_LENGTH];

	SHA256_End(m_state, digest);

	return NzHashDigest(GetHashName(), digest, SHA256_DIGEST_LENGTH);
}

unsigned int NzHashSHA256::GetDigestLength()
{
	return SHA256_DIGEST_LENGTH;
}

NzString NzHashSHA256::GetHashName()
{
	static NzString hashName = "SHA256";
	return hashName;
}
