// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <cassert>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	namespace
	{
		std::unordered_map<std::string, ShaderAst::BasicType> identifierToBasicType = {
			{ "bool",    ShaderAst::BasicType::Boolean },

			{ "i32",     ShaderAst::BasicType::Int1 },
			{ "vec2i32", ShaderAst::BasicType::Int2 },
			{ "vec3i32", ShaderAst::BasicType::Int3 },
			{ "vec4i32", ShaderAst::BasicType::Int4 },

			{ "f32",     ShaderAst::BasicType::Float1 },
			{ "vec2f32", ShaderAst::BasicType::Float2 },
			{ "vec3f32", ShaderAst::BasicType::Float3 },
			{ "vec4f32", ShaderAst::BasicType::Float4 },

			{ "mat4x4f32", ShaderAst::BasicType::Mat4x4 },
			{ "sampler2D", ShaderAst::BasicType::Sampler2D },
			{ "void",      ShaderAst::BasicType::Void },

			{ "u32",     ShaderAst::BasicType::UInt1 },
			{ "vec2u32", ShaderAst::BasicType::UInt3 },
			{ "vec3u32", ShaderAst::BasicType::UInt3 },
			{ "vec4u32", ShaderAst::BasicType::UInt4 },
		};
	}

	ShaderAst::StatementPtr Parser::Parse(const std::vector<Token>& tokens)
	{
		Context context;
		context.tokenCount = tokens.size();
		context.tokens = tokens.data();

		context.root = std::make_unique<ShaderAst::MultiStatement>();

		m_context = &context;

		m_context->tokenIndex = -1;

		bool reachedEndOfStream = false;
		while (!reachedEndOfStream)
		{
			const Token& nextToken = PeekNext();
			switch (nextToken.type)
			{
				case TokenType::FunctionDeclaration:
					context.root->statements.push_back(ParseFunctionDeclaration());
					break;

				case TokenType::EndOfStream:
					reachedEndOfStream = true;
					break;

				default:
					throw UnexpectedToken{};
			}
		}

		return std::move(context.root);
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

	std::vector<ShaderAst::StatementPtr> Parser::ParseFunctionBody()
	{
		return ParseStatementList();
	}

	ShaderAst::StatementPtr Parser::ParseFunctionDeclaration()
	{
		ExpectNext(TokenType::FunctionDeclaration);

		std::string functionName = ParseIdentifierAsName();

		ExpectNext(TokenType::OpenParenthesis);

		std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters;

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

		ShaderAst::ShaderExpressionType returnType = ShaderAst::BasicType::Void;
		if (PeekNext().type == TokenType::FunctionReturn)
		{
			Advance(); //< Consume ->

			returnType = ParseIdentifierAsType();
		}

		ExpectNext(TokenType::OpenCurlyBracket);

		std::vector<ShaderAst::StatementPtr> functionBody = ParseFunctionBody();

		ExpectNext(TokenType::ClosingCurlyBracket);

		return ShaderBuilder::DeclareFunction(std::move(functionName), std::move(parameters), std::move(functionBody), std::move(returnType));
	}

	ShaderAst::DeclareFunctionStatement::Parameter Parser::ParseFunctionParameter()
	{
		std::string parameterName = ParseIdentifierAsName();

		ExpectNext(TokenType::Colon);

		ShaderAst::ShaderExpressionType parameterType = ParseIdentifierAsType();

		return { parameterName, parameterType };
	}

	ShaderAst::StatementPtr Parser::ParseReturnStatement()
	{
		ExpectNext(TokenType::Return);

		ShaderAst::ExpressionPtr expr;
		if (PeekNext().type != TokenType::Semicolon)
			expr = ParseExpression();

		return ShaderBuilder::Return(std::move(expr));
	}

	ShaderAst::StatementPtr Parser::ParseStatement()
	{
		const Token& token = PeekNext();

		ShaderAst::StatementPtr statement;
		switch (token.type)
		{
			case TokenType::Let:
				statement = ParseVariableDeclaration();
				break;

			case TokenType::Return:
				statement = ParseReturnStatement();
				break;

			default:
				break;
		}

		ExpectNext(TokenType::Semicolon);

		return statement;
	}

	std::vector<ShaderAst::StatementPtr> Parser::ParseStatementList()
	{
		std::vector<ShaderAst::StatementPtr> statements;
		while (PeekNext().type != TokenType::ClosingCurlyBracket)
		{
			statements.push_back(ParseStatement());
		}

		return statements;
	}

	ShaderAst::StatementPtr Parser::ParseVariableDeclaration()
	{
		ExpectNext(TokenType::Let);

		std::string variableName = ParseIdentifierAsName();

		ExpectNext(TokenType::Colon);

		ShaderAst::ShaderExpressionType variableType = ParseIdentifierAsType();

		ShaderAst::ExpressionPtr expression;
		if (PeekNext().type == TokenType::Assign)
		{
			Advance();
			expression = ParseExpression();
		}

		return ShaderBuilder::DeclareVariable(std::move(variableName), std::move(variableType), std::move(expression));
	}

	ShaderAst::ExpressionPtr Parser::ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs)
	{
		for (;;)
		{
			const Token& currentOp = PeekNext();

			int tokenPrecedence = GetTokenPrecedence(currentOp.type);
			if (tokenPrecedence < exprPrecedence)
				return lhs;

			Advance();
			ShaderAst::ExpressionPtr rhs = ParsePrimaryExpression();

			const Token& nextOp = PeekNext();

			int nextTokenPrecedence = GetTokenPrecedence(nextOp.type);
			if (tokenPrecedence < nextTokenPrecedence)
				rhs = ParseBinOpRhs(tokenPrecedence + 1, std::move(rhs));

			ShaderAst::BinaryType binaryType;
			{
				switch (currentOp.type)
				{
					case TokenType::Plus:     binaryType = ShaderAst::BinaryType::Add; break;
					case TokenType::Minus:    binaryType = ShaderAst::BinaryType::Subtract; break;
					case TokenType::Multiply: binaryType = ShaderAst::BinaryType::Multiply; break;
					case TokenType::Divide:   binaryType = ShaderAst::BinaryType::Divide; break;
					default: throw UnexpectedToken{};
				}
			}
			

			lhs = ShaderBuilder::Binary(binaryType, std::move(lhs), std::move(rhs));
		}
	}

	ShaderAst::ExpressionPtr Parser::ParseExpression()
	{
		return ParseBinOpRhs(0, ParsePrimaryExpression());
	}

	ShaderAst::ExpressionPtr Parser::ParseIdentifier()
	{
		const Token& identifier = ExpectNext(TokenType::Identifier);

		return ShaderBuilder::Identifier(std::get<std::string>(identifier.data));
	}

	ShaderAst::ExpressionPtr Parser::ParseIntegerExpression()
	{
		const Token& integer = ExpectNext(TokenType::IntegerValue);
		return ShaderBuilder::Constant(static_cast<Nz::Int32>(std::get<long long>(integer.data)));
	}

	ShaderAst::ExpressionPtr Parser::ParseParenthesisExpression()
	{
		ExpectNext(TokenType::OpenParenthesis);
		ShaderAst::ExpressionPtr expression = ParseExpression();
		ExpectNext(TokenType::ClosingParenthesis);

		return expression;
	}

	ShaderAst::ExpressionPtr Parser::ParsePrimaryExpression()
	{
		const Token& token = PeekNext();
		switch (token.type)
		{
			case TokenType::BoolFalse: return ShaderBuilder::Constant(false);
			case TokenType::BoolTrue:  return ShaderBuilder::Constant(true);
			case TokenType::FloatingPointValue: return ShaderBuilder::Constant(float(std::get<double>(Advance().data))); //< FIXME
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

	ShaderAst::ShaderExpressionType Parser::ParseIdentifierAsType()
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
