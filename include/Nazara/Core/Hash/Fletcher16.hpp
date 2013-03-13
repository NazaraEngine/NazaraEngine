// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_FLETCHER16_HPP
#define NAZARA_HASH_FLETCHER16_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>
#include <Nazara/Core/String.hpp>

struct NzHashFletcher16_state;

class NAZARA_API NzHashFletcher16 : public NzHashImpl
{
	public:
		NzHashFletcher16();
		virtual ~NzHashFletcher16();

		void Append(const nzUInt8* data, unsigned int len);
		void Begin();
		NzHashDigest End();

		static unsigned int GetDigestLength();
		static NzString GetHashName();

	private:
		NzHashFletcher16_state* m_state;
};

#endif // NAZARA_HASH_FLETCHER16_HPP
