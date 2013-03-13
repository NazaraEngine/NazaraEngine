// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Debug.hpp>

NzInputStream::~NzInputStream() = default;

NzString NzInputStream::ReadLine(unsigned int lineSize)
{
	NzString line;
	if (lineSize == 0) // Taille maximale indéterminée
	{
		while (!EndOfStream())
		{
			char c;
			if (Read(&c, sizeof(char)) == sizeof(char))
			{
				if (c == '\n')
					break;

				line += c;
			}
			else
				break;
		}
	}
	else
	{
		line.Reserve(lineSize);
		for (unsigned int i = 0; i < lineSize; ++i)
		{
			char c;
			if (Read(&c, sizeof(char)) == sizeof(char))
			{
				if (c == '\n')
					break;

				line += c;
			}
			else
				break;
		}
	}

	if (m_streamOptions & nzStreamOption_Text && !EndOfStream() && line.EndsWith('\r'))
		line.Resize(-1);

	return line;
}
