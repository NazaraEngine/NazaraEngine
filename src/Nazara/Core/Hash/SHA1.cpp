// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SHA1Hash::SHA1Hash()
	{
		m_state = new SHA_CTX;
	}

	SHA1Hash::~SHA1Hash()
	{
		delete m_state;
	}

	void SHA1Hash::Append(const UInt8* data, std::size_t len)
	{
		SHA1_Update(m_state, data, len);
	}

	void SHA1Hash::Begin()
	{
		SHA1_Init(m_state);
	}

	ByteArray SHA1Hash::End()
	{
		UInt8 digest[SHA1_DIGEST_LENGTH];

		SHA1_End(m_state, digest);

		return ByteArray(digest, SHA1_DIGEST_LENGTH);
	}

	std::size_t SHA1Hash::GetDigestLength() const
	{
		return SHA1_DIGEST_LENGTH;
	}

	const char* SHA1Hash::GetHashName() const
	{
		return "SHA1";
	}
}
