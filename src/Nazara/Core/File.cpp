// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/File.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <cstring>
#include <memory>
#include <utility>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/FileImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/FileImpl.hpp>
#else
	#error OS not handled
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_FILE
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	File::File() :
	Stream(OpenMode_Current),
	m_impl(nullptr)
	{
	}

	File::File(const String& filePath) :
	File()
	{
		SetFile(filePath);
	}

	File::File(const String& filePath, UInt32 openMode) :
	File()
	{
		Open(filePath, openMode);
	}

	File::File(File&& file) noexcept :
	Stream(std::move(file)),
	InputStream(std::move(file)),
	OutputStream(std::move(file)),
	m_filePath(std::move(file.m_filePath)),
	m_impl(file.m_impl)
	{
		file.m_impl = nullptr;
	}

	File::~File()
	{
		Close();
	}

	bool File::Copy(const String& newFilePath)
	{
		return Copy(m_filePath, newFilePath);
	}

	void File::Close()
	{
		NazaraLock(m_mutex)

		if (m_impl)
		{
			m_impl->Close();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	bool File::Delete()
	{
		NazaraLock(m_mutex)

		Close();

		return Delete(m_filePath);
	}

	bool File::EndOfFile() const
	{
		NazaraLock(m_mutex)

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
		{
			NazaraError("File not opened");
			return false;
		}
		#endif

		return m_impl->EndOfFile();
	}

	bool File::EndOfStream() const
	{
		return EndOfFile();
	}

	bool File::Exists() const
	{
		NazaraLock(m_mutex)

		if (IsOpen())
			return true; // Le fichier est ouvert, donc il existe
		else
			return Exists(m_filePath);
	}

	void File::Flush()
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");
		NazaraAssert(IsWritable(), "File not opened with write access");

		m_impl->Flush();
	}

	time_t File::GetCreationTime() const
	{
		NazaraLock(m_mutex)

		return GetCreationTime(m_filePath);
	}

	UInt64 File::GetCursorPos() const
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not opened");

		return m_impl->GetCursorPos();
	}

	String File::GetDirectory() const
	{
		NazaraLock(m_mutex)

		return m_filePath.SubStringTo(NAZARA_DIRECTORY_SEPARATOR, -1, true, true);
	}

	String File::GetFileName() const
	{
		NazaraLock(m_mutex)

		return m_filePath.SubStringFrom(NAZARA_DIRECTORY_SEPARATOR, -1, true);
	}

	time_t File::GetLastAccessTime() const
	{
		NazaraLock(m_mutex)

		return GetLastAccessTime(m_filePath);
	}

	time_t File::GetLastWriteTime() const
	{
		NazaraLock(m_mutex)

		return GetLastWriteTime(m_filePath);
	}

	String File::GetPath() const
	{
		NazaraLock(m_mutex)

		return m_filePath;
	}

	UInt64 File::GetSize() const
	{
		NazaraLock(m_mutex)

		return GetSize(m_filePath);
	}

	bool File::IsOpen() const
	{
		NazaraLock(m_mutex)

		return m_impl != nullptr;
	}

	std::size_t File::Read(void* buffer, std::size_t size)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not opened");
		NazaraAssert(IsReadable(), "File not opened with read access");

		if (size == 0)
			return 0;

		if (buffer)
			return m_impl->Read(buffer, size);
		else
		{
			// Si nous ne devons rien lire, nous avançons simplement
			UInt64 currentPos = m_impl->GetCursorPos();

			m_impl->SetCursorPos(CursorPosition_AtCurrent, size);

			return static_cast<std::size_t>(m_impl->GetCursorPos() - currentPos);
		}
	}

	bool File::Rename(const String& newFilePath)
	{
		NazaraLock(m_mutex)

		bool opened = IsOpen();
		Close();

		bool success = Rename(m_filePath, newFilePath);
		if (success)
			m_filePath = NormalizePath(newFilePath);

		if (opened)
			Open();

		return success;
	}

	bool File::Open(unsigned int openMode)
	{
		NazaraLock(m_mutex)

		Close();

		if (m_filePath.IsEmpty())
			return false;

		if (openMode != 0)
			m_openMode = openMode;

		if (m_openMode == 0)
			return false;

		std::unique_ptr<FileImpl> impl(new FileImpl(this));
		if (!impl->Open(m_filePath, m_openMode))
		{
			ErrorFlags flags(ErrorFlag_Silent); // Silencieux par défaut
			NazaraError("Failed to open \"" + m_filePath + "\": " + Error::GetLastSystemError());
			return false;
		}

		m_impl = impl.release();

		if (m_openMode & OpenMode_Text)
			m_streamOptions |= StreamOption_Text;

		return true;
	}

	bool File::Open(const String& filePath, unsigned int openMode)
	{
		NazaraLock(m_mutex)

		Close();

		SetFile(filePath);
		return Open(openMode);
	}

	bool File::SetCursorPos(CursorPosition pos, Int64 offset)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not opened");

		return m_impl->SetCursorPos(pos, offset);
	}

	bool File::SetCursorPos(UInt64 offset)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not opened");

		return m_impl->SetCursorPos(CursorPosition_AtBegin, offset);
	}

	bool File::SetFile(const String& filePath)
	{
		NazaraLock(m_mutex)

		if (IsOpen())
		{
			if (filePath.IsEmpty())
				return false;

			std::unique_ptr<FileImpl> impl(new FileImpl(this));
			if (!impl->Open(filePath, m_openMode))
			{
				NazaraError("Failed to open new file; " + Error::GetLastSystemError());
				return false;
			}

			m_impl->Close();
			delete m_impl;

			m_impl = impl.release();
		}

		m_filePath = AbsolutePath(filePath);
		return true;
	}

	std::size_t File::Write(const void* buffer, std::size_t size)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not opened");
		NazaraAssert(IsWritable(), "File not opened with write access");

		if (size == 0)
			return 0;

		NazaraAssert(buffer, "Invalid buffer");

		return m_impl->Write(buffer, size);
	}

	File& File::operator=(const String& filePath)
	{
		SetFile(filePath);

		return *this;
	}

	File& File::operator=(File&& file) noexcept
	{
		NazaraLock(m_mutex)

		std::swap(m_filePath, file.m_filePath);
		std::swap(m_impl, file.m_impl);

		return *this;
	}

	String File::AbsolutePath(const String& filePath)
	{
		// Je n'utilise pas les fonctions de l'OS car elles ne fonctionnent que pour un chemin existant
		String path = NormalizePath(filePath);
		if (path.IsEmpty())
			return String();

		String base;
		unsigned int start;
		#ifdef NAZARA_PLATFORM_WINDOWS
		if (path.Match("?:*"))
			start = 1;
		else if (path.Match("\\\\*"))
		{
			base = "\\\\";
			start = 2;
		}
		else if (path.StartsWith('\\')) // Spécial : '\' fait référence au disque racine
		{
			String drive = Directory::GetCurrent().SubStringTo('\\');
			String end = path.SubString(1, -1);
			if (end.IsEmpty())
				path = drive;
			else
				path = drive + '\\' + end;

			start = 1;
		}
		else
		{
			NazaraError("Path unrecognized");
			return path;
		}
		#elif defined(NAZARA_PLATFORM_POSIX)
		base = '/';
		start = 0;
		#else
			#error OS case not implemented
		#endif

		static String upDir = NAZARA_DIRECTORY_SEPARATOR + String('.');

		if (path.Find(upDir) == String::npos)
			return path;

		std::vector<String> sep;
		if (path.Split(sep, NAZARA_DIRECTORY_SEPARATOR) <= 1)
			return path;

		// Nous avons un chemin absolu, mais il nous faut un peu le nettoyer
		for (unsigned int i = 0; i < sep.size(); ++i)
		{
			if (sep[i] == '.')
				sep.erase(sep.begin() + i--);
			else if (sep[i] == "..")
			{
				if (i > start) // Si nous ne sommes pas dans la partie protégée
					sep.erase(sep.begin() + i--);

				sep.erase(sep.begin() + i--);
			}
		}

		StringStream stream(base);
		for (unsigned int i = 0; i < sep.size(); ++i)
		{
			if (i != sep.size()-1)
				stream << sep[i] << NAZARA_DIRECTORY_SEPARATOR;
			else
				stream << sep[i];
		}

		return stream;
	}

	bool File::Copy(const String& sourcePath, const String& targetPath)
	{
		if (sourcePath.IsEmpty() || targetPath.IsEmpty())
			return false;

		return FileImpl::Copy(NormalizePath(sourcePath), NormalizePath(targetPath));
	}

	bool File::Delete(const String& filePath)
	{
		if (filePath.IsEmpty())
			return false;

		return FileImpl::Delete(NormalizePath(filePath));
	}

	bool File::Exists(const String& filePath)
	{
		if (filePath.IsEmpty())
			return false;

		return FileImpl::Exists(NormalizePath(filePath));
	}

	time_t File::GetCreationTime(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetCreationTime(NormalizePath(filePath));
	}

	String File::GetDirectory(const String& filePath)
	{
		return filePath.SubStringTo(NAZARA_DIRECTORY_SEPARATOR, -1, true, true);
	}

	time_t File::GetLastAccessTime(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetLastAccessTime(NormalizePath(filePath));
	}

	time_t File::GetLastWriteTime(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetLastWriteTime(NormalizePath(filePath));
	}

	UInt64 File::GetSize(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetSize(NormalizePath(filePath));
	}

	bool File::IsAbsolute(const String& filePath)
	{
		String path(filePath.Trimmed());
		if (path.IsEmpty())
			return false;

		path = NormalizeSeparators(path);

		#ifdef NAZARA_PLATFORM_WINDOWS
		if (path.Match("?:*")) // Ex: C:\Hello
			return true;
		else if (path.Match("\\\\*")) // Ex: \\Laptop
			return true;
		else if (path.StartsWith('\\')) // Spécial : '\' fait référence au disque racine
			return true;
		else
			return false;
		#elif defined(NAZARA_PLATFORM_POSIX)
		return path.StartsWith('/');
		#else
			#error OS case not implemented
		#endif
	}

	String File::NormalizePath(const String& filePath)
	{
		String path = NormalizeSeparators(filePath.Trimmed());

		if (!IsAbsolute(path))
			path = Directory::GetCurrent() + NAZARA_DIRECTORY_SEPARATOR + path;

		while (path.EndsWith(NAZARA_DIRECTORY_SEPARATOR))
			path.Resize(-1);

		return path;
	}

	String File::NormalizeSeparators(const String& filePath)
	{
		String path(filePath);

		#if defined(NAZARA_PLATFORM_WINDOWS)
		path.Replace('/', '\\');
		#elif defined(NAZARA_PLATFORM_LINUX)
		path.Replace('\\', '/');
		#else
			#error OS case not implemented
		#endif

		return path;
	}

	bool File::Rename(const String& sourcePath, const String& targetPath)
	{
		if (sourcePath.IsEmpty() || targetPath.IsEmpty())
			return false;

		return FileImpl::Rename(NormalizePath(sourcePath), NormalizePath(targetPath));
	}

	NAZARA_CORE_API bool HashAppend(AbstractHash* hash, const File& originalFile)
	{
		File file(originalFile.GetPath());
		if (!file.Open(OpenMode_ReadOnly))
		{
			NazaraError("Unable to open file");
			return false;
		}

		UInt64 remainingSize = file.GetSize();

		char buffer[NAZARA_CORE_FILE_BUFFERSIZE];
		while (remainingSize > 0)
		{
			unsigned int size = static_cast<unsigned int>(std::min(remainingSize, static_cast<UInt64>(NAZARA_CORE_FILE_BUFFERSIZE)));
			if (file.Read(&buffer[0], sizeof(char), size) != sizeof(char)*size)
			{
				NazaraError("Unable to read file");
				return false;
			}

			remainingSize -= size;
			hash->Append(reinterpret_cast<UInt8*>(&buffer[0]), size);
		}

		return true;
	};
}
