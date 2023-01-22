// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SHA384Hash::SHA384Hash()
	{
		m_state = new SHA_CTX;
	}

	SHA384Hash::~SHA384Hash()
	{
		delete m_state;
	}

	void SHA384Hash::Append(const UInt8* data, std::size_t len)
	{
		SHA384_Update(m_state, data, len);
	}

	void SHA384Hash::Begin()
	{
		SHA384_Init(m_state);
	}

	ByteArray SHA384Hash::End()
	{
		UInt8 digest[SHA384_DIGEST_LENGTH];

		SHA384_End(m_state, digest);

		return ByteArray(digest, SHA384_DIGEST_LENGTH);
	}

	std::size_t SHA384Hash::GetDigestLength() const
	{
		return SHA384_DIGEST_LENGTH;
	}

	const char* SHA384Hash::GetHashName() const
	{
		return "SHA384";
	}
}
