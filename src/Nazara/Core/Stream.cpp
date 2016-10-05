// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
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

	String Stream::GetDirectory() const
	{
		return String();
	}

	/*!
	* \brief Gets the path of the stream
	* \return Empty string (meant to be virtual)
	*/

	String Stream::GetPath() const
	{
		return String();
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
	* \return Line containing characters
	*
	* \remark With the text stream option, "\r\n" is treated as "\n"
	* \remark The line separator character is not returned as part of the string
	*/
	String Stream::ReadLine(unsigned int lineSize)
	{
		String line;
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
						if (m_streamOptions & StreamOption_Text && buffer[pos - 1] == '\r')
							line.Append(buffer, pos - 1);
						else
							line.Append(buffer, pos);
					}

					if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
						NazaraWarning("Failed to reset cursor pos");

					if (!line.IsEmpty())
						break;
				}
				else
				{
					std::size_t length = readSize;
					if (m_streamOptions & StreamOption_Text && buffer[length - 1] == '\r')
					{
						if (!SetCursorPos(GetCursorPos() - 1))
							NazaraWarning("Failed to reset cursor pos");

						length--;
					}

					line.Append(buffer, length);
				}
			}
			while (readSize == bufferSize);
		}
		else
		{
			line.Set(lineSize, '\0');
			std::size_t readSize = Read(&line[0], lineSize);
			std::size_t pos = line.Find('\n');
			if (pos <= readSize) // False only if the character is not available (npos being the biggest integer)
			{
				if (m_streamOptions & StreamOption_Text && pos > 0 && line[pos - 1] == '\r')
					line.Resize(pos);
				else
					line.Resize(pos + 1);

				if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
					NazaraWarning("Failed to reset cursos pos");
			}
			else
				line.Resize(readSize);
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

	bool Stream::Write(const String& string)
	{
		String temp(string);

		if (m_streamOptions & StreamOption_Text)
		{
			#if defined(NAZARA_PLATFORM_WINDOWS)
			temp.Replace("\n", "\r\n");
			#elif defined(NAZARA_PLATFORM_LINUX)
			// Nothing to do
			#elif defined(NAZARA_PLATFORM_MACOS)
			temp.Replace('\n', '\r');
			#endif
		}

		std::size_t size = temp.GetSize();
		return Write(temp.GetConstBuffer(), size) == size;
	}
}
