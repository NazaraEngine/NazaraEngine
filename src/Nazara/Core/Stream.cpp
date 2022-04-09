// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

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
	* \param lineSize Maximum number of characters to read, or zero for no limit
	*
	* \return Line read from file
	*
	* \remark With the text stream option, "\r\n" is treated as "\n"
	* \remark The line separator character is not returned as part of the string
	*/
	std::string Stream::ReadLine(unsigned int lineSize)
	{
		std::string line;
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
			line.resize(lineSize, '\0');
			std::size_t readSize = Read(&line[0], lineSize);
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
				line.resize(readSize);
		}

		return line;
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

	bool Stream::Write(const std::string_view& string)
	{
		if (m_streamOptions & StreamOption::Text)
		{
#if defined(NAZARA_PLATFORM_WINDOWS)
			std::string temp(string);
			ReplaceStr(temp, "\n", "\r\n");
#elif defined(NAZARA_PLATFORM_LINUX)
			std::string_view temp(string);
			// Nothing to do
#elif defined(NAZARA_PLATFORM_MACOS)
			std::string temp(string);
			ReplaceStr(temp, "\n", "\r");
#endif

			return Write(temp.data(), temp.size()) == temp.size();
		}
		else
			return Write(string.data(), string.size()) == string.size();
	}
}
