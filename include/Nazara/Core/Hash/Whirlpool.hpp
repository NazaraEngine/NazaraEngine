// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HASH_WHIRLPOOL_HPP
#define NAZARA_CORE_HASH_WHIRLPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	class NAZARA_CORE_API WhirlpoolHasher final : public AbstractHash
	{
		public:
			WhirlpoolHasher() = default;
			~WhirlpoolHasher() = default;

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			void ProcessBuffer();

			std::size_t m_bufferBits; // current number of bits on the buffer */
			std::size_t m_bufferPos; // current (possibly incomplete) byte slot on the buffer */
			UInt8 m_bitLength[32]; // global number of hashed bits (256-bit counter) */
			UInt8 m_buffer[64]; // buffer of data to hash */
			UInt64 m_hash[8]; // the hashing state */
	};
}

#endif // NAZARA_CORE_HASH_WHIRLPOOL_HPP
