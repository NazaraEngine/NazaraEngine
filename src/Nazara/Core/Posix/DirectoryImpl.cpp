// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/DirectoryImpl.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Core/String.hpp>
#include <cstring>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DirectoryImpl::DirectoryImpl(const Directory* parent) :
	m_parent(parent)
	{
	}

	void DirectoryImpl::Close()
	{
		closedir(m_handle);
	}

	String DirectoryImpl::GetResultName() const
	{
		return m_result->d_name;
	}

	UInt64 DirectoryImpl::GetResultSize() const
	{
		String path = m_parent->GetPath();
		std::size_t pathSize = path.GetSize();

		std::size_t resultNameSize = std::strlen(m_result->d_name);

		std::size_t fullNameSize = pathSize + 1 + resultNameSize;
		StackArray<char> fullName = NazaraStackArrayNoInit(char, fullNameSize + 1);
		std::memcpy(&fullName[0], path.GetConstBuffer(), pathSize * sizeof(char));
		fullName[pathSize] = '/';
		std::memcpy(&fullName[pathSize + 1], m_result->d_name, resultNameSize * sizeof(char));
		fullName[fullNameSize] = '\0';

		struct stat64 results;
		stat64(fullName.data(), &results);

		return results.st_size;
	}

	bool DirectoryImpl::IsResultDirectory() const
	{
		//TODO: Fix d_type handling (field can be missing or be a symbolic link, both cases which must be handled by calling stat)

		return m_result->d_type == DT_DIR;
	}

	bool DirectoryImpl::NextResult()
	{
		if ((m_result = readdir64(m_handle)))
			return true;
		else
		{
			if (errno == EBADF || errno == EOVERFLOW)
				NazaraError("Unable to get next result: " + Error::GetLastSystemError());

			return false;
		}
	}

	bool DirectoryImpl::Open(const String& dirPath)
	{
		m_handle = opendir(dirPath.GetConstBuffer());
		if (!m_handle)
		{
			NazaraError("Unable to open directory: " + Error::GetLastSystemError());
			return false;
		}

		return true;
	}

	bool DirectoryImpl::Create(const String& dirPath)
	{
		mode_t permissions = S_IRWXU | S_IRWXG | S_IRWXO; // 777
		return mkdir(dirPath.GetConstBuffer(), permissions) != -1;
	}

	bool DirectoryImpl::Exists(const String& dirPath)
	{
		struct stat64 filestats;
		if (stat64(dirPath.GetConstBuffer(), &filestats) == -1) // error
			return false;

		return S_ISDIR(filestats.st_mode) || S_ISREG(filestats.st_mode);
	}

	String DirectoryImpl::GetCurrent()
	{
		String currentPath;

		char path[MAXPATHLEN];
		if (getcwd(path, MAXPATHLEN))
			currentPath = path;
		else
			NazaraError("Unable to get current directory: " + Error::GetLastSystemError()); // Bug: initialisation -> if no path for log !

		return currentPath;
	}

	bool DirectoryImpl::Remove(const String& dirPath)
	{
		bool success = rmdir(dirPath.GetConstBuffer()) != -1;

		return success;
	}
}
