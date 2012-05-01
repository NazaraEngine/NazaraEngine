// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILEIMPL_HPP
#define NAZARA_FILEIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Utility/NonCopyable.hpp>
#include <ctime>
#include <windows.h>

class NzFile;
class NzString;

class NzFileImpl : NzNonCopyable
{
	public:
		NzFileImpl(const NzFile* parent);
		~NzFileImpl();

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
		HANDLE m_handle;
		bool m_endOfFile;
};

#endif // NAZARA_FILEIMPL_HPP
