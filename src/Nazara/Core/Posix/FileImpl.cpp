// Copyright (C) 2024 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Posix/FileImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Posix/PosixUtils.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef NAZARA_PLATFORM_BSD
	#define fstat64 fstat
	#define ftruncate64 ftruncate
	#define off64_t off_t
	#define open64 open
	#define lseek64 lseek
	#define stat64 stat
#endif

namespace Nz::PlatformImpl
{
	FileImpl::FileImpl(const File* /*parent*/) :
	m_fileDescriptor(-1),
	m_endOfFile(false),
	m_endOfFileUpdated(true)
	{
	}

	FileImpl::~FileImpl()
	{
		if (m_fileDescriptor != -1)
			close(m_fileDescriptor);
	}

	bool FileImpl::EndOfFile() const
	{
		if (!m_endOfFileUpdated)
		{
			struct stat64 fileSize;
			if (fstat64(m_fileDescriptor, &fileSize) == -1)
				fileSize.st_size = 0;

			m_endOfFile = (GetCursorPos() >= static_cast<UInt64>(fileSize.st_size));
			m_endOfFileUpdated = true;
		}

		return m_endOfFile;
	}

	void FileImpl::Flush()
	{
		if (fsync(m_fileDescriptor) == -1)
			NazaraError("unable to flush file: {0}", Error::GetLastSystemError());
	}

	UInt64 FileImpl::GetCursorPos() const
	{
		off64_t position = lseek64(m_fileDescriptor, 0, SEEK_CUR);
		return static_cast<UInt64>(position);
	}

	bool FileImpl::Open(const std::filesystem::path& filePath, OpenModeFlags mode)
	{
		int flags;
		mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

		if (mode.Test(OpenMode_ReadWrite))
			flags = O_CREAT | O_RDWR;
		else if (mode.Test(OpenMode::Read))
			flags = O_RDONLY;
		else if (mode.Test(OpenMode::Write))
			flags = O_CREAT | O_WRONLY;
		else
			return false;

		if (mode & OpenMode::Append)
			flags |= O_APPEND;

		if (mode & OpenMode::MustExist)
			flags &= ~O_CREAT;

		if (mode & OpenMode::Truncate)
			flags |= O_TRUNC;

		int fileDescriptor = open64(Nz::PathToString(filePath).data(), flags, permissions);
		if (fileDescriptor == -1)
			return false;

		if (mode & OpenMode::Lock)
		{
			CallOnExit closeOnError([&] { close(fileDescriptor); });

			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_start = 0;
			lock.l_whence = SEEK_SET;
			lock.l_len = 0;
			lock.l_pid = getpid();

			if (fcntl(fileDescriptor, F_GETLK, &lock) == -1)
			{
				NazaraError("unable to detect presence of lock on the file");
				return false;
			}

			if (lock.l_type != F_UNLCK)
			{
				NazaraError("a lock is present on the file");
				return false;
			}

			if (fcntl(fileDescriptor, F_SETLK, &lock) == -1)
			{
				NazaraError("unable to place a lock on the file");
				return false;
			}

			closeOnError.Reset();
		}

		m_fileDescriptor = fileDescriptor;
		return true;
	}

	std::size_t FileImpl::Read(void* buffer, std::size_t size)
	{
		ssize_t read = SafeRead(m_fileDescriptor, buffer, size);
		if (read < 0)
		{
			NazaraError("failed to read from file: {0}", Error::GetLastSystemError());
			return 0;
		}

		m_endOfFile = (static_cast<std::size_t>(read) != size);
		m_endOfFileUpdated = true;

		return static_cast<std::size_t>(read);
	}

	bool FileImpl::SetCursorPos(CursorPosition pos, Int64 offset)
	{
		int moveMethod;
		switch (pos)
		{
			case CursorPosition::AtBegin:
				moveMethod = SEEK_SET;
				break;

			case CursorPosition::AtCurrent:
				moveMethod = SEEK_CUR;
				break;

			case CursorPosition::AtEnd:
				moveMethod = SEEK_END;
				break;

			default:
				NazaraInternalError("cursor position not handled ({0:#x})", UnderlyingCast(pos));
				return false;
		}

		m_endOfFileUpdated = false;

		return lseek64(m_fileDescriptor, offset, moveMethod) != -1;
	}

	bool FileImpl::SetSize(UInt64 size)
	{
		return ftruncate64(m_fileDescriptor, size) != 0;
	}

	std::size_t FileImpl::Write(const void* buffer, std::size_t size)
	{
		ssize_t written = SafeWrite(m_fileDescriptor, buffer, size);
		if (written < 0)
		{
			NazaraError("failed to write to file: {0}", Error::GetLastSystemError());
			return 0;
		}

		m_endOfFileUpdated = false;

		return static_cast<std::size_t>(written);
	}
}
