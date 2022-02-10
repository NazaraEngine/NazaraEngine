// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SHA256Hash::SHA256Hash()
	{
		m_state = new SHA_CTX;
	}

	SHA256Hash::~SHA256Hash()
	{
		delete m_state;
	}

	void SHA256Hash::Append(const UInt8* data, std::size_t len)
	{
		SHA256_Update(m_state, data, len);
	}

	void SHA256Hash::Begin()
	{
		SHA256_Init(m_state);
	}

	ByteArray SHA256Hash::End()
	{
		UInt8 digest[SHA256_DIGEST_LENGTH];

		SHA256_End(m_state, digest);

		return ByteArray(digest, SHA256_DIGEST_LENGTH);
	}

	std::size_t SHA256Hash::GetDigestLength() const
	{
		return SHA256_DIGEST_LENGTH;
	}

	const char* SHA256Hash::GetHashName() const
	{
		return "SHA256";
	}
}
