// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

NzHashSHA224::NzHashSHA224()
{
	m_state = new SHA_CTX;
}

NzHashSHA224::~NzHashSHA224()
{
	delete m_state;
}

void NzHashSHA224::Append(const nzUInt8* data, unsigned int len)
{
	SHA224_Update(m_state, data, len);
}

void NzHashSHA224::Begin()
{
	SHA224_Init(m_state);
}

NzHashDigest NzHashSHA224::End()
{
	nzUInt8 digest[SHA224_DIGEST_LENGTH];

	SHA224_End(m_state, digest);

	return NzHashDigest(GetHashName(), digest, SHA224_DIGEST_LENGTH);
}

unsigned int NzHashSHA224::GetDigestLength()
{
	return SHA224_DIGEST_LENGTH;
}

NzString NzHashSHA224::GetHashName()
{
	static NzString hashName = "SHA224";
	return hashName;
}
