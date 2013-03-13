// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_CRC32_HPP
#define NAZARA_HASH_CRC32_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>

struct NzHashCRC32_state;

class NAZARA_API NzHashCRC32 : public NzHashImpl
{
	public:
		NzHashCRC32(nzUInt32 polynomial = 0x04c11db7);
		virtual ~NzHashCRC32();

		void Append(const nzUInt8* data, unsigned int len);
		void Begin();
		NzHashDigest End();

		static unsigned int GetDigestLength();
		static NzString GetHashName();

	private:
		NzHashCRC32_state* m_state;
};

#endif // NAZARA_HASH_CRC32_HPP
