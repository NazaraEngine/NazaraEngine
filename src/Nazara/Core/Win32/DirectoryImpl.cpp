// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DirectoryImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DirectoryImpl::DirectoryImpl(const Directory* parent)
	{
		NazaraUnused(parent);
	}

	void DirectoryImpl::Close()
	{
		FindClose(m_handle);
	}

	String DirectoryImpl::GetResultName() const
	{
		return String::Unicode(m_result.cFileName);
	}

	UInt64 DirectoryImpl::GetResultSize() const
	{
		LARGE_INTEGER size;
		size.HighPart = m_result.nFileSizeHigh;
		size.LowPart = m_result.nFileSizeLow;

		return size.QuadPart;
	}

	bool DirectoryImpl::IsResultDirectory() const
	{
		if (m_result.dwFileAttributes != INVALID_FILE_ATTRIBUTES)
			return (m_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		else
			return false;
	}

	bool DirectoryImpl::NextResult()
	{
		if (m_firstCall) // Nous devons ignorer le premier appel car FindFirstFile nous a déjà renvoyé des résultats
		{
			m_firstCall = false;
			return true;
		}

		if (FindNextFileW(m_handle, &m_result))
			return true;
		else
		{
			if (GetLastError() != ERROR_NO_MORE_FILES)
				NazaraError("Unable to get next result: " + Error::GetLastSystemError());

			return false;
		}
	}

	bool DirectoryImpl::Open(const String& dirPath)
	{
		String searchPath = dirPath + "\\*";

		m_handle = FindFirstFileW(searchPath.GetWideString().data(), &m_result);
		if (m_handle == INVALID_HANDLE_VALUE)
		{
			NazaraError("Unable to open directory: " + Error::GetLastSystemError());
			return false;
		}

		m_firstCall = true;

		return true;
	}

	bool DirectoryImpl::Create(const String& dirPath)
	{
		return (CreateDirectoryW(dirPath.GetWideString().data(), nullptr) != 0) || GetLastError() == ERROR_ALREADY_EXISTS;
	}

	bool DirectoryImpl::Exists(const String& dirPath)
	{
		DWORD attributes = GetFileAttributesW(dirPath.GetWideString().data());
		if (attributes != INVALID_FILE_ATTRIBUTES)
			return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		else
			return false;
	}

	String DirectoryImpl::GetCurrent()
	{
		String currentPath;
		std::unique_ptr<wchar_t[]> path(new wchar_t[MAX_PATH]);

		unsigned int size = GetCurrentDirectoryW(MAX_PATH, path.get());
		if (size > MAX_PATH) // La taille prends en compte le caractère nul
		{
			path.reset(new wchar_t[size]);
			if (GetCurrentDirectoryW(size, path.get()) != 0)
				currentPath = String::Unicode(path.get());
			else
				NazaraError("Unable to get current directory: " + Error::GetLastSystemError());
		}
		else if (size == 0)
			NazaraError("Unable to get current directory: " + Error::GetLastSystemError());
		else
			currentPath = String::Unicode(path.get());

		return currentPath;
	}

	bool DirectoryImpl::Remove(const String& dirPath)
	{
		bool success = RemoveDirectoryW(dirPath.GetWideString().data()) != 0;

		DWORD error = GetLastError();
		return success || error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
	}
}
