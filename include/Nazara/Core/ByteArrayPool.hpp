// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BYTEARRAYPOOL_HPP
#define NAZARA_BYTEARRAYPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <vector>

namespace Nz
{
	class AbstractHash;

	class NAZARA_CORE_API ByteArrayPool
	{
		public:
			ByteArrayPool() = default;
			ByteArrayPool(const ByteArrayPool&) = delete;
			ByteArrayPool(ByteArrayPool&&) = default;
			~ByteArrayPool() = default;

			inline void Clear();

			inline ByteArray GetByteArray(std::size_t capacity = 0);

			inline void ReturnByteArray(ByteArray byteArray);

			ByteArrayPool& operator=(const ByteArrayPool&) = delete;
			ByteArrayPool& operator=(ByteArrayPool&&) = default;

		private:
			std::vector<ByteArray> m_byteArrays;
	};
}

#include <Nazara/Core/ByteArrayPool.inl>

#endif // NAZARA_BYTEARRAYPOOL_HPP
