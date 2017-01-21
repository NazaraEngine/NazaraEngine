// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_MD5_HPP
#define NAZARA_HASH_MD5_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	struct HashMD5_state;

	class NAZARA_CORE_API HashMD5 : public AbstractHash
	{
		public:
			HashMD5();
			virtual ~HashMD5();

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			HashMD5_state* m_state;
	};
}

#endif // NAZARA_HASH_MD5_HPP
