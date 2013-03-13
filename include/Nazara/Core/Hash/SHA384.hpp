// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_SHA384_HPP
#define NAZARA_HASH_SHA384_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>

union SHA_CTX;

class NAZARA_API NzHashSHA384 : public NzHashImpl
{
	public:
		NzHashSHA384();
		virtual ~NzHashSHA384();

		void Append(const nzUInt8* data, unsigned int len);
		void Begin();
		NzHashDigest End();

		static unsigned int GetDigestLength();
		static NzString GetHashName();

	private:
		SHA_CTX* m_state;
};

#endif // NAZARA_HASH_SHA384_HPP
