// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_POOLBYTESTREAM_HPP
#define NAZARA_CORE_POOLBYTESTREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/Config.hpp>

namespace Nz
{
	class ByteArrayPool;

	class NAZARA_CORE_API PoolByteStream : public ByteStream
	{
		public:
			inline PoolByteStream(ByteArrayPool& pool);
			inline PoolByteStream(ByteArrayPool& pool, std::size_t capacity);
			PoolByteStream(const PoolByteStream&) = delete;
			PoolByteStream(PoolByteStream&& packet) = default;
			inline ~PoolByteStream();

			void Reset();
			void Reset(std::size_t capacity);

			PoolByteStream& operator=(const PoolByteStream&) = delete;
			PoolByteStream& operator=(PoolByteStream&&) = delete;

		private:
			void OnEmptyStream() override;

			ByteArrayPool& m_pool;
			ByteArray m_buffer;
	};
}

#include <Nazara/Core/PoolByteStream.inl>

#endif // NAZARA_CORE_POOLBYTESTREAM_HPP
