// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTHASH_HPP
#define NAZARA_ABSTRACTHASH_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class HashDigest;

	class NAZARA_CORE_API AbstractHash
	{
		public:
			AbstractHash() = default;
			AbstractHash(const AbstractHash&) = delete;
			AbstractHash(AbstractHash&&) = default;
			virtual ~AbstractHash();

			virtual void Append(const UInt8* data, unsigned int len) = 0;
			virtual void Begin() = 0;
			virtual HashDigest End() = 0;

			AbstractHash& operator=(const AbstractHash&) = delete;
			AbstractHash& operator=(AbstractHash&&) = default;
	};
}

#endif // NAZARA_ABSTRACTHASH_HPP
