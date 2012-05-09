// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA512.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

NzHashSHA512::NzHashSHA512()
{
	m_state = new SHA_CTX;
}

NzHashSHA512::~NzHashSHA512()
{
	delete m_state;
}

void NzHashSHA512::Append(const nzUInt8* data, unsigned int len)
{
	SHA512_Update(m_state, data, len);
}

void NzHashSHA512::Begin()
{
	SHA512_Init(m_state);
}

NzHashDigest NzHashSHA512::End()
{
	nzUInt8 digest[SHA512_DIGEST_LENGTH];

	SHA512_End(m_state, digest);

	return NzHashDigest(GetHashName(), digest, SHA512_DIGEST_LENGTH);
}

unsigned int NzHashSHA512::GetDigestLength()
{
	return SHA512_DIGEST_LENGTH;
}

NzString NzHashSHA512::GetHashName()
{
	static NzString hashName = "SHA512";
	return hashName;
}
