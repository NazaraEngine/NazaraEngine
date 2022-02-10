// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SHA224Hash::SHA224Hash()
	{
		m_state = new SHA_CTX;
	}

	SHA224Hash::~SHA224Hash()
	{
		delete m_state;
	}

	void SHA224Hash::Append(const UInt8* data, std::size_t len)
	{
		SHA224_Update(m_state, data, len);
	}

	void SHA224Hash::Begin()
	{
		SHA224_Init(m_state);
	}

	ByteArray SHA224Hash::End()
	{
		UInt8 digest[SHA224_DIGEST_LENGTH];

		SHA224_End(m_state, digest);

		return ByteArray(digest, SHA224_DIGEST_LENGTH);
	}

	std::size_t SHA224Hash::GetDigestLength() const
	{
		return SHA224_DIGEST_LENGTH;
	}

	const char* SHA224Hash::GetHashName() const
	{
		return "SHA224";
	}
}
