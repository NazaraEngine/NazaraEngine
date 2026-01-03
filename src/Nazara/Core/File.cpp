// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/File.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Export.hpp>
#include <memory>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/FileImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/FileImpl.hpp>
#else
	#error OS not handled
#endif


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

	File::File() = default;

	/*!
	* \brief Constructs a File object with a file path
	*
	* \param filePath Path to the file
	*/

	File::File(const std::filesystem::path& filePath) :
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

	File::File(const std::filesystem::path& filePath, OpenModeFlags openMode) :
	File()
	{
		Open(filePath, openMode);
	}

	/*!
	* \brief Destructs the object and calls Close
	*
	* \see Close
	*/

	File::File(File&& file) noexcept = default;

	File::~File()
	{
		Close();
	}

	/*!
	* \brief Copies this file to a new file path
	*
	* \param newFilePath Path of the new file
	*/
	void File::Copy(const std::filesystem::path& newFilePath)
	{
		std::filesystem::copy(m_filePath, newFilePath);
	}

	/*!
	* \brief Closes the file
	*/

	void File::Close()
	{
		if (m_impl)
		{
			m_impl.reset();

			m_openMode = OpenMode::NotOpen;
		}
	}

	/*!
	* \brief Deletes the file
	* \return true if delete is successful
	*/

	bool File::Delete()
	{
		Close();

		return std::filesystem::remove(m_filePath);
	}

	/*!
	* \brief Checks whether the file exists
	* \return true if file exists
	*/

	bool File::Exists() const
	{
		if (IsOpen())
			return true; // Le fichier est ouvert, donc il existe
		else
			return std::filesystem::exists(m_filePath);
	}

	/*!
	* \brief Gets the directory of the file
	* \return Directory of the file
	*/
	std::filesystem::path File::GetDirectory() const
	{
		return m_filePath.parent_path();
	}

	/*!
	* \brief Gets the name of the file
	* \return Name of the file
	*/
	std::filesystem::path File::GetFileName() const
	{
		return m_filePath.filename();
	}

	/*!
	* \brief Gets the path of the file
	* \return Path of the file
	*/
	std::filesystem::path File::GetPath() const
	{
		return m_filePath;
	}

	UInt64 File::GetSize() const
	{
		return std::filesystem::file_size(m_filePath);
	}

	/*!
	* \brief Checks whether the file is open
	* \return true if open
	*/

	bool File::IsOpen() const
	{
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

	bool File::Open(OpenModeFlags openMode)
	{
		Close();

		if (m_filePath.empty())
			return false;

		if (openMode == OpenMode::NotOpen)
			return false;

		m_openMode = openMode;
		if (m_openMode.Test(OpenMode::Defer))
		{
			// defer opening until a read/write operation is performed
			return true;
		}

		std::unique_ptr<PlatformImpl::FileImpl> impl = std::make_unique<PlatformImpl::FileImpl>(this);
		if (!impl->Open(m_filePath, openMode))
		{
			ErrorFlags flags(ErrorMode::Silent); // Silent by default
			NazaraError("failed to open \"{0}\": {1}", m_filePath, Error::GetLastSystemError());
			return false;
		}

		m_impl = std::move(impl);

		EnableBuffering(!m_openMode.Test(OpenMode::Unbuffered));

		if (m_openMode & OpenMode::Text)
			m_streamOptions |= StreamOption::Text;
		else
			m_streamOptions &= ~StreamOption::Text;

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

	bool File::Open(const std::filesystem::path& filePath, OpenModeFlags openMode)
	{
		Close();

		SetFile(filePath);
		return Open(openMode);
	}

	/*!
	* \brief Sets the file path
	* \return true if file opening is successful
	*
	* \remark Produces a NazaraError if file path can not be open
	*/

	bool File::SetFile(const std::filesystem::path& filePath)
	{
		if (IsOpen())
		{
			if (filePath.empty())
				return false;

			std::unique_ptr<PlatformImpl::FileImpl> impl = std::make_unique<PlatformImpl::FileImpl>(this);
			if (!impl->Open(filePath, m_openMode))
			{
				NazaraError("failed to open new file; {0}", Error::GetLastSystemError());
				return false;
			}

			m_impl = std::move(impl);
		}

		m_filePath = std::filesystem::absolute(filePath);
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
		if (!CheckFileOpening())
			return false;

		NazaraAssertMsg(IsOpen(), "file is not open");
		NazaraAssertMsg(IsWritable(), "file is not writable");

		return m_impl->SetSize(size);
	}

	File& File::operator=(File&& file) noexcept = default;

	std::optional<std::vector<UInt8>> File::ReadWhole(const std::filesystem::path& path)
	{
		File file(path);
		if (!file.Open(OpenMode::Read | OpenMode::Unbuffered)) //< unbuffered since we will read all the file at once
		{
			NazaraError("failed to open \"{0}\"", path);
			return std::nullopt;
		}

		std::size_t size = static_cast<std::size_t>(file.GetSize());
		std::vector<UInt8> content(size);
		if (size > 0 && file.Read(&content[0], size) != size)
		{
			NazaraError("failed to read file");
			return std::nullopt;
		}

		return content;
	}

	bool File::WriteWhole(const std::filesystem::path& path, const void* data, std::size_t size)
	{
		File file(path);
		if (!file.Open(OpenMode::Write | OpenMode::Unbuffered)) //< unbuffered since we will write all the file at once
		{
			NazaraError("failed to open \"{0}\"", path);
			return false;
		}

		if (file.Write(data, size) != size)
		{
			NazaraError("failed to write file");
			return false;
		}

		return true;
	}

	/*!
	* \brief Flushes the stream
	*
	* \remark Produces a NazaraAssertMsg if file is not open
	*/

	void File::FlushStream()
	{
		if (!IsOpen() && m_openMode.Test(OpenMode::Defer))
			return;

		NazaraAssertMsg(IsOpen(), "File is not open");

		m_impl->Flush();
	}

	/*!
	* \brief Reads blocks
	* \return Number of blocks read
	*
	* \param buffer Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*
	* \remark Produces a NazaraAssertMsg if file is not open
	*/

	std::size_t File::ReadBlock(void* buffer, std::size_t size)
	{
		if (!CheckFileOpening())
			return false;

		NazaraAssertMsg(IsOpen(), "File is not open");

		if (size == 0)
			return 0;

		if (buffer)
			return m_impl->Read(buffer, size);
		else
		{
			// If we don't have to read, we move forward
			UInt64 currentPos = m_impl->GetCursorPos();
			if (!m_impl->SetCursorPos(CursorPosition::AtCurrent, size))
				return 0;

			return static_cast<std::size_t>(m_impl->GetCursorPos() - currentPos);
		}
	}

	/*!
	* \brief Sets the position of the cursor
	* \return true if cursor is successfully positioned
	*
	* \param offset Offset according to the cursor begin position
	*
	* \remark Produces a NazaraAssertMsg if file is not open
	*/

	bool File::SeekStreamCursor(UInt64 offset)
	{
		if (!CheckFileOpening())
			return false;

		NazaraAssertMsg(IsOpen(), "File is not open");

		return m_impl->SetCursorPos(CursorPosition::AtBegin, offset);
	}

	/*!
	* \brief Gets the position of the cursor in the file
	* \return Position of the cursor
	*
	* \remark Produces a NazaraAssertMsg if file is not open
	*/
	UInt64 File::TellStreamCursor() const
	{
		if (!IsOpen() && m_openMode.Test(OpenMode::Defer))
			return 0;

		NazaraAssertMsg(IsOpen(), "File is not open");

		return m_impl->GetCursorPos();
	}

	/*!
	* \brief Checks whether the file has reached the end of the stream
	* \return true if cursor is at the end of the file
	*
	* \see EndOfFile
	*/
	bool File::TestStreamEnd() const
	{
		if (!IsOpen() && m_openMode.Test(OpenMode::Defer))
			return false;

		NazaraAssertMsg(IsOpen(), "File is not open");

		return m_impl->EndOfFile();
	}

	/*!
	* \brief Writes blocks
	* \return Number of blocks written
	*
	* \param buffer Preallocated buffer containing information to write
	* \param size Size of the writting and thus of the buffer
	*
	* \remark Produces a NazaraAssertMsg if file is not open
	* \remark Produces a NazaraAssertMsg if buffer is nullptr
	*/
	std::size_t File::WriteBlock(const void* buffer, std::size_t size)
	{
		if (!CheckFileOpening())
			return false;

		NazaraAssertMsg(IsOpen(), "File is not open");

		if (size == 0)
			return 0;

		NazaraAssertMsg(buffer, "Invalid buffer");

		return m_impl->Write(buffer, size);
	}

	/*!
	* \brief Appends the file to the hash
	* \return true if hash is successful
	*
	* \param hash Hash to append data of the file
	* \param originalFile Path of the file
	*
	* \remark Produces a NazaraAssertMsg if hash is nullptr
	* \remark Produces a NazaraError if file could not be open
	* \remark Produces a NazaraError if file could not be read
	*/
	NAZARA_CORE_API bool HashAppend(AbstractHash& hash, const File& originalFile)
	{
		File file(originalFile.GetPath());
		if (!file.Open(OpenMode::Read))
		{
			NazaraError("unable to open file");
			return false;
		}

		UInt64 remainingSize = file.GetSize();

		constexpr std::size_t BufferSize = Stream::DefaultBufferSize;
		std::unique_ptr<UInt8[]> buffer = std::make_unique_for_overwrite<UInt8[]>(BufferSize);
		while (remainingSize > 0)
		{
			std::size_t size = static_cast<std::size_t>(std::min<UInt64>(remainingSize, BufferSize));
			if (file.Read(&buffer[0], size) != size)
			{
				NazaraError("unable to read file");
				return false;
			}

			remainingSize -= size;
			hash.Append(reinterpret_cast<UInt8*>(&buffer[0]), size);
		}

		return true;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
