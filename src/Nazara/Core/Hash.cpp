// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash.hpp>
#include <Nazara/Core/Hash/CRC32.hpp>
#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <Nazara/Core/Hash/MD5.hpp>
#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA512.hpp>
#include <Nazara/Core/Hash/Whirlpool.hpp>
#include <Nazara/Core/Debug.hpp>

NzHash::NzHash(nzHash hash)
{
	switch (hash)
	{
		case nzHash_Fletcher16:
			m_impl = new NzHashFletcher16;
			break;

		case nzHash_CRC32:
			m_impl = new NzHashCRC32;
			break;

		case nzHash_MD5:
			m_impl = new NzHashMD5;
			break;

		case nzHash_SHA1:
			m_impl = new NzHashSHA1;
			break;

		case nzHash_SHA224:
			m_impl = new NzHashSHA224;
			break;

		case nzHash_SHA256:
			m_impl = new NzHashSHA256;
			break;

		case nzHash_SHA384:
			m_impl = new NzHashSHA384;
			break;

		case nzHash_SHA512:
			m_impl = new NzHashSHA512;
			break;

		case nzHash_Whirlpool:
			m_impl = new NzHashWhirlpool;
			break;
	}
}

NzHash::NzHash(NzAbstractHash* hashImpl) :
m_impl(hashImpl)
{
}

NzHash::~NzHash()
{
	delete m_impl;
}

NzHashDigest NzHash::Hash(const NzHashable& hashable)
{
	m_impl->Begin();
	if (hashable.FillHash(m_impl))
		return m_impl->End();
	else // Erreur
	{
		m_impl->End();

		return NzHashDigest();
	}
}
