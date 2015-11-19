// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYSTREAM_HPP
#define NAZARA_MEMORYSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/OutputStream.hpp>

namespace Nz
{
	class NAZARA_CORE_API MemoryStream : public InputStream, public OutputStream
	{
		public:
			MemoryStream();
			MemoryStream(const void* ptr, unsigned int size);
			MemoryStream(const MemoryStream&) = default;
			MemoryStream(MemoryStream&&) = default;
			~MemoryStream() = default;

			void Clear();

			bool EndOfStream() const override;

			void Flush() override;

			const ByteArray& GetBuffer() const;
			const UInt8* GetData() const;
			UInt64 GetCursorPos() const override;
			UInt64 GetSize() const override;

			std::size_t Read(void* buffer, std::size_t size) override;

			bool SetCursorPos(UInt64 offset) override;

			std::size_t Write(const void* buffer, std::size_t size) override;

			MemoryStream& operator=(const MemoryStream&) = default;
			MemoryStream& operator=(MemoryStream&&) = default;

		private:
			ByteArray m_buffer;
			UInt64 m_pos;
	};

	class AbstractHash;

	inline bool HashAppend(AbstractHash* hash, const String& string);
	NAZARA_CORE_API bool Serialize(SerializationContext& context, const String& string);
	NAZARA_CORE_API bool Unserialize(UnserializationContext& context, String* string);
}

#include <Nazara/Core/MemoryStream.hpp>

#endif // NAZARA_MEMORYSTREAM_HPP
