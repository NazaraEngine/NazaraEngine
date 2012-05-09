// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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

class NzHash;
class NzHashDigest;
class NzHashImpl;

class NAZARA_API NzHashable
{
	friend class NzHash;

	public:
		NzHashable() = default;
		virtual ~NzHashable();

		NzHashDigest GetHash(nzHash hash) const;
		NzHashDigest GetHash(NzHashImpl* impl) const;

	private:
		virtual bool FillHash(NzHashImpl* impl) const = 0;
};

#endif // HASHABLE_HPP_INCLUDED
