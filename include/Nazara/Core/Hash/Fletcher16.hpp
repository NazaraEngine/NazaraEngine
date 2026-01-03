// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_HASH_FLETCHER16_HPP
#define NAZARA_CORE_HASH_FLETCHER16_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	class NAZARA_CORE_API Fletcher16Hasher final : public AbstractHash
	{
		public:
			Fletcher16Hasher() = default;
			~Fletcher16Hasher() = default;

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			UInt16 m_sum1;
			UInt16 m_sum2;
	};
}

#endif // NAZARA_CORE_HASH_FLETCHER16_HPP
