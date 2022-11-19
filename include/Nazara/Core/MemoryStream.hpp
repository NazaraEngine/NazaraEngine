// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_MEMORYSTREAM_HPP
#define NAZARA_CORE_MEMORYSTREAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Utils/MovablePtr.hpp>

namespace Nz
{
	class ByteArray;

	class NAZARA_CORE_API MemoryStream : public Stream
	{
		public:
			inline MemoryStream();
			inline MemoryStream(ByteArray* byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);
			MemoryStream(const MemoryStream&) = delete;
			MemoryStream(MemoryStream&&) noexcept = default;
			~MemoryStream() = default;

			void Clear();

			inline ByteArray& GetBuffer();
			inline const ByteArray& GetBuffer() const;
			UInt64 GetSize() const override;

			void SetBuffer(ByteArray* byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);

			MemoryStream& operator=(const MemoryStream&) = delete;
			MemoryStream& operator=(MemoryStream&&) noexcept = default;

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			bool SeekStreamCursor(UInt64 offset) override;
			UInt64 TellStreamCursor() const override;
			bool TestStreamEnd() const override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			MovablePtr<ByteArray> m_buffer;
			UInt64 m_pos;
	};
}

#include <Nazara/Core/MemoryStream.inl>

#endif // NAZARA_CORE_MEMORYSTREAM_HPP
