// Copyright (C) 2024 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_FILEIMPL_HPP
#define NAZARA_CORE_POSIX_FILEIMPL_HPP

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <ctime>
#include <filesystem>

#if defined(NAZARA_PLATFORM_MACOS)
	#define Stat stat
	#define Fstat fstat
	#define Off_t off_t
	#define Lseek lseek
	#define Open_def open
	#define Ftruncate ftruncate
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_WEB)
	#define Stat stat64
	#define Fstat fstat64
	#define Off_t off64_t
	#define Lseek lseek64
	#define Open_def open64
	#define Ftruncate ftruncate64
#else
    #error This operating system is not fully supported by the Nazara Engine
#endif

namespace Nz
{
	class File;
	class String;

	class FileImpl
	{
		public:
			FileImpl(const File* parent);
			FileImpl(const FileImpl&) = delete;
			FileImpl(FileImpl&&) = delete; ///TODO
			~FileImpl();

			bool EndOfFile() const;
			void Flush();
			UInt64 GetCursorPos() const;
			bool Open(const std::filesystem::path& filePath, OpenModeFlags mode);
			std::size_t Read(void* buffer, std::size_t size);
			bool SetCursorPos(CursorPosition pos, Int64 offset);
			bool SetSize(UInt64 size);
			std::size_t Write(const void* buffer, std::size_t size);

			FileImpl& operator=(const FileImpl&) = delete;
			FileImpl& operator=(FileImpl&&) = delete; ///TODO

		private:
			int m_fileDescriptor;
			mutable bool m_endOfFile;
			mutable bool m_endOfFileUpdated;
	};
}

#endif // NAZARA_CORE_POSIX_FILEIMPL_HPP
