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
	/*!
	* \ingroup core
	* \class Nz::File
	* \brief Core class that represents a file
	*/

	/*!
	* \brief Constructs a File object by default
	*/

	File::File() :
	m_impl(nullptr)
	{
	}

	/*!
	* \brief Constructs a File object with a file path
	*
	* \param filePath Path to the file
	*/

	File::File(const String& filePath) :
	File()
	{
		SetFile(filePath);
	}

	/*!
	* \brief Constructs a File object with a file path and flags
	*
	* \param filePath Path to the file
	* \param openMode Flag of the file
	*/

	File::File(const String& filePath, UInt32 openMode) :
	File()
	{
		Open(filePath, openMode);
	}

	/*!
	* \brief Constructs a File object by move semantic
	*
	* \param file File to move into this
	*/

	File::File(File&& file) noexcept :
	Stream(std::move(file)),
	m_filePath(std::move(file.m_filePath)),
	m_impl(file.m_impl)
	{
		file.m_impl = nullptr;
	}

	/*!
	* \brief Destructs the object and calls Close
	*
	* \see Close
	*/

	File::~File()
	{
		Close();
	}

	/*!
	* \brief Copies this file to a new file path
	* \return true if copy is successful
	*
	* \param newFilePath Path of the new file
	*/

	bool File::Copy(const String& newFilePath)
	{
		return Copy(m_filePath, newFilePath);
	}

	/*!
	* \brief Closes the file
	*/

	void File::Close()
	{
		NazaraLock(m_mutex)

		if (m_impl)
		{
			m_impl->Close();
			delete m_impl;
			m_impl = nullptr;

			m_openMode = OpenMode_NotOpen;
		}
	}

	/*!
	* \brief Deletes the file
	* \return true if delete is successful
	*/

	bool File::Delete()
	{
		NazaraLock(m_mutex)

		Close();

		return Delete(m_filePath);
	}

	/*!
	* \brief Checks whether the file has reached the end
	* \return true if cursor is at the end of the file
	*
	* \remark Produces a NazaraError if file is not open with NAZARA_CORE_SAFE defined
	*/

	bool File::EndOfFile() const
	{
		NazaraLock(m_mutex)

		#if NAZARA_CORE_SAFE
		if (!IsOpen())
		{
			NazaraError("File not open");
			return false;
		}
		#endif

		return m_impl->EndOfFile();
	}

	/*!
	* \brief Checks whether the file has reached the end of the stream
	* \return true if cursor is at the end of the file
	*
	* \remark Produces a NazaraError if file is not open with NAZARA_CORE_SAFE defined
	*
	* \see EndOfFile
	*/

	bool File::EndOfStream() const
	{
		return EndOfFile();
	}

	/*!
	* \brief Checks whether the file exists
	* \return true if file exists
	*/

	bool File::Exists() const
	{
		NazaraLock(m_mutex)

		if (IsOpen())
			return true; // Le fichier est ouvert, donc il existe
		else
			return Exists(m_filePath);
	}

	/*!
	* \brief Gets the creation time of the file
	* \return Information about the creation time
	*/

	time_t File::GetCreationTime() const
	{
		NazaraLock(m_mutex)

		return GetCreationTime(m_filePath);
	}

	/*!
	* \brief Gets the position of the cursor in the file
	* \return Position of the cursor
	*
	* \remark Produces a NazaraAssert if file is not open
	*/

	UInt64 File::GetCursorPos() const
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");

		return m_impl->GetCursorPos();
	}

	/*!
	* \brief Gets the directory of the file
	* \return Directory of the file
	*/

	String File::GetDirectory() const
	{
		NazaraLock(m_mutex)

		return m_filePath.SubStringTo(NAZARA_DIRECTORY_SEPARATOR, -1, true, true);
	}

	/*!
	* \brief Gets the name of the file
	* \return Name of the file
	*/

	String File::GetFileName() const
	{
		NazaraLock(m_mutex)

		return m_filePath.SubStringFrom(NAZARA_DIRECTORY_SEPARATOR, -1, true);
	}

	/*!
	* \brief Gets the last time the file was accessed
	* \return Information about the last access time
	*/

	time_t File::GetLastAccessTime() const
	{
		NazaraLock(m_mutex)

		return GetLastAccessTime(m_filePath);
	}

	/*!
	* \brief Gets the last time the file was written
	* \return Information about the last writing time
	*/

	time_t File::GetLastWriteTime() const
	{
		NazaraLock(m_mutex)

		return GetLastWriteTime(m_filePath);
	}

	/*!
	* \brief Gets the path of the file
	* \return Path of the file
	*/

	String File::GetPath() const
	{
		NazaraLock(m_mutex)

		return m_filePath;
	}

	/*!
	* \brief Gets the size of the file
	* \return Size of the file
	*/

	UInt64 File::GetSize() const
	{
		NazaraLock(m_mutex)

		return GetSize(m_filePath);
	}

	/*!
	* \brief Checks whether the file is open
	* \return true if open
	*/

	bool File::IsOpen() const
	{
		NazaraLock(m_mutex)

		return m_impl != nullptr;
	}

	/*!
	* \brief Opens the file with flags
	* \return true if opening is successful
	*
	* \param openMode Flag for file
	*
	* \remark Produces a NazaraError if OS error to open a file
	*/

	bool File::Open(unsigned int openMode)
	{
		NazaraLock(m_mutex)

		Close();

		if (m_filePath.IsEmpty())
			return false;

		if (openMode == OpenMode_NotOpen)
			return false;

		std::unique_ptr<FileImpl> impl(new FileImpl(this));
		if (!impl->Open(m_filePath, openMode))
		{
			ErrorFlags flags(ErrorFlag_Silent); // Silent by default
			NazaraError("Failed to open \"" + m_filePath + "\": " + Error::GetLastSystemError());
			return false;
		}

		m_openMode = openMode;
		m_impl = impl.release();

		if (m_openMode & OpenMode_Text)
			m_streamOptions |= StreamOption_Text;
		else
			m_streamOptions &= ~StreamOption_Text;

		return true;
	}

	/*!
	* \brief Opens the file with file path and flags
	* \return true if opening is successful
	*
	* \param filePath Path to the file
	* \param openMode Flag for file
	*
	* \remark Produces a NazaraError if OS error to open a file
	*/

	bool File::Open(const String& filePath, unsigned int openMode)
	{
		NazaraLock(m_mutex)

		Close();

		SetFile(filePath);
		return Open(openMode);
	}

	/*!
	* \brief Renames the file with a new name
	* \return true if rename is successful
	*/

	bool File::Rename(const String& newFilePath)
	{
		NazaraLock(m_mutex)

		bool open = IsOpen();
		Close();

		bool success = Rename(m_filePath, newFilePath);
		if (success)
			m_filePath = NormalizePath(newFilePath);

		if (open)
			Open();

		return success;
	}

	/*!
	* \brief Sets the position of the cursor
	* \return true if cursor is successfully positioned
	*
	* \param pos Position of the cursor
	* \param offset Offset according to the cursor position
	*
	* \remark Produces a NazaraAssert if file is not open
	*/

	bool File::SetCursorPos(CursorPosition pos, Int64 offset)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");

		return m_impl->SetCursorPos(pos, offset);
	}

	/*!
	* \brief Sets the position of the cursor
	* \return true if cursor is successfully positioned
	*
	* \param offset Offset according to the cursor begin position
	*
	* \remark Produces a NazaraAssert if file is not open
	*/

	bool File::SetCursorPos(UInt64 offset)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");

		return m_impl->SetCursorPos(CursorPosition_AtBegin, offset);
	}

	/*!
	* \brief Sets the file path
	* \return true if file opening is successful
	*
	* \remark Produces a NazaraError if file path can not be open
	*/

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

	/*!
	* \brief Sets the size of the file
	* \return true if the file size has correctly changed
	*
	* \param size The size the file should have after this call
	*
	* \remark The cursor position is not affected by this call
	* \remark The file must be open in write mode
	*/
	bool File::SetSize(UInt64 size)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");
		NazaraAssert(IsWritable(), "File is not writable");

		return m_impl->SetSize(size);
	}

	/*!
	* \brief Sets the file path
	* \return A reference to this
	*
	* \remark Produces a NazaraError if file path can not be open
	*/

	File& File::operator=(const String& filePath)
	{
		SetFile(filePath);

		return *this;
	}

	/*!
	* \brief Moves the other file into this
	* \return A reference to this
	*
	* \param file File to move in this
	*/

	File& File::operator=(File&& file) noexcept
	{
		NazaraLock(m_mutex)

		std::swap(m_filePath, file.m_filePath);
		std::swap(m_impl, file.m_impl);

		return *this;
	}

	/*!
	* \brief Gets the absolute path of the file
	* \return Absolute path of the file
	*
	* \param filePath Path of the file
	*
	* \remark Produces a NazaraError if filePath is weird with NAZARA_PLATFORM_WINDOWS defined
	*/

	String File::AbsolutePath(const String& filePath)
	{
		// We don't use OS functions because they only work for existing path
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
		else if (path.StartsWith('\\')) // Special : '\' refering to root
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

		// We have the absolute path, but we need to clean it up
		for (unsigned int i = 0; i < sep.size(); ++i)
		{
			if (sep[i] == '.')
				sep.erase(sep.begin() + i--);
			else if (sep[i] == "..")
			{
				if (i > start) // If we are not in the protected area
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

	/*!
	* \brief Copies the first file to a new file path
	* \return true if copy is successful
	*
	* \param sourcePath Path of the original file
	* \param targetPath Path of the copied file
	*/

	bool File::Copy(const String& sourcePath, const String& targetPath)
	{
		if (sourcePath.IsEmpty() || targetPath.IsEmpty())
			return false;

		return FileImpl::Copy(NormalizePath(sourcePath), NormalizePath(targetPath));
	}

	/*!
	* \brief Deletes the file
	* \return true if delete is successful
	*
	* \param filePath Path of the file
	*/

	bool File::Delete(const String& filePath)
	{
		if (filePath.IsEmpty())
			return false;

		return FileImpl::Delete(NormalizePath(filePath));
	}

	/*!
	* \brief Checks whether the file exists
	* \return true if file exists
	*
	* \param filePath Path of the file
	*/

	bool File::Exists(const String& filePath)
	{
		if (filePath.IsEmpty())
			return false;

		return FileImpl::Exists(NormalizePath(filePath));
	}

	/*!
	* \brief Gets the creation time of the file
	* \return Information about the creation time
	*
	* \param filePath Path of the file
	*/

	time_t File::GetCreationTime(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetCreationTime(NormalizePath(filePath));
	}

	/*!
	* \brief Gets the directory of the file
	* \return Directory of the file
	*
	* \param filePath Path of the file
	*/

	String File::GetDirectory(const String& filePath)
	{
		return filePath.SubStringTo(NAZARA_DIRECTORY_SEPARATOR, -1, true, true);
	}
	/*!
	* \brief Gets the last time the file was accessed
	* \return Information about the last access time
	*
	* \param filePath Path of the file
	*/

	time_t File::GetLastAccessTime(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetLastAccessTime(NormalizePath(filePath));
	}

	/*!
	* \brief Gets the last time the file was written
	* \return Information about the last writing time
	*
	* \param filePath Path of the file
	*/

	time_t File::GetLastWriteTime(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetLastWriteTime(NormalizePath(filePath));
	}

	/*!
	* \brief Gets the size of the file
	* \return Size of the file
	*
	* \param filePath Path of the file
	*/

	UInt64 File::GetSize(const String& filePath)
	{
		if (filePath.IsEmpty())
			return 0;

		return FileImpl::GetSize(NormalizePath(filePath));
	}

	/*!
	* \brief Checks whether the file path is absolute
	* \return true if path is absolute
	*
	* \param filePath Path to test
	*/

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
		else if (path.StartsWith('\\')) // Special : '\' referring to the root
			return true;
		else
			return false;
		#elif defined(NAZARA_PLATFORM_POSIX)
		return path.StartsWith('/');
		#else
			#error OS case not implemented
		#endif
	}

	/*!
	* \brief Normalizes the file path
	* \return Path normalized (replacing '/' with '\\' on Windows, ...)
	*
	* \param filePath Path to normalize
	*/

	String File::NormalizePath(const String& filePath)
	{
		String path = NormalizeSeparators(filePath.Trimmed());

		if (!IsAbsolute(path))
			path = Directory::GetCurrent() + NAZARA_DIRECTORY_SEPARATOR + path;

		while (path.EndsWith(NAZARA_DIRECTORY_SEPARATOR))
			path.Resize(-1);

		return path;
	}

	/*!
	* \brief Normalizes the path separator
	* \return Path normalized (replacing '/' with '\\' on Windows, ...)
	*
	* \param filePath Path to normalize
	*/

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

	/*!
	* \brief Renames the file with a new name
	* \return true if rename is successful
	*
	* \param sourcePath Path of the original file
	* \param targetPath Path of the renamed file
	*/

	bool File::Rename(const String& sourcePath, const String& targetPath)
	{
		if (sourcePath.IsEmpty() || targetPath.IsEmpty())
			return false;

		return FileImpl::Rename(NormalizePath(sourcePath), NormalizePath(targetPath));
	}

	/*!
	* \brief Flushes the stream
	*
	* \remark Produces a NazaraAssert if file is not open
	*/

	void File::FlushStream()
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");

		m_impl->Flush();
	}

	/*!
	* \brief Reads blocks
	* \return Number of blocks read
	*
	* \param buffer Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*
	* \remark Produces a NazaraAssert if file is not open
	*/

	std::size_t File::ReadBlock(void* buffer, std::size_t size)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");

		if (size == 0)
			return 0;

		if (buffer)
			return m_impl->Read(buffer, size);
		else
		{
			// If we don't have to read, we move forward
			UInt64 currentPos = m_impl->GetCursorPos();

			m_impl->SetCursorPos(CursorPosition_AtCurrent, size);

			return static_cast<std::size_t>(m_impl->GetCursorPos() - currentPos);
		}
	}

	/*!
	* \brief Writes blocks
	* \return Number of blocks written
	*
	* \param buffer Preallocated buffer containing information to write
	* \param size Size of the writting and thus of the buffer
	*
	* \remark Produces a NazaraAssert if file is not open
	* \remark Produces a NazaraAssert if buffer is nullptr
	*/

	std::size_t File::WriteBlock(const void* buffer, std::size_t size)
	{
		NazaraLock(m_mutex)

		NazaraAssert(IsOpen(), "File is not open");

		if (size == 0)
			return 0;

		NazaraAssert(buffer, "Invalid buffer");

		return m_impl->Write(buffer, size);
	}

	/*!
	* \brief Appends the file to the hash
	* \return true if hash is successful
	*
	* \param hash Hash to append data of the file
	* \param originalFile Path of the file
	*
	* \remark Produces a NazaraAssert if hash is nullptr
	* \remark Produces a NazaraError if file could not be open
	* \remark Produces a NazaraError if file could not be read
	*/

	NAZARA_CORE_API bool HashAppend(AbstractHash* hash, const File& originalFile)
	{
		NazaraAssert(hash, "Invalid hash");

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
			std::size_t size = std::min<std::size_t>(static_cast<std::size_t>(remainingSize), NAZARA_CORE_FILE_BUFFERSIZE);
			if (file.Read(&buffer[0], size) != size)
			{
				NazaraError("Unable to read file");
				return false;
			}

			remainingSize -= size;
			hash->Append(reinterpret_cast<UInt8*>(&buffer[0]), size);
		}

		return true;
	}
}
