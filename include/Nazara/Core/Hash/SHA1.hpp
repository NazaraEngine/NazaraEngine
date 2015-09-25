// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_SHA1_HPP
#define NAZARA_HASH_SHA1_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/HashDigest.hpp>

namespace Nz
{
	union SHA_CTX;

	class NAZARA_CORE_API HashSHA1 : public AbstractHash
	{
		public:
			HashSHA1();
			virtual ~HashSHA1();

			void Append(const UInt8* data, unsigned int len);
			void Begin();
			HashDigest End();

			static unsigned int GetDigestLength();
			static String GetHashName();

		private:
			SHA_CTX* m_state;
	};
}

#endif // NAZARA_HASH_SHA1_HPP
