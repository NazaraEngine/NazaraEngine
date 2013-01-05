// Copyright (C) 2012 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/FileImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

NzFileImpl::NzFileImpl(const NzFile* parent) :
m_endOfFile(false),
m_endOfFileUpdated(true)
{
	NazaraUnused(parent);
}

void NzFileImpl::Close()
{
	close(m_fileDescriptor);
}

bool NzFileImpl::EndOfFile() const
{
	if (!m_endOfFileUpdated)
	{
		struct stat64 fileSize;
		if (fstat64(m_fileDescriptor, &fileSize) == -1)
			fileSize.st_size = 0;

		m_endOfFile = (GetCursorPos() >= static_cast<nzUInt64>(fileSize.st_size));
		m_endOfFileUpdated = true;
	}

	return m_endOfFile;
}

void NzFileImpl::Flush()
{
	if (fsync(m_fileDescriptor) == -1)
		NazaraError("Unable to flush file: " + NzGetLastSystemError());
}

nzUInt64 NzFileImpl::GetCursorPos() const
{
	off64_t position = lseek64(m_fileDescriptor, 0, SEEK_CUR);
	return static_cast<nzUInt64>(position);
}

bool NzFileImpl::Open(const NzString& filePath, unsigned int mode)
{
	int flags;
	mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if (mode & NzFile::ReadOnly)
		flags = O_RDONLY;
	else if (mode & NzFile::ReadWrite)
	{
		flags = O_CREAT | O_RDWR;

		if (mode & NzFile::Append)
			flags |= O_APPEND;

		if (mode & NzFile::Truncate)
			flags |= O_TRUNC;
	}
	else if (mode & NzFile::WriteOnly)
	{
		flags = O_CREAT | O_WRONLY;

		if (mode & NzFile::Append)
			flags |= O_APPEND;

		if (mode & NzFile::Truncate)
			flags |= O_TRUNC;
	}
	else
		return false;

///TODO: lock
//	if ((mode & NzFile::Lock) == 0)
//		shareMode |= FILE_SHARE_WRITE;

	m_fileDescriptor = open64(filePath.GetConstBuffer(), flags, permissions);
	return m_fileDescriptor != -1;
}

std::size_t NzFileImpl::Read(void* buffer, std::size_t size)
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

bool NzFileImpl::SetCursorPos(NzFile::CursorPosition pos, nzInt64 offset)
{
	int moveMethod;
	switch (pos)
	{
		case NzFile::AtBegin:
			moveMethod = SEEK_SET;
			break;

		case NzFile::AtCurrent:
			moveMethod = SEEK_CUR;
			break;

		case NzFile::AtEnd:
			moveMethod = SEEK_END;
			break;

		default:
			NazaraInternalError("Cursor position not handled (0x" + NzString::Number(pos, 16) + ')');
			return false;
	}

	m_endOfFileUpdated = false;

	return lseek64(m_fileDescriptor, offset, moveMethod) != -1;
}

std::size_t NzFileImpl::Write(const void* buffer, std::size_t size)
{
	lockf64(m_fileDescriptor, F_LOCK, size);
	ssize_t written = write(m_fileDescriptor, buffer, size);
	lockf64(m_fileDescriptor, F_ULOCK, size);

	m_endOfFileUpdated = false;

	return written;
}

bool NzFileImpl::Copy(const NzString& sourcePath, const NzString& targetPath)
{
	int fd1 = open64(sourcePath.GetConstBuffer(), O_RDONLY);
	if (fd1 == -1)
	{
		NazaraError("Fail to open input file (" + sourcePath + "): " + NzGetLastSystemError());
		return false;
	}

	mode_t permissions; // TODO : get permission from first file
	int fd2 = open64(targetPath.GetConstBuffer(), O_WRONLY | O_TRUNC, permissions);
	if (fd2 == -1)
	{
		NazaraError("Fail to open output file (" + targetPath + "): " + NzGetLastSystemError()); // TODO: more info ?
		close(fd1);
		return false;
	}

	char buffer[512];
	ssize_t bytes;
	do
	{
		bytes = read(fd1,buffer,512);
		if (bytes == -1)
		{
			close(fd1);
			close(fd2);
			NazaraError("An error occured from copy : " + NzGetLastSystemError());
			return false;
		}
		write(fd2,buffer,bytes);
	}
	while (bytes == 512);

	close(fd1);
	close(fd2);
}

bool NzFileImpl::Delete(const NzString& filePath)
{
	bool success = unlink(filePath.GetConstBuffer()) != -1;

	if (success)
		return true;
	else
	{
		NazaraError("Failed to delete file (" + filePath + "): " + NzGetLastSystemError());
		return false;
	}
}

bool NzFileImpl::Exists(const NzString& filePath)
{
	const char* path = filePath.GetConstBuffer();
	if (access(path, F_OK) != -1)
		return true;

	return false;
}

time_t NzFileImpl::GetCreationTime(const NzString& filePath)
{
	NazaraWarning("Posix has no creation time information");

	return 0;
}

time_t NzFileImpl::GetLastAccessTime(const NzString& filePath)
{
	struct stat64 stats;
	stat64(filePath.GetConstBuffer(), &stats);

	return stats.st_atime;
}

time_t NzFileImpl::GetLastWriteTime(const NzString& filePath)
{
	struct stat64 stats;
	stat64(filePath.GetConstBuffer(), &stats);

	return stats.st_mtime;
}

nzUInt64 NzFileImpl::GetSize(const NzString& filePath)
{
	struct stat64 stats;
	stat64(filePath.GetConstBuffer(), &stats);

	return static_cast<nzUInt64>(stats.st_size);
}

bool NzFileImpl::Rename(const NzString& sourcePath, const NzString& targetPath)
{
	bool success = std::rename(sourcePath.GetConstBuffer(), targetPath.GetConstBuffer()) != -1;

	if (success)
		return true;
	else
	{
		NazaraError("Unable to rename file: " + NzGetLastSystemError());
		return false;
	}
}
