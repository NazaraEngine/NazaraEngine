// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_HPP
#define NAZARA_HASH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NAZARA_API NzHash : NzNonCopyable
{
	public:
		NzHash(nzHash hash);
		NzHash(NzAbstractHash* hashImpl);
		~NzHash();

		NzHashDigest Hash(const NzHashable& hashable);

	private:
		NzAbstractHash* m_impl;
};

#endif // NAZARA_HASH_HPP
