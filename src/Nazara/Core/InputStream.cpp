// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

NzInputStream::~NzInputStream() = default;

NzString NzInputStream::ReadLine(unsigned int lineSize)
{
	NzString line;
	if (lineSize == 0) // Taille maximale indéterminée
	{
		const unsigned int bufferSize = 64;

		char buffer[bufferSize+1];
		buffer[bufferSize] = '\0';

		unsigned int readSize;
		do
		{
			readSize = Read(buffer, bufferSize);

			const char* ptr = std::strchr(buffer, '\n');
			if (ptr)
			{
				unsigned int pos = ptr-buffer;

				if (m_streamOptions & nzStreamOption_Text && pos > 0 && buffer[pos-1] == '\r')
					line.Append(buffer, pos-1);
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
		line.Resize(lineSize);
		unsigned int readSize = Read(&line[0], lineSize);
		unsigned int pos = line.Find('\n');
		if (pos <= readSize) // Forcément trouvé, npos étant le plus grand des entiers
		{
			if (m_streamOptions & nzStreamOption_Text && pos > 0 && line[pos-1] == '\r')
				line.Resize(pos);
			else
				line.Resize(pos+1);

			if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
				NazaraWarning("Failed to reset cursos pos");
		}
		else
			line.Resize(readSize);
	}

	return line;
}
