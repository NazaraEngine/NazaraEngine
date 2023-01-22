// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA512.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SHA512Hash::SHA512Hash()
	{
		m_state = new SHA_CTX;
	}

	SHA512Hash::~SHA512Hash()
	{
		delete m_state;
	}

	void SHA512Hash::Append(const UInt8* data, std::size_t len)
	{
		SHA512_Update(m_state, data, len);
	}

	void SHA512Hash::Begin()
	{
		SHA512_Init(m_state);
	}

	ByteArray SHA512Hash::End()
	{
		UInt8 digest[SHA512_DIGEST_LENGTH];

		SHA512_End(m_state, digest);

		return ByteArray(digest, SHA512_DIGEST_LENGTH);
	}

	std::size_t SHA512Hash::GetDigestLength() const
	{
		return SHA512_DIGEST_LENGTH;
	}

	const char* SHA512Hash::GetHashName() const
	{
		return "SHA512";
	}
}
