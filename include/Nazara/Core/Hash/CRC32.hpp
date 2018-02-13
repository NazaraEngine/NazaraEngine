// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_CRC32_HPP
#define NAZARA_HASH_CRC32_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	struct HashCRC32_state;

	class NAZARA_CORE_API HashCRC32 : public AbstractHash
	{
		public:
			HashCRC32(UInt32 polynomial = 0x04c11db7);
			virtual ~HashCRC32();

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			HashCRC32_state* m_state;
	};
}

#endif // NAZARA_HASH_CRC32_HPP
