// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>

namespace Nz
{
	SHA384Hasher::SHA384Hasher()
	{
		m_state = new SHA_CTX;
	}

	SHA384Hasher::~SHA384Hasher()
	{
		delete m_state;
	}

	void SHA384Hasher::Append(const UInt8* data, std::size_t len)
	{
		SHA384_Update(m_state, data, len);
	}

	void SHA384Hasher::Begin()
	{
		SHA384_Init(m_state);
	}

	ByteArray SHA384Hasher::End()
	{
		UInt8 digest[SHA384_DIGEST_LENGTH];

		SHA384_End(m_state, digest);

		return ByteArray(digest, SHA384_DIGEST_LENGTH);
	}

	std::size_t SHA384Hasher::GetDigestLength() const
	{
		return SHA384_DIGEST_LENGTH;
	}

	const char* SHA384Hasher::GetHashName() const
	{
		return "SHA384";
	}
}
