// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Stream::~Stream() = default;

	String Stream::GetDirectory() const
	{
		return String();
	}

	String Stream::GetPath() const
	{
		return String();
	}

	String Stream::ReadLine(unsigned int lineSize)
	{
		String line;
		if (lineSize == 0) // Taille maximale indéterminée
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

					if (m_streamOptions & StreamOption_Text && pos > 0 && buffer[pos - 1] == '\r')
						line.Append(buffer, pos - 1);
					else
						line.Append(buffer, pos);

					if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
						NazaraWarning("Failed to reset cursos pos");

					break;
				}
				else
					line.Append(buffer, readSize);
			}
			while (readSize == bufferSize);
		}
		else
		{
			line.Set(lineSize, '\0');
			std::size_t readSize = Read(&line[0], lineSize);
			std::size_t pos = line.Find('\n');
			if (pos <= readSize) // Faux uniquement si le caractère n'est pas présent (npos étant le plus grand entier)
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

	bool Stream::Write(const ByteArray& byteArray)
	{
		ByteArray::size_type size = byteArray.GetSize();
		return Write(byteArray.GetConstBuffer(), size) == size;
	}

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
