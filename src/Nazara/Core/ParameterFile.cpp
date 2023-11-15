// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ParameterFile.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool ParameterFile::EnsureLine(bool peek)
	{
		while (m_currentLine.find_first_not_of(" \r\t\n") == m_currentLine.npos)
		{
			m_currentLine.clear();
			m_stream.ReadLine(m_currentLine);
			if (m_currentLine.empty())
			{
				if (!peek)
					throw std::runtime_error("unexpected end of file");

				return false;
			}
		}

		return true;
	}

	std::string ParameterFile::ReadKeyword(bool peek)
	{
		std::size_t beginOffset;
		do
		{
			if (!EnsureLine(peek))
				return {};

			beginOffset = m_currentLine.find_first_not_of(" \r\t\n");
		} while (beginOffset == m_currentLine.npos);

		if (m_currentLine[beginOffset] == '"')
			throw std::runtime_error("expected a keyword, got a string");

		std::size_t endOffset = m_currentLine.find_first_of(" \r\t\n", beginOffset + 1);
		if (endOffset == m_currentLine.npos)
			endOffset = m_currentLine.size();

		std::string currentToken = std::string(m_currentLine.substr(beginOffset, endOffset - beginOffset));
		if (!peek)
			m_currentLine.erase(m_currentLine.begin(), m_currentLine.begin() + endOffset);

		return currentToken;
	}

	std::string ParameterFile::ReadString()
	{
		std::size_t beginOffset;
		do
		{
			EnsureLine();
			beginOffset = m_currentLine.find_first_not_of(" \r\t\n");
		} while (beginOffset == m_currentLine.npos);

		if (m_currentLine[beginOffset] != '"')
			throw std::runtime_error("expected a string, got a keyword");

		std::string str;
		for (std::size_t i = beginOffset + 1; i < m_currentLine.size(); ++i)
		{
			switch (m_currentLine[i])
			{
				case '\0':
				case '\n':
				case '\r':
					throw std::runtime_error("expected a string, got a keyword");

				case '"':
				{
					m_currentLine.erase(m_currentLine.begin(), m_currentLine.begin() + beginOffset + i);

					return str;
				}

				case '\\':
				{
					i++;
					char character;
					switch (m_currentLine[i])
					{
						case 'n': character = '\n'; break;
						case 'r': character = '\r'; break;
						case 't': character = '\t'; break;
						case '"': character = '"'; break;
						case '\\': character = '\\'; break;
						default:
							throw std::runtime_error(Format("unrecognized character {}", character));
					}

					str.push_back(character);
					break;
				}

				default:
					str.push_back(m_currentLine[i]);
			}
		}

		throw std::runtime_error("unfinished string");
	}

	// Required for MinGW (it tris to import those symbols, probably a bug)
	constexpr ParameterFile::Array_t ParameterFile::Array;
	constexpr ParameterFile::OptionalBlock_t ParameterFile::OptionalBlock;
}
