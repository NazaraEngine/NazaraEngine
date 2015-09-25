// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_FLETCHER16_HPP
#define NAZARA_HASH_FLETCHER16_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/String.hpp>

namespace Nz
{
	struct HashFletcher16_state;

	class NAZARA_CORE_API HashFletcher16 : public AbstractHash
	{
		public:
			HashFletcher16();
			virtual ~HashFletcher16();

			void Append(const UInt8* data, unsigned int len);
			void Begin();
			HashDigest End();

			static unsigned int GetDigestLength();
			static String GetHashName();

		private:
			HashFletcher16_state* m_state;
	};
}

#endif // NAZARA_HASH_FLETCHER16_HPP
