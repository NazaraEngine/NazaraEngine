// Copyright (C) 2015 Jérôme Leclercq
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

namespace Nz
{
	Hash::Hash(HashType hash)
	{
		switch (hash)
		{
			case HashType_Fletcher16:
				m_impl = new HashFletcher16;
				break;

			case HashType_CRC32:
				m_impl = new HashCRC32;
				break;

			case HashType_MD5:
				m_impl = new HashMD5;
				break;

			case HashType_SHA1:
				m_impl = new HashSHA1;
				break;

			case HashType_SHA224:
				m_impl = new HashSHA224;
				break;

			case HashType_SHA256:
				m_impl = new HashSHA256;
				break;

			case HashType_SHA384:
				m_impl = new HashSHA384;
				break;

			case HashType_SHA512:
				m_impl = new HashSHA512;
				break;

			case HashType_Whirlpool:
				m_impl = new HashWhirlpool;
				break;
		}
	}

	Hash::Hash(AbstractHash* hashImpl) :
	m_impl(hashImpl)
	{
	}

	Hash::~Hash()
	{
		delete m_impl;
	}

	HashDigest Hash::Process(const Hashable& hashable)
	{
		m_impl->Begin();
		if (hashable.FillHash(m_impl))
			return m_impl->End();
		else // Erreur
		{
			m_impl->End();

			return HashDigest();
		}
	}
}
