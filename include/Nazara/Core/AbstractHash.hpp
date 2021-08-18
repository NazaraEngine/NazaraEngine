// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTHASH_HPP
#define NAZARA_ABSTRACTHASH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <memory>

namespace Nz
{
	class ByteArray;

	class NAZARA_CORE_API AbstractHash
	{
		public:
			AbstractHash() = default;
			AbstractHash(const AbstractHash&) = delete;
			AbstractHash(AbstractHash&&) noexcept = default;
			virtual ~AbstractHash();

			virtual void Append(const UInt8* data, std::size_t len) = 0;
			virtual void Begin() = 0;
			virtual ByteArray End() = 0;

			virtual std::size_t GetDigestLength() const = 0;
			virtual const char* GetHashName() const = 0;

			AbstractHash& operator=(const AbstractHash&) = delete;
			AbstractHash& operator=(AbstractHash&&) noexcept = default;

			static std::unique_ptr<AbstractHash> Get(HashType hash);
	};
}

#endif // NAZARA_ABSTRACTHASH_HPP
