// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYSTREAM_HPP
#define NAZARA_MEMORYSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class ByteArray;

	class NAZARA_CORE_API MemoryStream : public Stream
	{
		public:
			inline MemoryStream();
			inline MemoryStream(ByteArray* byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);
			MemoryStream(const MemoryStream&) = default;
			MemoryStream(MemoryStream&&) = default;
			~MemoryStream() = default;

			void Clear();

			bool EndOfStream() const override;

			inline ByteArray& GetBuffer();
			inline const ByteArray& GetBuffer() const;
			UInt64 GetCursorPos() const override;
			UInt64 GetSize() const override;

			void SetBuffer(ByteArray* byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);
			bool SetCursorPos(UInt64 offset) override;

			MemoryStream& operator=(const MemoryStream&) = default;
			MemoryStream& operator=(MemoryStream&&) = default;

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			ByteArray* m_buffer;
			UInt64 m_pos;
	};
}

#include <Nazara/Core/MemoryStream.inl>

#endif // NAZARA_MEMORYSTREAM_HPP
