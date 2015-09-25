// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HASHABLE_HPP_INCLUDED
#define HASHABLE_HPP_INCLUDED

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>

namespace Nz
{
	class AbstractHash;
	class HashDigest;

	class NAZARA_CORE_API Hashable
	{
		friend class Hash;

		public:
			Hashable() = default;
			Hashable(const Hashable&) = default;
			Hashable(Hashable&&) = default;
			virtual ~Hashable();

			HashDigest GetHash(HashType hash) const;
			HashDigest GetHash(AbstractHash* impl) const;

			Hashable& operator=(const Hashable&) = default;
			Hashable& operator=(Hashable&&) = default;

		private:
			virtual bool FillHash(AbstractHash* impl) const = 0;
	};
}

#endif // HASHABLE_HPP_INCLUDED
