// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HASHABLE_HPP_INCLUDED
#define HASHABLE_HPP_INCLUDED

#include <Nazara/Prerequesites.hpp>

enum nzHash
{
	nzHash_CRC32,
	nzHash_Fletcher16,
	nzHash_MD5,
	nzHash_SHA1,
	nzHash_SHA224,
	nzHash_SHA256,
	nzHash_SHA384,
	nzHash_SHA512,
	nzHash_Whirlpool
};

class NzAbstractHash;
class NzHashDigest;

class NAZARA_API NzHashable
{
	friend class NzHash;

	public:
		NzHashable() = default;
		virtual ~NzHashable();

		NzHashDigest GetHash(nzHash hash) const;
		NzHashDigest GetHash(NzAbstractHash* impl) const;

	private:
		virtual bool FillHash(NzAbstractHash* impl) const = 0;
};

#endif // HASHABLE_HPP_INCLUDED
