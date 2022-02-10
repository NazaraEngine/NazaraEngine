// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HASH_CRC64_HPP
#define NAZARA_CORE_HASH_CRC64_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	class NAZARA_CORE_API CRC64Hash final : public AbstractHash
	{
		public:
			CRC64Hash() = default;
			~CRC64Hash() = default;

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			Nz::UInt64 m_crc;
	};
}

#endif // NAZARA_CORE_HASH_CRC64_HPP
