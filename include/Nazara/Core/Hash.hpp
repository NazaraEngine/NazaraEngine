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

namespace Nz
{
	class NAZARA_CORE_API Hash
	{
		public:
			Hash(HashType hash);
			Hash(AbstractHash* hashImpl);
			Hash(const Hash&) = delete;
			Hash(Hash&&) = delete; ///TODO
			~Hash();

			HashDigest Process(const Hashable& hashable);

			Hash& operator=(const Hash&) = delete;
			Hash& operator=(Hash&&) = delete; ///TODO

		private:
			AbstractHash* m_impl;
	};
}

#endif // NAZARA_HASH_HPP
