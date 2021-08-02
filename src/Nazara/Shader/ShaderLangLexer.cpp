// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <cctype>
#include <charconv>
#include <locale>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	namespace
	{
		class ForceCLocale
		{
			public:
				ForceCLocale()
				{
					m_previousLocale = std::locale::global(std::locale::classic());
				}

				~ForceCLocale()
				{
					std::locale::global(m_previousLocale);
				}

			private:
				std::locale m_previousLocale;
		};
	}

	std::vector<Token> Tokenize(const std::string_view& str)
	{
		// Can't use std::from_chars for double, thanks to libc++ and libstdc++ developers for being lazy, so we have to force C locale
		ForceCLocale forceCLocale;

		std::unordered_map<std::string, TokenType> reservedKeywords = {
			{ "const",        TokenType::Const },
			{ "const_select", TokenType::ConstSelect },
			{ "discard",      TokenType::Discard },
			{ "else",         TokenType::Else },
			{ "external",     TokenType::External },
			{ "false",        TokenType::BoolFalse },
			{ "fn",           TokenType::FunctionDeclaration },
			{ "if",           TokenType::If },
			{ "let",          TokenType::Let },
			{ "option",       TokenType::Option },
			{ "return",       TokenType::Return },
			{ "struct",       TokenType::Struct },
			{ "true",         TokenType::BoolTrue }
		};

		std::size_t currentPos = 0;

		auto Peek = [&](std::size_t advance = 1) -> char
		{
			if (currentPos + advance < str.size() && str[currentPos + advance] != '\0')
				return str[currentPos + advance];
			else
				return char(-1);
		};

		auto IsAlphaNum = [&](const char c)
		{
			return std::isalnum(c) || c == '_';
		};

		unsigned int lineNumber = 0;
		std::size_t lastLineFeed = 0;
		std::vector<Token> tokens;

		for (;;)
		{
			char c = Peek(0);

			Token token;
			token.column = static_cast<unsigned int>(currentPos - lastLineFeed);
			token.line = lineNumber;

			if (c == -1)
			{
				token.type = TokenType::EndOfStream;
				tokens.push_back(std::move(token));
				break;
			}

			std::optional<TokenType> tokenType;
			switch (c)
			{
				case ' ':
				case '\t':
				case '\r':
					break; //< Ignore blank spaces

				case '\n':
				{
					lineNumber++;
					lastLineFeed = currentPos;
					break;
				}

				case '-':
				{
					if (Peek() == '>')
					{
						currentPos++;
						tokenType = TokenType::FunctionReturn;
						break;
					}

					tokenType = TokenType::Minus;
					break;
				}

				case '/':
				{
					char next = Peek();
					if (next == '/')
					{
						// Line comment
						do
						{
							currentPos++;
							next = Peek();
						}
						while (next != -1 && next != '\n');
					}
					else if (next == '*')
					{
						// Block comment
						do
						{
							currentPos++;
							next = Peek();

							if (next == '*')
							{
								currentPos++;
								if (Peek() == '/')
								{
									currentPos++;
									break;
								}
							}
							else if (next == '\n')
							{
								lineNumber++;
								lastLineFeed = currentPos + 1;
							}
						}
						while (next != -1);
					}
					else
						tokenType = TokenType::Divide;

					break;
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
					bool floatingPoint = false;

					std::size_t start = currentPos;
					char next = Peek();

					if (next == 'x' || next == 'X')
						currentPos++;

					for (;;)
					{
						next = Peek();

						if (!isdigit(next))
						{
							if (next != '.')
								break;

							if (floatingPoint)
								break;

							floatingPoint = true;
						}

						currentPos++;
					}

					if (floatingPoint)
					{
						tokenType = TokenType::FloatingPointValue;

						std::string valueStr(str.substr(start, currentPos - start + 1));

						const char* ptr = valueStr.c_str();

						char* end;
						double value = std::strtod(ptr, &end);
						if (end != &ptr[valueStr.size()])
							throw BadNumber{};

						token.data = value;
					}
					else
					{
						tokenType = TokenType::IntegerValue;

						long long value;
						std::from_chars_result r = std::from_chars(&str[start], &str[currentPos + 1], value);
						if (r.ptr != &str[currentPos + 1])
						{
							if (r.ec == std::errc::result_out_of_range)
								throw NumberOutOfRange{};

							throw BadNumber{};
						}

						token.data = value;
					}

					break;
				}

				case '=':
				{
					char next = Peek();
					if (next == '=')
					{
						currentPos++;
						tokenType = TokenType::Equal;
					}
					else
						tokenType = TokenType::Assign;

					break;
				}

				case '|':
				{
					char next = Peek();
					if (next == '|')
					{
						currentPos++;
						tokenType = TokenType::LogicalOr;
					}
					else
						throw UnrecognizedToken{}; //< TODO: Add BOR (a | b)

					break;
				}

				case '&':
				{
					char next = Peek();
					if (next == '&')
					{
						currentPos++;
						tokenType = TokenType::LogicalAnd;
					}
					else
						throw UnrecognizedToken{}; //< TODO: Add BAND (a & b)

					break;
				}

				case '<':
				{
					char next = Peek();
					if (next == '=')
					{
						currentPos++;
						tokenType = TokenType::LessThanEqual;
					}
					else
						tokenType = TokenType::LessThan;

					break;
				}

				case '>':
				{
					char next = Peek();
					if (next == '=')
					{
						currentPos++;
						tokenType = TokenType::GreaterThanEqual;
					}
					else
						tokenType = TokenType::GreaterThan;

					break;
				}

				case '!':
				{
					char next = Peek();
					if (next == '=')
					{
						currentPos++;
						tokenType = TokenType::NotEqual;
					}
					else
						tokenType = TokenType::Not;

					break;
				}


				case '+': tokenType = TokenType::Plus; break;
				case '*': tokenType = TokenType::Multiply; break;
				case ':': tokenType = TokenType::Colon; break;
				case ';': tokenType = TokenType::Semicolon; break;
				case '.': tokenType = TokenType::Dot; break;
				case ',': tokenType = TokenType::Comma; break;
				case '{': tokenType = TokenType::OpenCurlyBracket; break;
				case '}': tokenType = TokenType::ClosingCurlyBracket; break;
				case '(': tokenType = TokenType::OpenParenthesis; break;
				case ')': tokenType = TokenType::ClosingParenthesis; break;
				case '[': tokenType = TokenType::OpenSquareBracket; break;
				case ']': tokenType = TokenType::ClosingSquareBracket; break;

				default:
				{
					if (IsAlphaNum(c))
					{
						std::size_t start = currentPos;

						while (IsAlphaNum(Peek()))
							currentPos++;

						std::string identifier(str.substr(start, currentPos - start + 1));
						if (auto it = reservedKeywords.find(identifier); it == reservedKeywords.end())
						{
							tokenType = TokenType::Identifier;
							token.data = std::move(identifier);
						}
						else
							tokenType = it->second;

						break;
					}
					else
						throw UnrecognizedToken{};
				}
			}

			if (tokenType)
			{
				token.type = *tokenType;

				tokens.push_back(std::move(token));
			}

			currentPos++;
		}

		return tokens;
	}

	const char* ToString(TokenType tokenType)
	{
		switch (tokenType)
		{
#define NAZARA_SHADERLANG_TOKEN(X) case TokenType:: X: return #X;

#include <Nazara/Shader/ShaderLangTokenList.hpp>
		}

		return "<Error>";
	}

	std::string ToString(const std::vector<Token>& tokens, bool pretty)
	{
		if (tokens.empty())
			return {};

		unsigned int lastLineNumber = tokens.front().line;

		std::stringstream ss;
		bool empty = true;

		for (const Token& token : tokens)
		{
			if (token.line != lastLineNumber && pretty)
			{
				lastLineNumber = token.line;
				if (!empty)
					ss << '\n';
			}
			else if (!empty)
				ss << ' ';

			ss << ToString(token.type);
			switch (token.type)
			{
				case TokenType::FloatingPointValue:
					ss << "(" << std::get<double>(token.data) << ")";
					break;

				case TokenType::Identifier:
					ss << "(" << std::get<std::string>(token.data) << ")";
					break;

				case TokenType::IntegerValue:
					ss << "(" << std::get<long long>(token.data) << ")";
					break;

				default:
					break;
			}

			empty = false;
		}

		ss << '\n';

		return std::move(ss).str();
	}
}
