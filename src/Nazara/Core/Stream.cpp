// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <cstring>
#include <memory>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Stream
	* \brief Core class that represents a stream
	*/

	/*!
	* \brief Destructs the object
	*/

	Stream::~Stream() = default;

	bool Stream::EndOfStream() const
	{
		if (m_bufferCapacity > 0)
		{
			if (m_bufferOffset < m_bufferSize)
				return false;
		}

		return TestStreamEnd();
	}

	UInt64 Stream::GetCursorPos() const
	{
		if (m_bufferCapacity == 0)
			return TellStreamCursor();
		else
		{
			assert(m_bufferCursor >= m_bufferSize);
			return m_bufferCursor - m_bufferSize + m_bufferOffset;
		}
	}

	/*!
	* \brief Gets the directory of the stream
	* \return Empty string (meant to be virtual)
	*/
	std::filesystem::path Stream::GetDirectory() const
	{
		return {};
	}

	/*!
	* \brief Gets the path of the stream
	* \return Empty string (meant to be virtual)
	*/
	std::filesystem::path Stream::GetPath() const
	{
		return {};
	}

	/*!
	* \brief Reads the stream and puts the result in a buffer
	* \return Size of the read
	*
	* \param buffer Buffer to stock data
	* \param size Size meant to be read
	*
	* \remark Produces a NazaraAssert if stream is not readable
	* \remark If preallocated space of buffer is less than the size, the behavior is undefined
	*/
	std::size_t Stream::Read(void* buffer, std::size_t size)
	{
		NazaraAssert(IsReadable(), "Stream is not readable");

		if (m_bufferCapacity == 0)
			return ReadBlock(buffer, size);

		UInt8* ptr = static_cast<UInt8*>(buffer);

		std::size_t readSize = 0;
		if (m_bufferOffset < m_bufferSize)
		{
			std::size_t availableSize = std::min(size, m_bufferSize - m_bufferOffset);
			if (ptr)
			{
				std::memcpy(ptr, &m_buffer[m_bufferOffset], availableSize);
				ptr += availableSize;
			}

			m_bufferOffset += availableSize;
			readSize += availableSize;
			size -= availableSize;
		}

		if (size > m_bufferCapacity)
		{
			// Unbuffered read
			m_bufferOffset = 0;
			m_bufferSize = 0;
			std::size_t blockSize = ReadBlock(ptr, size);
			m_bufferCursor += blockSize;
			readSize += blockSize;
		}
		else if (size > 0)
		{
			m_bufferOffset = 0;
			m_bufferSize = ReadBlock(&m_buffer[0], m_bufferCapacity);
			m_bufferCursor += m_bufferSize;

			std::size_t remainingSize = std::min(m_bufferSize, size);
			if (ptr)
			{
				std::memcpy(ptr, &m_buffer[m_bufferOffset], remainingSize);
				ptr += remainingSize;
			}

			m_bufferOffset += remainingSize;
			readSize += remainingSize;
			size -= remainingSize;
		}

		return readSize;
	}

	/*!
	* \brief Reads a line from the stream
	*
	* Reads the stream until a line separator or the end of the stream is found.
	*
	* If lineSize does not equal zero, it represents the maximum character count to be read from the stream.
	*
	* \param line Line object to use to store characters
	* \param lineSize Maximum number of characters to read, or zero for no limit
	*
	* \return Line read from file
	*
	* \remark With the text stream option, "\r\n" is treated as "\n"
	* \remark The line separator character is not returned as part of the string
	*/
	void Stream::ReadLine(std::string& line, unsigned int lineSize)
	{
		if (lineSize == 0) // Maximal size undefined
		{
			const unsigned int bufferSize = 64;

			char buffer[bufferSize + 1];
			buffer[bufferSize] = '\0';

			std::size_t readSize;
			do
			{
				readSize = Read(buffer, bufferSize);

				const char* ptr = std::strchr(buffer, '\n');
				if (ptr)
				{
					std::ptrdiff_t pos = ptr - buffer;
					if (ptr != buffer)
					{
						if (m_streamOptions & StreamOption::Text && buffer[pos - 1] == '\r')
							line.append(buffer, pos - 1);
						else
							line.append(buffer, pos);
					}

					if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
						NazaraWarning("Failed to reset cursor pos");

					break;
				}
				else
				{
					std::size_t length = readSize;
					if (m_streamOptions & StreamOption::Text && buffer[length - 1] == '\r')
					{
						if (!SetCursorPos(GetCursorPos() - 1))
							NazaraWarning("Failed to reset cursor pos");

						length--;
					}

					line.append(buffer, length);
				}
			}
			while (readSize == bufferSize);
		}
		else
		{
			std::size_t offset = line.size();
			line.resize(offset + lineSize, '\0');
			std::size_t readSize = Read(&line[offset], lineSize);
			std::size_t pos = line.find('\n');
			if (pos <= readSize) // False only if the character is not available (npos being the biggest integer)
			{
				if (m_streamOptions & StreamOption::Text && pos > 0 && line[pos - 1] == '\r')
					line.resize(pos);
				else
					line.resize(pos + 1);

				if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
					NazaraWarning("Failed to reset cursos pos");
			}
			else
				line.resize(offset + readSize);
		}
	}

	bool Stream::SetCursorPos(UInt64 offset)
	{
		if (m_bufferCapacity == 0)
			return SeekStreamCursor(offset);
		else
		{
			assert(m_bufferCursor >= m_bufferSize);
			if (offset < m_bufferCursor && offset >= m_bufferCursor - m_bufferSize)
				m_bufferOffset = offset - (m_bufferCursor - m_bufferSize);
			else
			{
				// Out of buffer
				if (!SeekStreamCursor(offset))
					return false;

				m_bufferCursor = offset;
				m_bufferOffset = 0;
				m_bufferSize = 0;
			}

			return true;
		}
	}

	/*!
	* \brief Writes a ByteArray into the stream
	* \return true if successful
	*
	* \param byteArray Bytes to write
	*/

	bool Stream::Write(const ByteArray& byteArray)
	{
		ByteArray::size_type size = byteArray.GetSize();
		return Write(byteArray.GetConstBuffer(), size) == size;
	}

	/*!
	* \brief Writes a String into the stream
	* \return true if successful
	*
	* \param string String to write
	*/

	bool Stream::Write(std::string_view string)
	{
		if (m_streamOptions & StreamOption::Text)
		{
#if defined(NAZARA_PLATFORM_WINDOWS)
			std::string temp(string);
			ReplaceStr(temp, "\n", "\r\n");

			string = temp;
#elif defined(NAZARA_PLATFORM_MACOS)
			std::string temp(string);
			ReplaceStr(temp, "\n", "\r");

			string = temp;
#endif
		}

		return Write(string.data(), string.size()) == string.size();
	}

	void* Stream::GetMemoryMappedPointer() const
	{
		NazaraError("stream set the MemoryMapped option but did not implement GetMemoryMappedPointer");
		return nullptr;
	}

	bool HashAppend(AbstractHash& hash, Stream& stream)
	{
		if (stream.IsMemoryMapped())
		{
			const void* ptr = stream.GetMappedPointer();
			UInt64 size = stream.GetSize();
			if (ptr && size > 0)
			{
				hash.Append(static_cast<const UInt8*>(ptr), size);
				return true;
			}
		}

		constexpr std::size_t BufferSize = Stream::DefaultBufferSize;

		std::unique_ptr<UInt8[]> buffer = std::make_unique<UInt8[]>(BufferSize);

		// Save and restore cursor position after the call
		std::size_t cursorPos = stream.GetCursorPos();
		CallOnExit restoreCursorPos([&] { stream.SetCursorPos(cursorPos); });

		stream.SetCursorPos(0);

		while (!stream.EndOfStream())
		{
			std::size_t readSize = stream.Read(&buffer[0], BufferSize);
			if (readSize > 0)
				hash.Append(&buffer[0], readSize);

			if (readSize != BufferSize)
			{
				if (!stream.EndOfStream())
				{
					NazaraError("failed to read from stream");
					return false;
				}

				break;
			}
		}

		return true;
	}
}
