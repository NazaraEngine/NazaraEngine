// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <cassert>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	namespace
	{
		std::unordered_map<std::string, ShaderNodes::BasicType> identifierToBasicType = {
			{ "bool",    ShaderNodes::BasicType::Boolean },

			{ "i32",     ShaderNodes::BasicType::Int1 },
			{ "vec2i32", ShaderNodes::BasicType::Int2 },
			{ "vec3i32", ShaderNodes::BasicType::Int3 },
			{ "vec4i32", ShaderNodes::BasicType::Int4 },

			{ "f32",     ShaderNodes::BasicType::Float1 },
			{ "vec2f32", ShaderNodes::BasicType::Float2 },
			{ "vec3f32", ShaderNodes::BasicType::Float3 },
			{ "vec4f32", ShaderNodes::BasicType::Float4 },

			{ "mat4x4f32", ShaderNodes::BasicType::Mat4x4 },
			{ "sampler2D", ShaderNodes::BasicType::Sampler2D },
			{ "void",      ShaderNodes::BasicType::Void },

			{ "u32",     ShaderNodes::BasicType::UInt1 },
			{ "vec2u32", ShaderNodes::BasicType::UInt3 },
			{ "vec3u32", ShaderNodes::BasicType::UInt3 },
			{ "vec4u32", ShaderNodes::BasicType::UInt4 },
		};
	}

	ShaderAst Parser::Parse(const std::vector<Token>& tokens)
	{
		Context context;
		context.tokenCount = tokens.size();
		context.tokens = tokens.data();

		m_context = &context;

		m_context->tokenIndex = -1;

		bool reachedEndOfStream = false;
		while (!reachedEndOfStream)
		{
			const Token& nextToken = PeekNext();
			switch (nextToken.type)
			{
				case TokenType::FunctionDeclaration:
					ParseFunctionDeclaration();
					break;

				case TokenType::EndOfStream:
					reachedEndOfStream = true;
					break;

				default:
					throw UnexpectedToken{};
			}
		}

		return std::move(context.result);
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

	const Token& Parser::ExpectNext(TokenType type)
	{
		const Token& token = Advance();
		Expect(token, type);

		return token;
	}

	const Token& Parser::PeekNext()
	{
		assert(m_context->tokenIndex + 1 < m_context->tokenCount);
		return m_context->tokens[m_context->tokenIndex + 1];
	}

	ShaderNodes::StatementPtr Parser::ParseFunctionBody()
	{
		return ParseStatementList();
	}

	void Parser::ParseFunctionDeclaration()
	{
		ExpectNext(TokenType::FunctionDeclaration);

		std::string functionName = ParseIdentifierAsName();

		ExpectNext(TokenType::OpenParenthesis);

		std::vector<ShaderAst::FunctionParameter> parameters;

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

			parameters.push_back(ParseFunctionParameter());
			firstParameter = false;
		}

		ExpectNext(TokenType::ClosingParenthesis);

		ShaderExpressionType returnType = ShaderNodes::BasicType::Void;
		if (PeekNext().type == TokenType::FunctionReturn)
		{
			Advance(); //< Consume ->

			returnType = ParseIdentifierAsType();
		}

		ExpectNext(TokenType::OpenCurlyBracket);

		ShaderNodes::StatementPtr functionBody = ParseFunctionBody();

		ExpectNext(TokenType::ClosingCurlyBracket);

		m_context->result.AddFunction(functionName, functionBody, std::move(parameters), returnType);
	}

	ShaderAst::FunctionParameter Parser::ParseFunctionParameter()
	{
		std::string parameterName = ParseIdentifierAsName();

		ExpectNext(TokenType::Colon);

		ShaderExpressionType parameterType = ParseIdentifierAsType();

		return { parameterName, parameterType };
	}

	ShaderNodes::StatementPtr Parser::ParseReturnStatement()
	{
		ExpectNext(TokenType::Return);

		ShaderNodes::ExpressionPtr expr;
		if (PeekNext().type != TokenType::Semicolon)
			expr = ParseExpression();

		return ShaderNodes::ReturnStatement::Build(std::move(expr));
	}

	ShaderNodes::StatementPtr Parser::ParseStatement()
	{
		const Token& token = PeekNext();

		ShaderNodes::StatementPtr statement;
		switch (token.type)
		{
			case TokenType::Return:
				statement = ParseReturnStatement();
				break;

			default:
				break;
		}

		ExpectNext(TokenType::Semicolon);

		return statement;
	}

	ShaderNodes::StatementPtr Parser::ParseStatementList()
	{
		std::vector<ShaderNodes::StatementPtr> statements;
		while (PeekNext().type != TokenType::ClosingCurlyBracket)
		{
			statements.push_back(ParseStatement());
		}

		return ShaderNodes::StatementBlock::Build(std::move(statements));
	}

	ShaderNodes::ExpressionPtr Parser::ParseBinOpRhs(int exprPrecedence, ShaderNodes::ExpressionPtr lhs)
	{
		for (;;)
		{
			const Token& currentOp = PeekNext();

			int tokenPrecedence = GetTokenPrecedence(currentOp.type);
			if (tokenPrecedence < exprPrecedence)
				return lhs;

			Advance();
			ShaderNodes::ExpressionPtr rhs = ParsePrimaryExpression();

			const Token& nextOp = PeekNext();

			int nextTokenPrecedence = GetTokenPrecedence(nextOp.type);
			if (tokenPrecedence < nextTokenPrecedence)
				rhs = ParseBinOpRhs(tokenPrecedence + 1, std::move(rhs));

			ShaderNodes::BinaryType binaryType;
			{
				switch (currentOp.type)
				{
					case TokenType::Plus:     binaryType = ShaderNodes::BinaryType::Add; break;
					case TokenType::Minus:    binaryType = ShaderNodes::BinaryType::Subtract; break;
					case TokenType::Multiply: binaryType = ShaderNodes::BinaryType::Multiply; break;
					case TokenType::Divide:   binaryType = ShaderNodes::BinaryType::Divide; break;
					default: throw UnexpectedToken{};
				}
			}
			

			lhs = ShaderNodes::BinaryOp::Build(binaryType, std::move(lhs), std::move(rhs));
		}
	}

	ShaderNodes::ExpressionPtr Parser::ParseExpression()
	{
		return ParseBinOpRhs(0, ParsePrimaryExpression());
	}

	ShaderNodes::ExpressionPtr Parser::ParseIdentifier()
	{
		const Token& identifier = ExpectNext(TokenType::Identifier);

		return ShaderNodes::Identifier::Build(ShaderNodes::ParameterVariable::Build(std::get<std::string>(identifier.data), ShaderNodes::BasicType::Float3));
	}

	ShaderNodes::ExpressionPtr Parser::ParseIntegerExpression()
	{
		const Token& integer = ExpectNext(TokenType::IntegerValue);
		return ShaderNodes::Constant::Build(static_cast<Nz::Int32>(std::get<long long>(integer.data)));
	}

	ShaderNodes::ExpressionPtr Parser::ParseParenthesisExpression()
	{
		ExpectNext(TokenType::OpenParenthesis);
		ShaderNodes::ExpressionPtr expression = ParseExpression();
		ExpectNext(TokenType::ClosingParenthesis);

		return expression;
	}

	ShaderNodes::ExpressionPtr Parser::ParsePrimaryExpression()
	{
		const Token& token = PeekNext();
		switch (token.type)
		{
			case TokenType::BoolFalse: return ShaderNodes::Constant::Build(false);
			case TokenType::BoolTrue:  return ShaderNodes::Constant::Build(true);
			case TokenType::Identifier: return ParseIdentifier();
			case TokenType::IntegerValue: return ParseIntegerExpression();
			case TokenType::OpenParenthesis: return ParseParenthesisExpression();
			default: throw UnexpectedToken{};
		}
	}

	std::string Parser::ParseIdentifierAsName()
	{
		const Token& identifierToken = ExpectNext(TokenType::Identifier);

		std::string identifier = std::get<std::string>(identifierToken.data);

		auto it = identifierToBasicType.find(identifier);
		if (it != identifierToBasicType.end())
			throw ReservedKeyword{};

		return identifier;
	}

	ShaderExpressionType Parser::ParseIdentifierAsType()
	{
		const Token& identifier = ExpectNext(TokenType::Identifier);

		auto it = identifierToBasicType.find(std::get<std::string>(identifier.data));
		if (it == identifierToBasicType.end())
			throw UnknownType{};

		return it->second;
	}

	int Parser::GetTokenPrecedence(TokenType token)
	{
		switch (token)
		{
			case TokenType::Plus: return 20;
			case TokenType::Divide: return 40;
			case TokenType::Multiply: return 40;
			case TokenType::Minus: return 20;
			default: return -1;
		}
	}
}
