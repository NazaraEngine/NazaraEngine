// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_CRC64_HPP
#define NAZARA_HASH_CRC64_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	class NAZARA_CORE_API HashCRC64 : public AbstractHash
	{
		public:
			HashCRC64() = default;
			~HashCRC64() = default;

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			Nz::UInt64 m_crc;
	};
}

#endif // NAZARA_HASH_CRC64_HPP
