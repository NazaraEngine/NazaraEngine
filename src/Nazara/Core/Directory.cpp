// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <cstddef>
#include <cstring>
#include <string>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/DirectoryImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/DirectoryImpl.hpp>
#else
	#error OS not handled
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DIRECTORY
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
	    //FIXME: MinGW seems to dislike thread_local shared_ptr.. (using a std::string is a working hackfix)
		thread_local std::string currentPath(DirectoryImpl::GetCurrent());
	}

	Directory::Directory() :
	m_pattern('*'),
	m_impl(nullptr)
	{
	}

	Directory::Directory(const String& dirPath) :
	m_dirPath(dirPath),
	m_pattern('*'),
	m_impl(nullptr)
	{
	}

	Directory::~Directory()
	{
		Close();
	}

	void Directory::Close()
	{
		NazaraLock(m_mutex);

		if (m_impl)
		{
			m_impl->Close();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	bool Directory::Exists() const
	{
		NazaraLock(m_mutex);

		if (IsOpen())
			return true; // Le fichier est ouvert, donc il existe
		else
			return Exists(m_dirPath);
	}

	String Directory::GetPath() const
	{
		NazaraLock(m_mutex);

		return m_dirPath;
	}

	String Directory::GetPattern() const
	{
		NazaraLock(m_mutex);

		return m_pattern;
	}

	String Directory::GetResultName() const
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!m_impl)
		{
			NazaraError("Directory not opened");
			return String();
		}
		#endif

		return m_impl->GetResultName();
	}

	String Directory::GetResultPath() const
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!m_impl)
		{
			NazaraError("Directory not opened");
			return String();
		}
		#endif

		return m_dirPath + NAZARA_DIRECTORY_SEPARATOR + m_impl->GetResultName();
	}

	UInt64 Directory::GetResultSize() const
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

	bool Directory::IsOpen() const
	{
		NazaraLock(m_mutex);

		return m_impl != nullptr;
	}

	bool Directory::IsResultDirectory() const
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

	bool Directory::NextResult(bool skipDots)
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!m_impl)
		{
			NazaraError("Directory not opened");
			return false;
		}
		#endif

		String name;
		for (;;)
		{
			if (!m_impl->NextResult())
				return false;

			name = m_impl->GetResultName();

			if (skipDots && (name == '.' || name == ".."))
				continue;

			if (name.Match(m_pattern))
				break;
		}

		return true;
	}

	bool Directory::Open()
	{
		NazaraLock(m_mutex);

		Close();

		if (!Exists(m_dirPath))
			return false;

		m_impl = new DirectoryImpl(this);
		if (!m_impl->Open(m_dirPath))
		{
			delete m_impl;
			m_impl = nullptr;

			return false;
		}

		return true;
	}

	void Directory::SetPath(const String& dirPath)
	{
		NazaraLock(m_mutex);

		Close();

		m_dirPath = File::AbsolutePath(dirPath);
	}

	void Directory::SetPattern(const String& pattern)
	{
		NazaraLock(m_mutex);

		m_pattern = pattern;
	}

	bool Directory::Copy(const String& sourcePath, const String& destPath)
	{
		if (sourcePath.IsEmpty() || destPath.IsEmpty())
			return false;

		String dirPath(sourcePath);
		String dest(File::NormalizePath(destPath));

		if (!Create(destPath, true))
		{
			NazaraError("Unable to create \"" + destPath + '"');
			return false;
		}

		Directory dir(dirPath);
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
			else if (!File::Copy(dir.GetResultPath(), dest + NAZARA_DIRECTORY_SEPARATOR + dir.GetResultName()))
			{
				NazaraError("Failed to copy \"" + dir.GetResultPath() + "\" to \"" + dest + NAZARA_DIRECTORY_SEPARATOR + dir.GetResultName() + '"');
				return false;
			}
		}

		dir.Close();

		return true;
	}

	bool Directory::Create(const String& dirPath, bool recursive)
	{
		if (dirPath.IsEmpty())
			return false;

		if (recursive)
		{
			String path = File::NormalizePath(dirPath);
			std::size_t foundPos = path.Find(NAZARA_DIRECTORY_SEPARATOR);
			if (foundPos == String::npos)
				return false;

			#ifdef NAZARA_PLATFORM_WINDOWS
			// Contrairement au disque (Ex: "C:"), le chemin réseau n'est pas considéré comme un dossier (Ex: "\\Laptop")
			if (path.Match("\\\\*"))
			{
				foundPos = path.Find('\\', 2);
				if (foundPos == String::npos)
					return false;

				foundPos = path.Find('\\', foundPos + 1);
				if (foundPos == String::npos)
					return false;
			}
			#endif

			for (;;)
			{
				String p = path.SubString(0, foundPos);
				if (p.EndsWith(NAZARA_DIRECTORY_SEPARATOR))
					p = p.SubString(0, -2);

				if (!DirectoryImpl::Exists(p) && !DirectoryImpl::Create(p))
					return false;

				if (foundPos == String::npos)
					break;

				foundPos = path.Find(NAZARA_DIRECTORY_SEPARATOR, foundPos + 1);
			}

			return true;
		}
		else
			return DirectoryImpl::Create(File::NormalizePath(dirPath));
	}

	bool Directory::Exists(const String& dirPath)
	{
		if (dirPath.IsEmpty())
			return false;

		return DirectoryImpl::Exists(File::NormalizePath(dirPath));
	}

	String Directory::GetCurrent()
	{
		return currentPath;
	}

	const char* Directory::GetCurrentFileRelativeToEngine(const char* currentFile)
	{
		///FIXME: Est-ce que cette méthode est au bon endroit ?
		const char* ptr = std::strstr(currentFile, "NazaraEngine/");
		if (!ptr)
			ptr = std::strstr(currentFile, "NazaraEngine\\");

		if (!ptr)
			ptr = currentFile;

		return ptr;
	}

	bool Directory::Remove(const String& dirPath, bool emptyDirectory)
	{
		if (dirPath.IsEmpty())
			return false;

		if (emptyDirectory)
		{
			Directory dir(dirPath);
			if (!dir.Open())
				return DirectoryImpl::Remove(dirPath); // Si on n'arrive pas à ouvrir le dossier, on tente de le supprimer

			while (dir.NextResult(true))
			{
				if (dir.IsResultDirectory())
				{
					if (!Remove(dir.GetResultPath(), true))
						return false;
				}
				else if (!File::Delete(dir.GetResultPath()))
				{
					NazaraError(dir.GetResultPath());
					return false;
				}
			}

			dir.Close();
		}

		return DirectoryImpl::Remove(File::NormalizePath(dirPath));
	}

	bool Directory::SetCurrent(const String& dirPath)
	{
		String path = File::AbsolutePath(dirPath);
		if (DirectoryImpl::Exists(path))
		{
			currentPath = path;
			return true;
		}
		else
			return false;
		}
}
