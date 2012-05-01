// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_HASH_WHIRLPOOL_HPP
#define NAZARA_HASH_WHIRLPOOL_HPP

#define NAZARA_HASH_WHIRLPOOL

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>

struct NzHashWhirlpool_state;

class NAZARA_API NzHashWhirlpool : public NzHashImpl
{
	public:
		NzHashWhirlpool();
		virtual ~NzHashWhirlpool();

		void Append(const nzUInt8* data, unsigned int len);
		void Begin();
		NzHashDigest End();

		static unsigned int GetDigestLength();
		static NzString GetHashName();

	private:
		NzHashWhirlpool_state* m_state;
};

#undef NAZARA_HASH_WHIRLPOOL

#endif // NAZARA_HASH_WHIRLPOOL_HPP
