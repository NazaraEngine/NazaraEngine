// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STREAM_HPP
#define NAZARA_STREAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Enums.hpp>

namespace Nz
{
	class ByteArray;
	class String; //< Do not include String.hpp in this file

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
			virtual String GetDirectory() const;
			virtual String GetPath() const;
			inline OpenModeFlags GetOpenMode() const;
			inline StreamOptionFlags GetStreamOptions() const;

			virtual UInt64 GetSize() const = 0;

			inline std::size_t Read(void* buffer, std::size_t size);
			virtual String ReadLine(unsigned int lineSize = 0);

			inline bool IsReadable() const;
			inline bool IsSequential() const;
			inline bool IsTextModeEnabled() const;
			inline bool IsWritable() const;

			virtual bool SetCursorPos(UInt64 offset) = 0;

			bool Write(const ByteArray& byteArray);
			bool Write(const String& string);
			inline std::size_t Write(const void* buffer, std::size_t size);

			Stream& operator=(const Stream&) = default;
			Stream& operator=(Stream&&) noexcept = default;

		protected:
			inline Stream(StreamOptionFlags streamOptions = StreamOption_None, OpenModeFlags openMode = OpenMode_NotOpen);

			virtual void FlushStream() = 0;
			virtual std::size_t ReadBlock(void* buffer, std::size_t size) = 0;
			virtual std::size_t WriteBlock(const void* buffer, std::size_t size) = 0;

			OpenModeFlags m_openMode;
			StreamOptionFlags m_streamOptions;
	};
}

#include <Nazara/Core/Stream.inl>

#endif // NAZARA_STREAM_HPP
