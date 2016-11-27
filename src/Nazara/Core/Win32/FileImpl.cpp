// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/FileImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Win32/Time.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	FileImpl::FileImpl(const File* parent) :
	m_endOfFile(false),
	m_endOfFileUpdated(true)
	{
		NazaraUnused(parent);
	}

	void FileImpl::Close()
	{
		CloseHandle(m_handle);
	}

	bool FileImpl::EndOfFile() const
	{
		if (!m_endOfFileUpdated)
		{
			LARGE_INTEGER fileSize;
			if (!GetFileSizeEx(m_handle, &fileSize))
				fileSize.QuadPart = 0;

			m_endOfFile = (GetCursorPos() >= static_cast<UInt64>(fileSize.QuadPart));
			m_endOfFileUpdated = true;
		}

		return m_endOfFile;
	}

	void FileImpl::Flush()
	{
		if (!FlushFileBuffers(m_handle))
			NazaraError("Unable to flush file: " + Error::GetLastSystemError());
	}

	UInt64 FileImpl::GetCursorPos() const
	{
		LARGE_INTEGER zero;
		zero.QuadPart = 0;

		LARGE_INTEGER position;
		SetFilePointerEx(m_handle, zero, &position, FILE_CURRENT);

		return position.QuadPart;
	}

	bool FileImpl::Open(const String& filePath, OpenModeFlags mode)
	{
		DWORD access = 0;
		DWORD shareMode = FILE_SHARE_READ;
		DWORD openMode = 0;

		if (mode & OpenMode_ReadOnly)
		{
			access |= GENERIC_READ;

			if (mode & OpenMode_MustExit || (mode & OpenMode_WriteOnly) == 0)
				openMode |= OPEN_EXISTING;
		}

		if (mode & OpenMode_WriteOnly)
		{
			if (mode & OpenMode_Append)
				access |= FILE_APPEND_DATA;
			else
				access |= GENERIC_WRITE;

			if (mode & OpenMode_Truncate)
				openMode |= CREATE_ALWAYS;
			else if (mode & OpenMode_MustExit)
				openMode |= OPEN_EXISTING;
			else
				openMode |= OPEN_ALWAYS;
		}

		if ((mode & OpenMode_Lock) == 0)
			shareMode |= FILE_SHARE_WRITE;

		m_handle = CreateFileW(filePath.GetWideString().data(), access, shareMode, nullptr, openMode, 0, nullptr);
		return m_handle != INVALID_HANDLE_VALUE;
	}

	std::size_t FileImpl::Read(void* buffer, std::size_t size)
	{
		//UInt64 oldCursorPos = GetCursorPos();

		DWORD read = 0;
		if (ReadFile(m_handle, buffer, static_cast<DWORD>(size), &read, nullptr))
		{
			m_endOfFile = (read != size);
			m_endOfFileUpdated = true;

			return read;
			///FIXME: D'après la documentation, read vaut 0 si ReadFile atteint la fin du fichier
			///       D'après les tests, ce n'est pas le cas, la taille lue est inférieure à la taille en argument, mais pas nulle
			///       Peut-être ais-je mal compris la documentation
			///       Le correctif (dans le cas où la doc serait vraie) est commenté en début de fonction et après ce commentaire
			///       Il est cependant plus lourd, et ne fonctionne pas avec le comportement observé de la fonction
			/*
			if (read == 0)
			{
				// Si nous atteignons la fin du fichier, la taille lue vaut 0
				// pour renvoyer le nombre d'octets lus nous comparons la position du curseur
				// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365690(v=vs.85).aspx
				m_endOfFile = true;
				m_endOfFileUpdated = true;

				return GetCursorPos()-oldCursorPos;
			}
			else
			{
				m_endOfFileUpdated = false;
				return read;
			}
			*/
		}
		else
			return 0;
	}

	bool FileImpl::SetCursorPos(CursorPosition pos, Int64 offset)
	{
		DWORD moveMethod;
		switch (pos)
		{
			case CursorPosition_AtBegin:
				moveMethod = FILE_BEGIN;
				break;

			case CursorPosition_AtCurrent:
				moveMethod = FILE_CURRENT;
				break;

			case CursorPosition_AtEnd:
				moveMethod = FILE_END;
				break;

			default:
				NazaraInternalError("Cursor position not handled (0x" + String::Number(pos, 16) + ')');
				return false;
		}

		LARGE_INTEGER distance;
		distance.QuadPart = offset;

		m_endOfFileUpdated = false;

		return SetFilePointerEx(m_handle, distance, nullptr, moveMethod) != 0;
	}

	bool FileImpl::SetSize(UInt64 size)
	{
		UInt64 cursorPos = GetCursorPos();

		CallOnExit resetCursor([this, cursorPos] ()
		{
			if (!SetCursorPos(CursorPosition_AtBegin, cursorPos))
				NazaraWarning("Failed to reset cursor position to previous position: " + Error::GetLastSystemError());
		});

		if (!SetCursorPos(CursorPosition_AtBegin, size))
		{
			NazaraError("Failed to set file size: failed to move cursor position: " + Error::GetLastSystemError());
			return false;
		}

		if (!SetEndOfFile(m_handle))
		{
			NazaraError("Failed to set file size: " + Error::GetLastSystemError());
			return false;
		}

		return true;
	}

	std::size_t FileImpl::Write(const void* buffer, std::size_t size)
	{
		DWORD written = 0;

		LARGE_INTEGER cursorPos;
		cursorPos.QuadPart = GetCursorPos();

		LockFile(m_handle, cursorPos.LowPart, cursorPos.HighPart, static_cast<DWORD>(size), 0);
		WriteFile(m_handle, buffer, static_cast<DWORD>(size), &written, nullptr);
		UnlockFile(m_handle, cursorPos.LowPart, cursorPos.HighPart, static_cast<DWORD>(size), 0);

		m_endOfFileUpdated = false;

		return written;
	}

	bool FileImpl::Copy(const String& sourcePath, const String& targetPath)
	{
		if (CopyFileW(sourcePath.GetWideString().data(), targetPath.GetWideString().data(), false))
			return true;
		else
		{
			NazaraError("Failed to copy file: " + Error::GetLastSystemError());
			return false;
		}
	}

	bool FileImpl::Delete(const String& filePath)
	{
		if (DeleteFileW(filePath.GetWideString().data()))
			return true;
		else
		{
			NazaraError("Failed to delete file (" + filePath + "): " + Error::GetLastSystemError());
			return false;
		}
	}

	bool FileImpl::Exists(const String& filePath)
	{
		HANDLE handle = CreateFileW(filePath.GetWideString().data(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return false;

		CloseHandle(handle);
		return true;
	}

	time_t FileImpl::GetCreationTime(const String& filePath)
	{
		HANDLE handle = CreateFileW(filePath.GetWideString().data(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return 0;

		FILETIME creationTime;
		if (!GetFileTime(handle, &creationTime, nullptr, nullptr))
		{
			CloseHandle(handle);

			NazaraError("Unable to get creation time: " + Error::GetLastSystemError());
			return 0;
		}

		CloseHandle(handle);
		return FileTimeToTime(&creationTime);
	}

	time_t FileImpl::GetLastAccessTime(const String& filePath)
	{
		HANDLE handle = CreateFileW(filePath.GetWideString().data(), 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return 0;

		FILETIME accessTime;
		if (!GetFileTime(handle, nullptr, &accessTime, nullptr))
		{
			CloseHandle(handle);

			NazaraError("Unable to get last access time: " + Error::GetLastSystemError());
			return 0;
		}

		CloseHandle(handle);
		return FileTimeToTime(&accessTime);
	}

	time_t FileImpl::GetLastWriteTime(const String& filePath)
	{
		HANDLE handle = CreateFileW(filePath.GetWideString().data(), 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return 0;

		FILETIME writeTime;
		if (!GetFileTime(handle, nullptr, nullptr, &writeTime))
		{
			CloseHandle(handle);

			NazaraError("Unable to get last write time: " + Error::GetLastSystemError());
			return 0;
		}

		CloseHandle(handle);
		return FileTimeToTime(&writeTime);
	}

	UInt64 FileImpl::GetSize(const String& filePath)
	{
		HANDLE handle = CreateFileW(filePath.GetWideString().data(), 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return 0;

		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(handle, &fileSize))
			fileSize.QuadPart = 0;

		CloseHandle(handle);
		return fileSize.QuadPart;
	}

	bool FileImpl::Rename(const String& sourcePath, const String& targetPath)
	{
		if (MoveFileExW(sourcePath.GetWideString().data(), targetPath.GetWideString().data(), MOVEFILE_COPY_ALLOWED))
			return true;
		else
		{
			NazaraError("Unable to rename file: " + Error::GetLastSystemError());
			return false;
		}
	}
}
