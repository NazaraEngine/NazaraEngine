// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BYTESTREAM_HPP
#define NAZARA_BYTESTREAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/SerializationContext.hpp>
#include <memory>

namespace Nz
{
	class ByteArray;
	class Stream;

	class NAZARA_CORE_API ByteStream
	{
		public:
			inline ByteStream(Stream* stream = nullptr);
			ByteStream(ByteArray* byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);
			ByteStream(void* ptr, Nz::UInt64 size);
			ByteStream(const void* ptr, Nz::UInt64 size);
			ByteStream(const ByteStream&) = delete;
			ByteStream(ByteStream&& stream) noexcept = default;
			virtual ~ByteStream();

			inline void ClearStream();

			inline Endianness GetDataEndianness() const;
			inline Nz::UInt64 GetSize() const;
			inline Stream* GetStream() const;

			inline bool FlushBits();

			inline std::size_t Read(void* ptr, std::size_t size);

			inline void SetDataEndianness(Endianness endiannes);
			inline void SetStream(Stream* stream);
			void SetStream(ByteArray* byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);
			void SetStream(void* ptr, Nz::UInt64 size);
			void SetStream(const void* ptr, Nz::UInt64 size);

			inline std::size_t Write(const void* data, std::size_t size);

			template<typename T>
			ByteStream& operator>>(T& value);

			template<typename T>
			ByteStream& operator<<(const T& value);

			ByteStream& operator=(const ByteStream&) = delete;
			ByteStream& operator=(ByteStream&&) noexcept = default;

		private:
			virtual void OnEmptyStream();

			std::unique_ptr<Stream> m_ownedStream;
			SerializationContext m_context;
	};
}

#include <Nazara/Core/ByteStream.inl>

#endif // NAZARA_BYTESTREAM_HPP
