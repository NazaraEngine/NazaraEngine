// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/FileImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
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

	FileImpl::~FileImpl()
	{
		if (m_handle)
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

	bool FileImpl::Open(const std::filesystem::path& filePath, OpenModeFlags mode)
	{
		DWORD access = 0;
		DWORD shareMode = FILE_SHARE_READ;
		DWORD openMode = 0;

		if (mode & OpenMode::ReadOnly)
		{
			access |= GENERIC_READ;

			if (mode & OpenMode::MustExist || (mode & OpenMode::WriteOnly) == 0)
				openMode |= OPEN_EXISTING;
		}

		if (mode & OpenMode::WriteOnly)
		{
			if (mode & OpenMode::Append)
				access |= FILE_APPEND_DATA;
			else
				access |= GENERIC_WRITE;

			if (mode & OpenMode::Truncate)
				openMode |= CREATE_ALWAYS;
			else if (mode & OpenMode::MustExist)
				openMode |= OPEN_EXISTING;
			else
				openMode |= OPEN_ALWAYS;
		}

		if ((mode & OpenMode::Lock) == 0)
			shareMode |= FILE_SHARE_WRITE;

		m_handle = CreateFileW(ToWideString(filePath.generic_u8string()).data(), access, shareMode, nullptr, openMode, 0, nullptr);
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
			case CursorPosition::AtBegin:
				moveMethod = FILE_BEGIN;
				break;

			case CursorPosition::AtCurrent:
				moveMethod = FILE_CURRENT;
				break;

			case CursorPosition::AtEnd:
				moveMethod = FILE_END;
				break;

			default:
				NazaraInternalError("Cursor position not handled (0x" + NumberToString(UnderlyingCast(pos), 16) + ')');
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
			if (!SetCursorPos(CursorPosition::AtBegin, cursorPos))
				NazaraWarning("Failed to reset cursor position to previous position: " + Error::GetLastSystemError());
		});

		if (!SetCursorPos(CursorPosition::AtBegin, size))
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

		WriteFile(m_handle, buffer, static_cast<DWORD>(size), &written, nullptr);

		m_endOfFileUpdated = false;

		return written;
	}
}
