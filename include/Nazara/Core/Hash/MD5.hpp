// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_HASH_MD5_HPP
#define NAZARA_CORE_HASH_MD5_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	class NAZARA_CORE_API MD5Hasher final : public AbstractHash
	{
		public:
			MD5Hasher() = default;
			~MD5Hasher() = default;

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			void md5_process(const UInt8* data);

			std::size_t m_count[2]; /* message length in bits, lsw first */
			UInt32 m_abcd[4]; /* digest buffer */
			UInt8  m_buf[64]; /* accumulate block */
	};
}

#endif // NAZARA_CORE_HASH_MD5_HPP
