// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_STREAM_HPP
#define NAZARA_CORE_STREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <filesystem>
#include <memory>
#include <string>

namespace Nz
{
	class AbstractHash;
	class ByteArray;

	class NAZARA_CORE_API Stream
	{
		public:
			Stream(const Stream&) = delete;
			Stream(Stream&&) noexcept = default;
			virtual ~Stream();

			bool EndOfStream() const;

			inline void EnableBuffering(bool buffering, std::size_t bufferSize = DefaultBufferSize);
			inline void EnableTextMode(bool textMode);

			inline void Flush();

			UInt64 GetCursorPos() const;
			virtual std::filesystem::path GetDirectory() const;
			inline const void* GetMappedPointer() const;
			inline void* GetMappedPointerMutable();
			inline OpenModeFlags GetOpenMode() const;
			virtual std::filesystem::path GetPath() const;
			virtual UInt64 GetSize() const = 0;
			inline StreamOptionFlags GetStreamOptions() const;

			std::size_t Read(void* buffer, std::size_t size);
			virtual void ReadLine(std::string& line, unsigned int lineSize = 0);
			inline std::string ReadLine(unsigned int lineSize = 0);

			inline bool IsBufferingEnabled() const;
			inline bool IsMemoryMapped() const;
			inline bool IsReadable() const;
			inline bool IsSequential() const;
			inline bool IsTextModeEnabled() const;
			inline bool IsWritable() const;

			bool SetCursorPos(UInt64 offset);

			bool Write(const ByteArray& byteArray);
			bool Write(std::string_view string);
			inline std::size_t Write(const void* buffer, std::size_t size);

			Stream& operator=(const Stream&) = delete;
			Stream& operator=(Stream&&) noexcept = default;

			static constexpr std::size_t DefaultBufferSize = 0xFFFF;

		protected:
			inline Stream(StreamOptionFlags streamOptions = StreamOption::None, OpenModeFlags openMode = OpenMode::NotOpen);

			virtual void FlushStream() = 0;
			virtual void* GetMemoryMappedPointer() const;
			virtual std::size_t ReadBlock(void* buffer, std::size_t size) = 0;
			virtual bool SeekStreamCursor(UInt64 offset) = 0;
			virtual UInt64 TellStreamCursor() const = 0;
			virtual bool TestStreamEnd() const = 0;
			virtual std::size_t WriteBlock(const void* buffer, std::size_t size) = 0;

			std::size_t m_bufferCapacity;
			std::size_t m_bufferOffset;
			std::size_t m_bufferSize;
			std::unique_ptr<UInt8[]> m_buffer;
			OpenModeFlags m_openMode;
			StreamOptionFlags m_streamOptions;
			UInt64 m_bufferCursor;
	};

	NAZARA_CORE_API bool HashAppend(AbstractHash& hash, Stream& stream);
}

#include <Nazara/Core/Stream.inl>

#endif // NAZARA_CORE_STREAM_HPP
