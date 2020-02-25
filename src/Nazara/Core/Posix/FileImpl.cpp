// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/FileImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	FileImpl::FileImpl(const File* parent) :
	m_fileDescriptor(-1),
	m_endOfFile(false),
	m_endOfFileUpdated(true)
	{
		NazaraUnused(parent);
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
			NazaraError("Unable to flush file: " + Error::GetLastSystemError());
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

		if ((mode & OpenMode_ReadWrite) == OpenMode_ReadWrite)
			flags = O_CREAT | O_RDWR;
		else if ((mode & OpenMode_ReadOnly) == OpenMode_ReadOnly)
			flags = O_RDONLY;
		else if ((mode & OpenMode_WriteOnly) == OpenMode_WriteOnly)
			flags = O_CREAT | O_WRONLY;
		else
			return false;

		if (mode & OpenMode_Append)
			flags |= O_APPEND;

		if (mode & OpenMode_MustExist)
			flags &= ~O_CREAT;

		if (mode & OpenMode_Truncate)
			flags |= O_TRUNC;

		int fileDescriptor = open64(filePath.generic_u8string().data(), flags, permissions);
		if (fileDescriptor == -1)
		{
			NazaraError("Failed to open \"" + filePath.generic_u8string() + "\" : " + Error::GetLastSystemError());
			return false;
		}

		static struct flock lock;

		auto initialize_flock = [](struct flock& fileLock)
		{
			fileLock.l_type = F_WRLCK;
			fileLock.l_start = 0;
			fileLock.l_whence = SEEK_SET;
			fileLock.l_len = 0;
			fileLock.l_pid = getpid();
		};

		initialize_flock(lock);

		if (fcntl(fileDescriptor, F_GETLK, &lock) == -1)
		{
			close(fileDescriptor);
			NazaraError("Unable to detect presence of lock on the file");
			return false;
		}

		if (lock.l_type != F_UNLCK)
		{
			close(fileDescriptor);
			NazaraError("A lock is present on the file");
			return false;
		}

		if (mode & OpenMode_Lock)
		{
			initialize_flock(lock);

			if (fcntl(fileDescriptor, F_SETLK, &lock) == -1)
			{
				close(fileDescriptor);
				NazaraError("Unable to place a lock on the file");
				return false;
			}
		}

		m_fileDescriptor = fileDescriptor;

		return true;
	}

	std::size_t FileImpl::Read(void* buffer, std::size_t size)
	{
		ssize_t bytes;
		if ((bytes = read(m_fileDescriptor, buffer, size)) != -1)
		{
			m_endOfFile = (static_cast<std::size_t>(bytes) != size);
			m_endOfFileUpdated = true;

			return static_cast<std::size_t>(bytes);
		}
		else
			return 0;
	}

	bool FileImpl::SetCursorPos(CursorPosition pos, Int64 offset)
	{
		int moveMethod;
		switch (pos)
		{
			case CursorPosition_AtBegin:
				moveMethod = SEEK_SET;
				break;

			case CursorPosition_AtCurrent:
				moveMethod = SEEK_CUR;
				break;

			case CursorPosition_AtEnd:
				moveMethod = SEEK_END;
				break;

			default:
				NazaraInternalError("Cursor position not handled (0x" + String::Number(pos, 16) + ')');
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
		ssize_t written = write(m_fileDescriptor, buffer, size);
		m_endOfFileUpdated = false;

		return written;
	}
}
