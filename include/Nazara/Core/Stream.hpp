// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STREAM_HPP
#define NAZARA_CORE_STREAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Enums.hpp>
#include <filesystem>
#include <string>

namespace Nz
{
	class ByteArray;

	class NAZARA_CORE_API Stream
	{
		public:
			Stream(const Stream&) = default;
			Stream(Stream&&) noexcept = default;
			virtual ~Stream();

			virtual bool EndOfStream() const = 0;

			inline void EnableTextMode(bool textMode);

			inline void Flush();

			virtual UInt64 GetCursorPos() const = 0;
			virtual std::filesystem::path GetDirectory() const;
			virtual std::filesystem::path GetPath() const;
			inline OpenModeFlags GetOpenMode() const;
			inline StreamOptionFlags GetStreamOptions() const;

			virtual UInt64 GetSize() const = 0;

			inline std::size_t Read(void* buffer, std::size_t size);
			virtual std::string ReadLine(unsigned int lineSize = 0);

			inline bool IsReadable() const;
			inline bool IsSequential() const;
			inline bool IsTextModeEnabled() const;
			inline bool IsWritable() const;

			virtual bool SetCursorPos(UInt64 offset) = 0;

			bool Write(const ByteArray& byteArray);
			bool Write(const std::string_view& string);
			inline std::size_t Write(const void* buffer, std::size_t size);

			Stream& operator=(const Stream&) = default;
			Stream& operator=(Stream&&) noexcept = default;

		protected:
			inline Stream(StreamOptionFlags streamOptions = StreamOption::None, OpenModeFlags openMode = OpenMode::NotOpen);

			virtual void FlushStream() = 0;
			virtual std::size_t ReadBlock(void* buffer, std::size_t size) = 0;
			virtual std::size_t WriteBlock(const void* buffer, std::size_t size) = 0;

			OpenModeFlags m_openMode;
			StreamOptionFlags m_streamOptions;
	};
}

#include <Nazara/Core/Stream.inl>

#endif // NAZARA_CORE_STREAM_HPP
