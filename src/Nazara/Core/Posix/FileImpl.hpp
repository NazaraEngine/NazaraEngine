// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILEIMPL_HPP
#define NAZARA_FILEIMPL_HPP

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
    04/01/2012 : alexandre.janniaux@gmail.com
    Add posix file implementation.
*/

class NzFile;
class NzString;

class NzFileImpl : NzNonCopyable
{
	public:
		NzFileImpl(const NzFile* parent);
		~NzFileImpl() = default;

		void Close();
		bool EndOfFile() const;
		void Flush();
		nzUInt64 GetCursorPos() const;
		bool Open(const NzString& filePath, unsigned int mode);
		std::size_t Read(void* buffer, std::size_t size);
		bool SetCursorPos(NzFile::CursorPosition pos, nzInt64 offset);
		std::size_t Write(const void* buffer, std::size_t size);

		static bool Copy(const NzString& sourcePath, const NzString& targetPath);
		static bool Delete(const NzString& filePath);
		static bool Exists(const NzString& filePath);
		static time_t GetCreationTime(const NzString& filePath);
		static time_t GetLastAccessTime(const NzString& filePath);
		static time_t GetLastWriteTime(const NzString& filePath);
		static nzUInt64 GetSize(const NzString& filePath);
		static bool Rename(const NzString& sourcePath, const NzString& targetPath);

	private:
		int m_fileDescriptor;
		FILE* m_handle;
		mutable bool m_endOfFile;
		mutable bool m_endOfFileUpdated;
};

#endif // NAZARA_FILEIMPL_HPP
