// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILE_HPP
#define NAZARA_FILE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_FILE
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <ctime>

namespace Nz
{
	class FileImpl;

	class NAZARA_CORE_API File : public Stream
	{
		public:
			File();
			File(const String& filePath);
			File(const String& filePath, UInt32 openMode);
			File(const File&) = delete;
			File(File&& file) noexcept;
			~File();

			bool Copy(const String& newFilePath);
			void Close();

			bool Delete();

			bool EndOfFile() const;
			bool EndOfStream() const override;

			bool Exists() const;

			time_t GetCreationTime() const;
			UInt64 GetCursorPos() const override;
			String GetDirectory() const override;
			String GetFileName() const;
			time_t GetLastAccessTime() const;
			time_t GetLastWriteTime() const;
			String GetPath() const override;
			UInt64 GetSize() const override;

			bool IsOpen() const;

			bool Open(unsigned int openMode = OpenMode_NotOpen);
			bool Open(const String& filePath, unsigned int openMode = OpenMode_NotOpen);

			bool Rename(const String& newFilePath);

			bool SetCursorPos(CursorPosition pos, Int64 offset = 0);
			bool SetCursorPos(UInt64 offset) override;
			bool SetFile(const String& filePath);
			bool SetSize(UInt64 size);

			File& operator=(const String& filePath);
			File& operator=(const File&) = delete;
			File& operator=(File&& file) noexcept;

			static String AbsolutePath(const String& filePath);
			static inline ByteArray ComputeHash(HashType hash, const String& filePath);
			static inline ByteArray ComputeHash(AbstractHash* hash, const String& filePath);
			static bool Copy(const String& sourcePath, const String& targetPath);
			static bool Delete(const String& filePath);
			static bool Exists(const String& filePath);
			static time_t GetCreationTime(const String& filePath);
			static String GetDirectory(const String& filePath);
			static time_t GetLastAccessTime(const String& filePath);
			static time_t GetLastWriteTime(const String& filePath);
			static UInt64 GetSize(const String& filePath);
			static bool IsAbsolute(const String& filePath);
			static String NormalizePath(const String& filePath);
			static String NormalizeSeparators(const String& filePath);
			static bool Rename(const String& sourcePath, const String& targetPath);

		private:
			NazaraMutexAttrib(m_mutex, mutable)

			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			String m_filePath;
			FileImpl* m_impl;
	};

	NAZARA_CORE_API bool HashAppend(AbstractHash* hash, const File& originalFile);
}

#include <Nazara/Core/File.inl>

#endif // NAZARA_FILE_HPP
