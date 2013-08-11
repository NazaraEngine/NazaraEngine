// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <cstring>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/DirectoryImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/DirectoryImpl.hpp>
#else
	#error OS not handled
#endif

#include <Nazara/Core/Debug.hpp>

namespace
{
	/*thread_local*/ NzString currentPath(NzDirectoryImpl::GetCurrent());
}

NzDirectory::NzDirectory() :
m_pattern('*')
{
}

NzDirectory::NzDirectory(const NzString& dirPath) :
m_dirPath(dirPath),
m_pattern('*')
{
}

NzDirectory::~NzDirectory()
{
	Close();
}

void NzDirectory::Close()
{
	NazaraLock(m_mutex);

	if (m_impl)
	{
		m_impl->Close();
		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzDirectory::Exists() const
{
	NazaraLock(m_mutex);

	if (IsOpen())
		return true; // Le fichier est ouvert, donc il existe
	else
		return Exists(m_dirPath);
}

NzString NzDirectory::GetPattern() const
{
	NazaraLock(m_mutex);

	return m_pattern;
}

NzString NzDirectory::GetResultName() const
{
	NazaraLock(m_mutex);

	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("Directory not opened");
		return NzString();
	}
	#endif

	return m_impl->GetResultName();
}

NzString NzDirectory::GetResultPath() const
{
	NazaraLock(m_mutex);

	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("Directory not opened");
		return NzString();
	}
	#endif

	return m_dirPath + NAZARA_DIRECTORY_SEPARATOR + m_impl->GetResultName();
}

nzUInt64 NzDirectory::GetResultSize() const
{
	NazaraLock(m_mutex);

	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("Directory not opened");
		return 0;
	}
	#endif

	return m_impl->GetResultSize();
}

bool NzDirectory::IsOpen() const
{
	NazaraLock(m_mutex);

	return m_impl != nullptr;
}

bool NzDirectory::IsResultDirectory() const
{
	NazaraLock(m_mutex);

	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("Directory not opened");
		return false;
	}
	#endif

	return m_impl->IsResultDirectory();
}

bool NzDirectory::NextResult(bool skipDots)
{
	NazaraLock(m_mutex);

	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("Directory not opened");
		return false;
	}
	#endif

	NzString name;
	do
	{
		if (!m_impl->NextResult())
			return false;

		name = m_impl->GetResultName();

		if (skipDots && (name == '.' || name == ".."))
			continue;

		if (name.Match(m_pattern))
			break;
	}
	while (true);

	return true;
}

bool NzDirectory::Open()
{
	NazaraLock(m_mutex);

	Close();

	if (!Exists(m_dirPath))
		return false;

	m_impl = new NzDirectoryImpl(this);
	if (!m_impl->Open(m_dirPath))
	{
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	return true;
}

void NzDirectory::SetDirectory(const NzString& dirPath)
{
	NazaraLock(m_mutex);

	Close();

	m_dirPath = NzFile::AbsolutePath(dirPath);
}

void NzDirectory::SetPattern(const NzString& pattern)
{
	NazaraLock(m_mutex);

	m_pattern = pattern;
}

bool NzDirectory::Copy(const NzString& sourcePath, const NzString& destPath)
{
	if (sourcePath.IsEmpty() || destPath.IsEmpty())
		return false;

	NzString dirPath(sourcePath);
	NzString dest(NzFile::NormalizePath(destPath));

	if (!Create(destPath, true))
	{
		NazaraError("Unable to create \"" + destPath + '"');
		return false;
	}

	NzDirectory dir(dirPath);
	if (!dir.Open())
	{
		NazaraError("Unable to open \"" + destPath + '"');
		return false;
	}

	while (dir.NextResult(true))
	{
		if (dir.IsResultDirectory())
		{
			if (!Copy(dir.GetResultPath(), dest + NAZARA_DIRECTORY_SEPARATOR + dir.GetResultName()))
				return false;
		}
		else if (!NzFile::Copy(dir.GetResultPath(), dest + NAZARA_DIRECTORY_SEPARATOR + dir.GetResultName()))
		{
			NazaraError("Failed to copy \"" + dir.GetResultPath() + "\" to \"" + dest + NAZARA_DIRECTORY_SEPARATOR + dir.GetResultName() + '"');
			return false;
		}
	}

	dir.Close();

	return true;
}

bool NzDirectory::Create(const NzString& dirPath, bool recursive)
{
	if (dirPath.IsEmpty())
		return false;

	if (recursive)
	{
		NzString path = NzFile::NormalizePath(dirPath);
		unsigned int foundPos = path.Find(NAZARA_DIRECTORY_SEPARATOR);
		if (foundPos == NzString::npos)
			return false;

		#ifdef NAZARA_PLATFORM_WINDOWS
		// Contrairement au disque (Ex: "C:"), le chemin réseau n'est pas considéré comme un dossier (Ex: "\\Laptop")
		if (path.Match("\\\\*"))
		{
			foundPos = path.Find('\\', 2);
			if (foundPos == NzString::npos)
				return false;

			foundPos = path.Find('\\', foundPos+1);
			if (foundPos == NzString::npos)
				return false;
		}
		#endif

		do
		{
			NzString p = path.SubString(0, foundPos);
			if (p.EndsWith(NAZARA_DIRECTORY_SEPARATOR))
				p = p.SubString(0, -2);

			if (!NzDirectoryImpl::Exists(p) && !NzDirectoryImpl::Create(p))
				return false;

			if (foundPos == NzString::npos)
				break;

			foundPos = path.Find(NAZARA_DIRECTORY_SEPARATOR, foundPos+1);
		}
		while (true);

		return true;
	}
	else
		return NzDirectoryImpl::Create(NzFile::NormalizePath(dirPath));
}

bool NzDirectory::Exists(const NzString& dirPath)
{
	if (dirPath.IsEmpty())
		return false;

	return NzDirectoryImpl::Exists(NzFile::NormalizePath(dirPath));
}

NzString NzDirectory::GetCurrent()
{
	return currentPath;
}

const char* NzDirectory::GetCurrentFileRelativeToEngine(const char* currentFile)
{
	///FIXME: Est-ce que cette méthode est au bon endroit ?
	static int offset = -1;

	if (offset < 0)
	{
		const char* directoryFile = __FILE__;
		const char* ptr = std::strstr(directoryFile, "NazaraEngine/src/Nazara/Core/Directory.cpp");
		if (ptr)
			offset = ptr - directoryFile;
		else
		{
			ptr = std::strstr(directoryFile, "NazaraEngine\\src\\Nazara\\Core\\Directory.cpp");
			if (ptr)
				offset = ptr - directoryFile;
			else
				offset = 0;
		}
	}

	return &currentFile[offset];
}

bool NzDirectory::Remove(const NzString& dirPath, bool emptyDirectory)
{
	if (dirPath.IsEmpty())
		return false;

	if (emptyDirectory)
	{
		NzDirectory dir(dirPath);
		if (!dir.Open())
			return NzDirectoryImpl::Remove(dirPath); // Si on n'arrive pas à ouvrir le dossier, on tente de le supprimer

		while (dir.NextResult(true))
		{
			if (dir.IsResultDirectory())
			{
				if (!Remove(dir.GetResultPath(), true))
					return false;
			}
			else if (!NzFile::Delete(dir.GetResultPath()))
			{
				NazaraError(dir.GetResultPath());
				return false;
			}
		}

		dir.Close();
	}

	return NzDirectoryImpl::Remove(NzFile::NormalizePath(dirPath));
}

bool NzDirectory::SetCurrent(const NzString& dirPath)
{
	NzString path = NzFile::AbsolutePath(dirPath);
	if (NzDirectoryImpl::Exists(path))
	{
		currentPath = path;
		return true;
	}
	else
		return false;
}
