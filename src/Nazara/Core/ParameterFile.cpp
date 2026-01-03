// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ParameterFile.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	auto ParameterFile::Advance() -> Token
	{
		auto IsAlphaNum = [&](char c)
		{
			return std::isalnum(c) || c == '_';
		};

		// Remove processed tokens from buffer
		m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_bufferOffset);
		m_bufferOffset = 0;

		Token currentToken = std::move(m_nextToken);

		for (;;)
		{
			char c = PeekCharacter(0);
			if (c == '\0')
			{
				m_nextToken = EndOfStream{};
				return currentToken;
			}

			switch (c)
			{
				case ' ':
				case '\t':
				case '\r':
					break; //< Ignore blank spaces

				case '\n':
					m_currentLine++;
					break;

				case '{':
					ConsumeChar();
					m_nextToken = OpenCurlyBracket{};
					return currentToken;

				case '}':
					ConsumeChar();
					m_nextToken = ClosingCurlyBracket{};
					return currentToken;

				case '/':
				{
					char next = PeekCharacter();
					if (next == '/')
					{
						// Line comment
						do
						{
							ConsumeChar();
							next = PeekCharacter();
						}
						while (next != '\0' && next != '\n');
					}
					else if (next == '*')
					{
						// Block comment
						for (;;)
						{
							ConsumeChar();
							next = PeekCharacter();

							if (next == '*')
							{
								if (PeekCharacter(2) == '/')
								{
									ConsumeChar(2);
									break;
								}
							}
							else if (next == '\n')
								m_currentLine++;
							else if (next == '\0')
								throw std::runtime_error(Format("unfinished block comment on line {}", m_currentLine));
						}
					}

					break;
				}

				case '"':
				{
					// string literal
					ConsumeChar();

					std::string literal;

					char cur;
					while ((cur = PeekCharacter(0)) != '"')
					{
						char character;
						switch (cur)
						{
							case '\0':
							case '\n':
							case '\r':
								throw std::runtime_error(Format("unfinished string on line {}", m_currentLine));

							case '\\':
							{
								ConsumeChar();
								char next = PeekCharacter(0);
								switch (next)
								{
									case 'n':  character = '\n'; break;
									case 'r':  character = '\r'; break;
									case 't':  character = '\t'; break;
									case '"':  character = '"'; break;
									case '\\': character = '\\'; break;
									default:
										throw std::runtime_error(Format("unrecognized character on line {}", m_currentLine));
								}
								break;
							}

							default:
								character = cur;
								break;
						}

						literal.push_back(character);
						ConsumeChar();
					}
					ConsumeChar();

					m_nextToken = String{ std::move(literal) };
					return currentToken;
				}

				default:
				{
					if (IsAlphaNum(c))
					{
						// Identifier or keyword
						std::size_t start = m_bufferOffset;

						while (IsAlphaNum(PeekCharacter()))
							ConsumeChar();

						m_nextToken = Identifier{ m_buffer.substr(start, m_bufferOffset - start) };
						return currentToken;
					}
					else
						throw std::runtime_error(Format("unrecognized token on line {}", m_currentLine));
				}
			}

			ConsumeChar();
		}
	}

	void ParameterFile::ConsumeChar(std::size_t count)
	{
		assert(m_bufferOffset < m_buffer.size());
		assert(m_bufferOffset + count <= m_buffer.size());
		m_bufferOffset += count;
	}

	auto ParameterFile::PeekToken() -> Token&
	{
		if (std::holds_alternative<std::monostate>(m_nextToken))
			Advance();

		return m_nextToken;
	}

	char ParameterFile::PeekCharacter(std::size_t advance)
	{
		constexpr std::size_t Capacity = 512;

		if NAZARA_UNLIKELY(m_bufferOffset + advance >= m_buffer.size())
		{
			if NAZARA_UNLIKELY(m_stream.EndOfStream())
				return '\0';

			std::size_t prevSize = m_buffer.size();
			m_buffer.resize(prevSize + Capacity);
			std::size_t readSize = m_stream.Read(&m_buffer[prevSize], Capacity);
			m_buffer.resize(prevSize + readSize);

			if (m_bufferOffset + advance >= m_buffer.size())
				return '\0';
		}

		return m_buffer[m_bufferOffset];
	}

	// Required for MinGW (it tris to import those symbols, probably a bug)
	constexpr ParameterFile::List_t ParameterFile::List;
	constexpr ParameterFile::OptionalBlock_t ParameterFile::OptionalBlock;
}
