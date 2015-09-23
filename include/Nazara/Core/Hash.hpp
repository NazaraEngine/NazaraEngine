// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_HPP
#define NAZARA_HASH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/HashDigest.hpp>

class NAZARA_CORE_API NzHash
{
	public:
		NzHash(nzHash hash);
		NzHash(NzAbstractHash* hashImpl);
		NzHash(const NzHash&) = delete;
		NzHash(NzHash&&) = delete; ///TODO
		~NzHash();

		NzHashDigest Hash(const NzHashable& hashable);

		NzHash& operator=(const NzHash&) = delete;
		NzHash& operator=(NzHash&&) = delete; ///TODO

	private:
		NzAbstractHash* m_impl;
};

#endif // NAZARA_HASH_HPP
