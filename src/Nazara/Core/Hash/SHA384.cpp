// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

NzHashSHA384::NzHashSHA384()
{
	m_state = new SHA_CTX;
}

NzHashSHA384::~NzHashSHA384()
{
	delete m_state;
}

void NzHashSHA384::Append(const nzUInt8* data, unsigned int len)
{
	SHA384_Update(m_state, data, len);
}

void NzHashSHA384::Begin()
{
	SHA384_Init(m_state);
}

NzHashDigest NzHashSHA384::End()
{
	nzUInt8 digest[SHA384_DIGEST_LENGTH];

	SHA384_End(m_state, digest);

	return NzHashDigest(GetHashName(), digest, SHA384_DIGEST_LENGTH);
}

unsigned int NzHashSHA384::GetDigestLength()
{
	return SHA384_DIGEST_LENGTH;
}

NzString NzHashSHA384::GetHashName()
{
	static NzString hashName = "SHA384";
	return hashName;
}
