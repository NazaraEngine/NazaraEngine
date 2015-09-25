// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_HASH_WHIRLPOOL_HPP
#define NAZARA_HASH_WHIRLPOOL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/HashDigest.hpp>

namespace Nz
{
	struct HashWhirlpool_state;

	class NAZARA_CORE_API HashWhirlpool : public AbstractHash
	{
		public:
			HashWhirlpool();
			virtual ~HashWhirlpool();

			void Append(const UInt8* data, unsigned int len);
			void Begin();
			HashDigest End();

			static unsigned int GetDigestLength();
			static String GetHashName();

		private:
			HashWhirlpool_state* m_state;
	};
}

#endif // NAZARA_HASH_WHIRLPOOL_HPP
