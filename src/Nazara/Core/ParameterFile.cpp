// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ParameterFile.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <fast_float/fast_float.h>

namespace Nz
{
	auto ParameterFile::Advance() -> Token
	{
		auto IsAlphaNum = [&](char c)
		{
			return std::isalnum(c) || c == '_';
		};

		std::string literalTemp;

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

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					literalTemp.clear();

					std::size_t start = m_bufferOffset;
					char next = PeekCharacter();

					int base = 10;
					if (next == 'x' || next == 'X')
					{
						base = 16;
						m_bufferOffset++;
					}
					else if (next == 'b' || next == 'B')
					{
						base = 2;
						m_bufferOffset++;
					}
					else
						literalTemp.push_back(c);

					bool floatingPoint = false;
					for (;;)
					{
						auto IsDigitOrSep = [=](char c)
						{
							if (c == '_')
								return true;

							if (c >= '0' && c <= '9')
								return true;

							if (base > 10)
							{
								if (c >= 'a' && c <= 'f')
									return true;

								if (c >= 'A' && c <= 'F')
									return true;
							}

							return false;
						};

						next = PeekCharacter();

						if (!IsDigitOrSep(next))
						{
							if (next != '.')
								break;

							if (floatingPoint)
								break;

							floatingPoint = true;
						}

						if (next != '_')
							literalTemp.push_back(next);

						m_bufferOffset++;
					}

					if (base != 10)
					{
						if (c != '0')
							throw std::runtime_error(Format("bad number on line {}", m_currentLine));

						if (floatingPoint)
							throw std::runtime_error(Format("floating-point number can only be specified using base 10 (got base {}) on line {}", base, m_currentLine));
					}

					// avoid std::string operator[] assertions (as &literalTemp[literalTemp.size()] is out of the string)
					const char* first = literalTemp.data();
					const char* last = first + literalTemp.size();

					if (floatingPoint)
					{
						double value;
						fast_float::from_chars_result r = fast_float::from_chars(first, last, value);
						if (r.ptr == last && r.ec == std::errc{})
							return Float{ value };
						else if (r.ec == std::errc::result_out_of_range)
							throw std::runtime_error(Format("number out of range on line {}", m_currentLine));
						else
							throw std::runtime_error(Format("bad number on line {}", m_currentLine));
					}
					else
					{
						std::int64_t value;
						std::from_chars_result r = std::from_chars(first, last, value, base);
						if (r.ptr == last && r.ec == std::errc{})
							return Integer{ value };
						else if (r.ec == std::errc::result_out_of_range)
							throw std::runtime_error(Format("number out of range on line {}", m_currentLine));
						else
							throw std::runtime_error(Format("bad number on line {}", m_currentLine));
					}

					break;
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
