// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

NzHashSHA1::NzHashSHA1()
{
	m_state = new SHA_CTX;
}

NzHashSHA1::~NzHashSHA1()
{
	delete m_state;
}

void NzHashSHA1::Append(const nzUInt8* data, unsigned int len)
{
	SHA1_Update(m_state, data, len);
}

void NzHashSHA1::Begin()
{
	SHA1_Init(m_state);
}

NzHashDigest NzHashSHA1::End()
{
	nzUInt8 digest[SHA1_DIGEST_LENGTH];

	SHA1_End(m_state, digest);

	return NzHashDigest(GetHashName(), digest, SHA1_DIGEST_LENGTH);
}

unsigned int NzHashSHA1::GetDigestLength()
{
	return SHA1_DIGEST_LENGTH;
}

NzString NzHashSHA1::GetHashName()
{
	static NzString hashName = "SHA1";
	return hashName;
}
