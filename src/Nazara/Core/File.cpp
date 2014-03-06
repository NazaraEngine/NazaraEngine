// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/File.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Hash.hpp>
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

NzFile::NzFile() :
m_endianness(nzEndianness_Unknown),
m_impl(nullptr),
m_openMode(0)
{
}

NzFile::NzFile(const NzString& filePath) :
m_endianness(nzEndianness_Unknown),
m_impl(nullptr),
m_openMode(0)
{
	SetFile(filePath);
}

NzFile::NzFile(const NzString& filePath, unsigned long openMode) :
m_endianness(nzEndianness_Unknown),
m_impl(nullptr),
m_openMode(0)
{
	SetFile(filePath);
	Open(openMode);
}

NzFile::NzFile(NzFile&& file) noexcept :
m_endianness(file.m_endianness),
m_filePath(std::move(file.m_filePath)),
m_impl(file.m_impl),
m_openMode(file.m_openMode)
{
	file.m_impl = nullptr;
}

NzFile::~NzFile()
{
	Close();
}

bool NzFile::Copy(const NzString& newFilePath)
{
	return Copy(m_filePath, newFilePath);
}

void NzFile::Close()
{
	NazaraLock(m_mutex)

	if (m_impl)
	{
		m_impl->Close();
		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzFile::Delete()
{
	NazaraLock(m_mutex)

	Close();

	return Delete(m_filePath);
}

bool NzFile::EndOfFile() const
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

bool NzFile::EndOfStream() const
{
	return EndOfFile();
}

bool NzFile::Exists() const
{
	NazaraLock(m_mutex)

	if (IsOpen())
		return true; // Le fichier est ouvert, donc il existe
	else
		return Exists(m_filePath);
}

void NzFile::Flush()
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!IsOpen())
	{
		NazaraError("File not opened");
		return;
	}

	if ((m_openMode & ReadWrite) == 0 && (m_openMode & WriteOnly) == 0)
	{
		NazaraError("Cannot flush file without write access");
		return;
	}
	#endif

	m_impl->Flush();
}

time_t NzFile::GetCreationTime() const
{
	NazaraLock(m_mutex)

	return GetCreationTime(m_filePath);
}

nzUInt64 NzFile::GetCursorPos() const
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!IsOpen())
	{
		NazaraError("File not opened");
		return false;
	}
	#endif

	return m_impl->GetCursorPos();
}

NzString NzFile::GetDirectory() const
{
	NazaraLock(m_mutex)

	return m_filePath.SubStringTo(NAZARA_DIRECTORY_SEPARATOR, -1, true, true);
}

NzString NzFile::GetFileName() const
{
	NazaraLock(m_mutex)

	return m_filePath.SubStringFrom(NAZARA_DIRECTORY_SEPARATOR, -1, true);
}

time_t NzFile::GetLastAccessTime() const
{
	NazaraLock(m_mutex)

	return GetLastAccessTime(m_filePath);
}

time_t NzFile::GetLastWriteTime() const
{
	NazaraLock(m_mutex)

	return GetLastWriteTime(m_filePath);
}

NzString NzFile::GetPath() const
{
	NazaraLock(m_mutex)

	return m_filePath;
}

nzUInt64 NzFile::GetSize() const
{
	NazaraLock(m_mutex)

	return GetSize(m_filePath);
}

bool NzFile::IsOpen() const
{
	NazaraLock(m_mutex)

	return m_impl != nullptr;
}

std::size_t NzFile::Read(void* buffer, std::size_t size)
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!IsOpen())
	{
		NazaraError("File not opened");
		return 0;
	}

	if ((m_openMode & ReadOnly) == 0 && (m_openMode & ReadWrite) == 0)
	{
		NazaraError("File not opened with read access");
		return 0;
	}
	#endif

	if (size == 0)
		return 0;

	if (buffer)
		return m_impl->Read(buffer, size);
	else
	{
		// Si nous ne devons rien lire, nous avançons simplement
		nzUInt64 currentPos = m_impl->GetCursorPos();

		m_impl->SetCursorPos(NzFile::AtCurrent, size);

		return static_cast<std::size_t>(m_impl->GetCursorPos()-currentPos);
	}
}

std::size_t NzFile::Read(void* buffer, std::size_t typeSize, unsigned int count)
{
	std::size_t byteRead = Read(buffer, typeSize*count);
	if (byteRead == 0)
		return 0;

	if (buffer && typeSize != 1 && m_endianness != nzEndianness_Unknown && m_endianness != NzGetPlatformEndianness())
	{
		unsigned int typeCount = byteRead/typeSize;
		for (unsigned int i = 0; i < typeCount; ++i)
			NzByteSwap(reinterpret_cast<nzUInt8*>(buffer) + i*typeSize, typeSize);
	}

	return byteRead;
}

bool NzFile::Rename(const NzString& newFilePath)
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

bool NzFile::Open(unsigned long openMode)
{
	NazaraLock(m_mutex)

	Close();

	if (m_filePath.IsEmpty())
		return false;

	if (openMode != 0)
		m_openMode = openMode;

	if (m_openMode == 0)
		return false;

	std::unique_ptr<NzFileImpl> impl(new NzFileImpl(this));
	if (!impl->Open(m_filePath, m_openMode))
	{
		NzErrorFlags flags(nzErrorFlag_Silent); // Silencieux par défaut
		NazaraError("Failed to open \"" + m_filePath + "\": " + NzError::GetLastSystemError());
		return false;
	}

	m_impl = impl.release();

	if (m_openMode & Text)
		m_streamOptions |= nzStreamOption_Text;

	return true;
}

bool NzFile::SetCursorPos(CursorPosition pos, nzInt64 offset)
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!IsOpen())
	{
		NazaraError("File not opened");
		return false;
	}
	#endif

	return m_impl->SetCursorPos(pos, offset);
}

bool NzFile::SetCursorPos(nzUInt64 offset)
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!IsOpen())
	{
		NazaraError("File not opened");
		return false;
	}
	#endif

	return m_impl->SetCursorPos(AtBegin, offset);
}

void NzFile::SetEndianness(nzEndianness endianness)
{
	NazaraLock(m_mutex)

	m_endianness = endianness;
}

bool NzFile::SetFile(const NzString& filePath)
{
	NazaraLock(m_mutex)

	if (IsOpen())
	{
		if (filePath.IsEmpty())
			return false;

		std::unique_ptr<NzFileImpl> impl(new NzFileImpl(this));
		if (!impl->Open(filePath, m_openMode))
		{
			NazaraError("Failed to open new file; " + NzError::GetLastSystemError());
			return false;
		}

		m_impl->Close();
		delete m_impl;

		m_impl = impl.release();
	}

	m_filePath = AbsolutePath(filePath);
	return true;
}

bool NzFile::SetOpenMode(unsigned int openMode)
{
	NazaraLock(m_mutex)

	if (openMode == 0 || openMode == m_openMode)
		return true;

	if (IsOpen())
	{
		std::unique_ptr<NzFileImpl> impl(new NzFileImpl(this));
		if (!impl->Open(m_filePath, openMode))
		{
			NazaraError("Failed to open file with new mode: " + NzError::GetLastSystemError());
			return false;
		}

		m_impl->Close();
		delete m_impl;

		m_impl = impl.release();

		if (m_openMode & Text)
			m_streamOptions |= nzStreamOption_Text;
	}

	m_openMode = openMode;

	return true;
}

bool NzFile::Write(const NzByteArray& byteArray)
{
	unsigned int size = byteArray.GetSize();
	return Write(byteArray.GetConstBuffer(), 1, size) == size;
}

bool NzFile::Write(const NzString& string)
{
	NzString temp(string);

	if (m_streamOptions & nzStreamOption_Text)
	{
		#if defined(NAZARA_PLATFORM_WINDOWS)
		temp.Replace("\n", "\r\n");
		#elif defined(NAZARA_PLATFORM_LINUX)
		// Rien à faire
		#elif defined(NAZARA_PLATFORM_MACOS)
		temp.Replace('\n', '\r');
		#else
			#error OS not handled
		#endif
	}

	unsigned int size = temp.GetSize();
	std::size_t bytesWritten = Write(temp.GetBuffer(), sizeof(char), size);

	return bytesWritten == size*sizeof(char);
}

std::size_t NzFile::Write(const void* buffer, std::size_t typeSize, unsigned int count)
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!IsOpen())
	{
		NazaraError("File not opened");
		return 0;
	}

	if ((m_openMode & ReadWrite) == 0 && (m_openMode & WriteOnly) == 0)
	{
		NazaraError("File not opened with write access");
		return 0;
	}
	#endif

	if (!buffer || count == 0 || typeSize == 0)
		return 0;

	std::size_t bytesWritten;
	if (m_endianness != nzEndianness_Unknown && m_endianness != NzGetPlatformEndianness() && typeSize != 1)
	{
		std::unique_ptr<char[]> buf(new char[count*typeSize]);
		std::memcpy(buf.get(), buffer, count*typeSize);

		for (unsigned int i = 0; i < count; ++i)
			NzByteSwap(&buf[i*typeSize], typeSize);

		bytesWritten = m_impl->Write(buf.get(), count*typeSize);
	}
	else
		bytesWritten = m_impl->Write(buffer, count*typeSize);

	return bytesWritten;
}

NzFile& NzFile::operator=(const NzString& filePath)
{
	SetFile(filePath);

	return *this;
}

NzFile& NzFile::operator=(NzFile&& file) noexcept
{
	NazaraLock(m_mutex)

	std::swap(m_endianness, file.m_endianness);
	std::swap(m_filePath, file.m_filePath);
	std::swap(m_impl, file.m_impl);
	std::swap(m_openMode, file.m_openMode);

	return *this;
}

NzString NzFile::AbsolutePath(const NzString& filePath)
{
	// Je n'utilise pas les fonctions de l'OS car elles ne fonctionnent que pour un chemin existant
	NzString path = NormalizePath(filePath);
	if (path.IsEmpty())
		return NzString();

	NzString base;
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
		NzString drive = NzDirectory::GetCurrent().SubStringTo('\\');
		NzString end = path.SubString(1, -1);
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

	static NzString upDir = NAZARA_DIRECTORY_SEPARATOR + NzString('.');

	if (path.Find(upDir) == NzString::npos)
		return path;

	std::vector<NzString> sep;
	if (path.Split(sep, NAZARA_DIRECTORY_SEPARATOR) <= 1)
		return path;

	// Nous avons un chemin absolu, mais il nous faut un peu le nettoyer
	unsigned int pathLen = base.GetSize();
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
		else
			pathLen += sep[i].GetSize();
	}

	pathLen += sep.size()-1;

	NzStringStream stream(base);
	for (unsigned int i = 0; i < sep.size(); ++i)
	{
		if (i != sep.size()-1)
			stream << sep[i] << NAZARA_DIRECTORY_SEPARATOR;
		else
			stream << sep[i];
	}

	return stream;
}

bool NzFile::Copy(const NzString& sourcePath, const NzString& targetPath)
{
	if (sourcePath.IsEmpty() || targetPath.IsEmpty())
		return false;

	return NzFileImpl::Copy(NormalizePath(sourcePath), NormalizePath(targetPath));
}

bool NzFile::Delete(const NzString& filePath)
{
	if (filePath.IsEmpty())
		return false;

	return NzFileImpl::Delete(NormalizePath(filePath));
}

bool NzFile::Exists(const NzString& filePath)
{
	if (filePath.IsEmpty())
		return false;

	return NzFileImpl::Exists(NormalizePath(filePath));
}

time_t NzFile::GetCreationTime(const NzString& filePath)
{
	if (filePath.IsEmpty())
		return 0;

	return NzFileImpl::GetCreationTime(NormalizePath(filePath));
}

NzString NzFile::GetDirectory(const NzString& filePath)
{
	return filePath.SubStringTo(NAZARA_DIRECTORY_SEPARATOR, -1, true, true);
}

time_t NzFile::GetLastAccessTime(const NzString& filePath)
{
	if (filePath.IsEmpty())
		return 0;

	return NzFileImpl::GetLastAccessTime(NormalizePath(filePath));
}

time_t NzFile::GetLastWriteTime(const NzString& filePath)
{
	if (filePath.IsEmpty())
		return 0;

	return NzFileImpl::GetLastWriteTime(NormalizePath(filePath));
}

NzHashDigest NzFile::GetHash(const NzString& filePath, nzHash hash)
{
	NzFile file(filePath);

	NzHash h(hash);
	return h.Hash(file);
}

NzHashDigest NzFile::GetHash(const NzString& filePath, NzAbstractHash* hash)
{
	NzFile file(filePath);

	NzHash h(hash);
	return h.Hash(file);
}

nzUInt64 NzFile::GetSize(const NzString& filePath)
{
	if (filePath.IsEmpty())
		return 0;

	return NzFileImpl::GetSize(NormalizePath(filePath));
}

bool NzFile::IsAbsolute(const NzString& filePath)
{
	NzString path(filePath.Trimmed());
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

NzString NzFile::NormalizePath(const NzString& filePath)
{
	NzString path = NormalizeSeparators(filePath.Trimmed());

	if (!IsAbsolute(path))
		path = NzDirectory::GetCurrent() + NAZARA_DIRECTORY_SEPARATOR + path;

	while (path.EndsWith(NAZARA_DIRECTORY_SEPARATOR))
		path.Resize(-1);

	return path;
}

NzString NzFile::NormalizeSeparators(const NzString& filePath)
{
	NzString path(filePath);

	#if defined(NAZARA_PLATFORM_WINDOWS)
	path.Replace('/', '\\');
	#elif defined(NAZARA_PLATFORM_LINUX)
	path.Replace('\\', '/');
	#else
		#error OS case not implemented
	#endif

	return path;
}

bool NzFile::Rename(const NzString& sourcePath, const NzString& targetPath)
{
	if (sourcePath.IsEmpty() || targetPath.IsEmpty())
		return false;

	return NzFileImpl::Rename(NormalizePath(sourcePath), NormalizePath(targetPath));
}

bool NzFile::FillHash(NzAbstractHash* hash) const
{
	NzFile file(m_filePath);
	if (!file.Open(NzFile::ReadOnly))
	{
		NazaraError("Unable to open file");
		return false;
	}

	nzUInt64 remainingSize = file.GetSize();

	char buffer[NAZARA_CORE_FILE_BUFFERSIZE];
	unsigned int size;
	while (remainingSize > 0)
	{
		size = (remainingSize >= NAZARA_CORE_FILE_BUFFERSIZE) ? NAZARA_CORE_FILE_BUFFERSIZE : static_cast<unsigned int>(remainingSize);
		if (file.Read(&buffer[0], sizeof(char), size) != sizeof(char)*size)
		{
			NazaraError("Unable to read file");
			return false;
		}

		remainingSize -= size;
		hash->Append(reinterpret_cast<nzUInt8*>(&buffer[0]), size);
	}

	return true;
} // Fermeture automatique du fichier
