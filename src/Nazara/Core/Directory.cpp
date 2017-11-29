// Copyright (C) 2017 Jérôme Leclercq
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

	/*!
	* \ingroup core
	* \class Nz::Directory
	* \brief Core class that represents a directory
	*/

	/*!
	* \brief Constructs a Directory object by default
	*/

	Directory::Directory() :
	m_pattern('*'),
	m_impl(nullptr)
	{
	}

	/*!
	* \brief Constructs a Directory object with a path
	*
	* \param dirPath Path to the directory
	*/

	Directory::Directory(const String& dirPath) :
	m_dirPath(dirPath),
	m_pattern('*'),
	m_impl(nullptr)
	{
	}

	/*!
	* \brief Destructs the object and calls Close
	*
	* \see Close
	*/

	Directory::~Directory()
	{
		Close();
	}

	/*!
	* \brief Closes the directory
	*/

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

	/*!
	* \brief Checks whether the directory exists
	* \return true if directory exists
	*/

	bool Directory::Exists() const
	{
		NazaraLock(m_mutex);

		if (IsOpen())
			return true; // If directory is open, then it exists
		else
			return Exists(m_dirPath);
	}

	/*!
	* \brief Gets the path of the directory
	* \return Path of the directory
	*/

	String Directory::GetPath() const
	{
		NazaraLock(m_mutex);

		return m_dirPath;
	}

	/*!
	* \brief Gets the pattern for the path of the directory
	* \return Pattern for the path of the directory
	*/

	String Directory::GetPattern() const
	{
		NazaraLock(m_mutex);

		return m_pattern;
	}

	/*!
	* \brief Gets the result name of the directory
	* \return Resulting name
	*
	* \remark Produces a NazaraError if directory is not open with NAZARA_CORE_SAFE defined
	*/

	String Directory::GetResultName() const
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
		{
			NazaraError("Directory not opened");
			return String();
		}
		#endif

		return m_impl->GetResultName();
	}

	/*!
	* \brief Gets the result path of the directory
	* \return Resulting path
	*
	* \remark Produces a NazaraError if directory is not open with NAZARA_CORE_SAFE defined
	*/

	String Directory::GetResultPath() const
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
		{
			NazaraError("Directory not opened");
			return String();
		}
		#endif

		return m_dirPath + NAZARA_DIRECTORY_SEPARATOR + m_impl->GetResultName();
	}

	/*!
	* \brief Gets the resulting size of the directory
	* \return Size of the directory
	*
	* \remark Produces a NazaraError if directory is not open with NAZARA_CORE_SAFE defined
	*/

	UInt64 Directory::GetResultSize() const
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
		{
			NazaraError("Directory not opened");
			return 0;
		}
		#endif

		return m_impl->GetResultSize();
	}

	/*!
	* \brief Checks whether the directory is open
	* \return true if open
	*/

	bool Directory::IsOpen() const
	{
		NazaraLock(m_mutex);

		return m_impl != nullptr;
	}

	/*!
	* \brief Checks whether the directory is result
	* \return true if result
	*
	* \remark Produces a NazaraError if directory is not open with NAZARA_CORE_SAFE defined
	*/

	bool Directory::IsResultDirectory() const
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
		{
			NazaraError("Directory not opened");
			return false;
		}
		#endif

		return m_impl->IsResultDirectory();
	}

	/*!
	* \brief Sets the next result in the directory
	* \return true if directory has a next result
	*
	* \param skipDots Skips the dots in the path
	*
	* \remark Produces a NazaraError if directory is not open with NAZARA_CORE_SAFE defined
	*/

	bool Directory::NextResult(bool skipDots)
	{
		NazaraLock(m_mutex);

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
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

	/*!
	* \brief Opens the directory
	* \return true if opening is successful
	*/

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

	/*!
	* \brief Sets the path of the directory
	*
	* \param dirPath Path of the directory
	*/

	void Directory::SetPath(const String& dirPath)
	{
		NazaraLock(m_mutex);

		Close();

		m_dirPath = File::AbsolutePath(dirPath);
	}

	/*!
	* \brief Sets the pattern of the directory
	*
	* \param pattern Pattern of the directory
	*/

	void Directory::SetPattern(const String& pattern)
	{
		NazaraLock(m_mutex);

		m_pattern = pattern;
	}

	/*!
	* \brief Copies the first directory to a new directory path
	* \return true if copy is successful
	*
	* \param sourcePath Path of the original directory
	* \param destPath Path of the copied directory
	*
	* \remark Produces a NazaraError if could not create destination directory
	* \remark Produces a NazaraError if could not open origin directory
	* \remark Produces a NazaraError if could not copy a file
	*/

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

	/*!
	* \brief Creates a directory from a path
	* \return true if creation is successful
	*
	* \param dirPath Path of the directory
	* \param recursive Creates subdirectories
	*/

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
			// Unlike to disk (Ex: "C:"), the network path is not considered as a directory (Ex: "\\Laptop")
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

				if (!p.IsEmpty())
				{
					if (!DirectoryImpl::Exists(p) && !DirectoryImpl::Create(p))
						return false;

					if (foundPos == String::npos)
						break;
				}

				foundPos = path.Find(NAZARA_DIRECTORY_SEPARATOR, foundPos + 1);
			}

			return true;
		}
		else
			return DirectoryImpl::Create(File::NormalizePath(dirPath));
	}

	/*!
	* \brief Checks whether the directory exists
	* \return true if directory exists
	*
	* \param dirPath Path of the directory
	*/

	bool Directory::Exists(const String& dirPath)
	{
		if (dirPath.IsEmpty())
			return false;

		return DirectoryImpl::Exists(File::NormalizePath(dirPath));
	}

	/*!
	* \brief Gets the current path of this directory
	* \return Current path
	*/

	String Directory::GetCurrent()
	{
		return currentPath;
	}

	/*!
	* \brief Gets this current file relative to the engine
	* \return Path to this file
	*/

	const char* Directory::GetCurrentFileRelativeToEngine(const char* currentFile)
	{
		///FIXME: Is this method in the right place ?
		const char* ptr = std::strstr(currentFile, "NazaraEngine/");
		if (!ptr)
			ptr = std::strstr(currentFile, "NazaraEngine\\");

		if (!ptr)
			ptr = currentFile;

		return ptr;
	}

	/*!
	* \brief Removes the directory
	* \return true if remove is successful
	*
	* \param dirPath Path of the directory
	* \param emptyDirectory Remove recursively
	*/

	bool Directory::Remove(const String& dirPath, bool emptyDirectory)
	{
		if (dirPath.IsEmpty())
			return false;

		if (emptyDirectory)
		{
			Directory dir(dirPath);
			if (!dir.Open())
				return DirectoryImpl::Remove(dirPath); // If we can't open the directory, we try to delete it

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

	/*!
	* \brief Sets the current directory
	* \return true if directory path exists
	*
	* \param dirPath Path of the directory
	*/

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
