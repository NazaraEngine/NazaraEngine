// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/DirectoryImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

#include <errno.h>
#include <sys/param.h>

namespace Nz
{
	DirectoryImpl::DirectoryImpl(const Directory* parent)
	{
		NazaraUnused(parent);
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
		struct stat64 resulststat;
		stat64(m_result->d_name, &resulststat);

		return static_cast<UInt64>(resulststat.st_size);
	}

	bool DirectoryImpl::IsResultDirectory() const
	{
		struct stat64 filestats;
		if (stat64(m_result->d_name, &filestats) == -1) // error
			return false;

		return S_ISDIR(filestats.st_mode);
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
		mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // TODO: check permissions, no right to execute but read and write for every others.
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
