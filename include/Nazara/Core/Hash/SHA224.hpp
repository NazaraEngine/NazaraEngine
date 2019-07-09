// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_SHA224_HPP
#define NAZARA_HASH_SHA224_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	union SHA_CTX;

	class NAZARA_CORE_API HashSHA224 : public AbstractHash
	{
		public:
			HashSHA224();
			virtual ~HashSHA224();

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			SHA_CTX* m_state;
	};
}

#endif // NAZARA_HASH_SHA224_HPP
