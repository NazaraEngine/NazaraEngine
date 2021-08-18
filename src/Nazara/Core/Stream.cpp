// Copyright (C) 2020 Jérôme Leclercq
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
		return std::filesystem::path();
	}

	/*!
	* \brief Gets the path of the stream
	* \return Empty string (meant to be virtual)
	*/

	std::filesystem::path Stream::GetPath() const
	{
		return std::filesystem::path();
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
#elif defined(NAZARA_PLATFORM_MACOSX)
			std::string temp(string);
			ReplaceStr(temp, "\n", "\r");
#endif

			return Write(temp.data(), temp.size()) == temp.size();
		}
		else
			return Write(string.data(), string.size()) == string.size();
	}
}
