// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <cassert>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	void Parser::Parse(const std::vector<Token>& tokens)
	{
		Context context;
		context.tokenCount = tokens.size();
		context.tokens = tokens.data();

		m_context = &context;

		for (const Token& token : tokens)
		{
			switch (token.type)
			{
				case TokenType::FunctionDeclaration:
					ParseFunctionDeclaration();
					break;

				default:
					throw UnexpectedToken{};
			}
		}
	}

	const Token& Parser::Advance()
	{
		assert(m_context->tokenIndex + 1 < m_context->tokenCount);
		return m_context->tokens[++m_context->tokenIndex];
	}

	void Parser::Expect(const Token& token, TokenType type)
	{
		if (token.type != type)
			throw ExpectedToken{};
	}

	void Parser::ExpectNext(TokenType type)
	{
		Expect(m_context->tokens[m_context->tokenIndex + 1], type);
	}

	void Parser::ParseFunctionBody()
	{

	}

	void Parser::ParseFunctionDeclaration()
	{
		ExpectNext(TokenType::Identifier);

		std::string functionName = std::get<std::string>(Advance().data);

		ExpectNext(TokenType::OpenParenthesis);
		Advance();

		bool firstParameter = true;
		for (;;)
		{
			const Token& t = PeekNext();
			if (t.type == TokenType::ClosingParenthesis)
				break;

			if (!firstParameter)
			{
				Expect(t, TokenType::Comma);
				Advance();
			}

			ParseFunctionParameter();
			firstParameter = false;
		}

		ExpectNext(TokenType::ClosingParenthesis);
		Advance();

		if (PeekNext().type == TokenType::FunctionReturn)
		{
			Advance();

			std::string returnType = std::get<std::string>(Advance().data);
		}


		ExpectNext(TokenType::OpenCurlyBracket);
		Advance();

		ParseFunctionBody();

		ExpectNext(TokenType::ClosingCurlyBracket);
		Advance();
	}

	void Parser::ParseFunctionParameter()
	{
		ExpectNext(TokenType::Identifier);
		std::string parameterName = std::get<std::string>(Advance().data);

		ExpectNext(TokenType::Colon);
		Advance();

		ExpectNext(TokenType::Identifier);
		std::string parameterType = std::get<std::string>(Advance().data);
	}

	const Token& Parser::PeekNext()
	{
		assert(m_context->tokenIndex + 1 < m_context->tokenCount);
		return m_context->tokens[m_context->tokenIndex + 1];
	}
}
