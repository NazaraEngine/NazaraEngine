// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYSTREAM_HPP
#define NAZARA_MEMORYSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class NAZARA_CORE_API MemoryStream : public Stream
	{
		public:
			MemoryStream();
			MemoryStream(const void* ptr, unsigned int size);
			MemoryStream(const MemoryStream&) = default;
			MemoryStream(MemoryStream&&) = default;
			~MemoryStream() = default;

			void Clear();

			bool EndOfStream() const override;


			const ByteArray& GetBuffer() const;
			const UInt8* GetData() const;
			UInt64 GetCursorPos() const override;
			UInt64 GetSize() const override;

			bool SetCursorPos(UInt64 offset) override;

			MemoryStream& operator=(const MemoryStream&) = default;
			MemoryStream& operator=(MemoryStream&&) = default;

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			ByteArray m_buffer;
			UInt64 m_pos;
	};

	class AbstractHash;

	inline bool HashAppend(AbstractHash* hash, const String& string);
	NAZARA_CORE_API bool Serialize(SerializationContext& context, const String& string);
	NAZARA_CORE_API bool Unserialize(UnserializationContext& context, String* string);
}

#include <Nazara/Core/MemoryStream.inl>

#endif // NAZARA_MEMORYSTREAM_HPP
