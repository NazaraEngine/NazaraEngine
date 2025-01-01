// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_HASH_CRC32_HPP
#define NAZARA_CORE_HASH_CRC32_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	class NAZARA_CORE_API CRC32Hasher final : public AbstractHash
	{
		public:
			CRC32Hasher(UInt32 polynomial = DefaultPolynomial);
			~CRC32Hasher();

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

			static constexpr UInt32 DefaultPolynomial = 0x04C11DB7;

		private:
			UInt32 m_crc;
			const UInt32* m_table;
	};
}

#endif // NAZARA_CORE_HASH_CRC32_HPP
