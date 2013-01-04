// Copyright (C) 2012 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/DirectoryImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

NzDirectoryImpl::NzDirectoryImpl(const NzDirectory* parent)
{
	NazaraUnused(parent);
}

void NzDirectoryImpl::Close()
{
	closedir(m_handle);
}

NzString NzDirectoryImpl::GetResultName() const
{
	return m_result.d_name;
}

nzUInt64 NzDirectoryImpl::GetResultSize() const
{
	struct stat64 resulststat;
	stat64(m_result.d_name, &resulststat);

	return static_cast<nzUInt64>(resulststat.st_size);
}

bool NzDirectoryImpl::IsResultDirectory() const
{
	return S_ISDIR(m_result.d_name);
}

bool NzDirectoryImpl::NextResult()
{
	if ((m_result = readdir64(m_handle)))
		return true;
	else
	{
		if (errno != ENOENT)
			NazaraError("Unable to get next result: " + NzGetLastSystemError());

		return false;
	}
}

bool NzDirectoryImpl::Open(const NzString& dirPath)
{
	m_handle = opendir(dirPath.GetConstBuffer());
	if (!m_handle)
	{
		NazaraError("Unable to open directory: " + NzGetLastSystemError());
		return false;
	}

	return true;
}

bool NzDirectoryImpl::Create(const NzString& dirPath)
{
	mode_t permissions; // TODO: check permissions

	return mkdir(dirPath.GetConstBuffer(), permissions) != -1;;
}

bool NzDirectoryImpl::Exists(const NzString& dirPath)
{
	if (S_ISDIR(dirPath.GetConstBuffer()))
		return true;

	return false;
}

NzString NzDirectoryImpl::GetCurrent()
{
	NzString currentPath;
	char* path = new char[_PC_PATH_MAX];

	if (getcwd(path, _PC_PATH_MAX))
		currentPath = path;
	else
		NazaraError("Unable to get current directory: " + NzGetLastSystemError());

	delete[] path;

	return currentPath;
}

bool NzDirectoryImpl::Remove(const NzString& dirPath)
{
	bool success = rmdir(dirPath.GetConstBuffer()) != -1;

	return success;
}
