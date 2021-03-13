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

		std::unordered_map<std::string, ShaderAst::AttributeType> identifierToAttributeType = {
			{ "entry",  ShaderAst::AttributeType::Entry },
			{ "layout", ShaderAst::AttributeType::Layout },
		};
	}

	ShaderAst::StatementPtr Parser::Parse(const std::vector<Token>& tokens)
	{
		Context context;
		context.tokenCount = tokens.size();
		context.tokens = tokens.data();

		context.root = std::make_unique<ShaderAst::MultiStatement>();

		m_context = &context;

		bool reachedEndOfStream = false;
		while (!reachedEndOfStream)
		{
			const Token& nextToken = Peek();
			switch (nextToken.type)
			{
				case TokenType::OpenAttribute:
					HandleAttributes();
					break;

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
		const Token& token = Peek();
		m_context->tokenIndex++;

		return token;
	}

	void Parser::Consume(std::size_t count)
	{
		assert(m_context->tokenIndex + count < m_context->tokenCount);
		m_context->tokenIndex += count;
	}

	const Token& Parser::Expect(const Token& token, TokenType type)
	{
		if (token.type != type)
			throw ExpectedToken{};

		return token;
	}

	const Token& Parser::ExpectNot(const Token& token, TokenType type)
	{
		if (token.type == type)
			throw ExpectedToken{};

		return token;
	}

	const Token& Parser::Expect(TokenType type)
	{
		const Token& token = Peek();
		Expect(token, type);

		return token;
	}

	const Token& Parser::Peek(std::size_t advance)
	{
		assert(m_context->tokenIndex + advance < m_context->tokenCount);
		return m_context->tokens[m_context->tokenIndex + advance];
	}

	void Parser::HandleAttributes()
	{
		std::vector<ShaderAst::Attribute> attributes;

		Expect(Advance(), TokenType::OpenAttribute);

		bool expectComma = false;
		for (;;)
		{
			const Token& t = Peek();
			ExpectNot(t, TokenType::EndOfStream);

			if (t.type == TokenType::ClosingAttribute)
			{
				// Parse [[attribute1]] [[attribute2]] the same as [[attribute1, attribute2]]
				if (Peek(1).type == TokenType::OpenAttribute)
				{
					Consume(2);
					expectComma = false;
					continue;
				}

				break;
			}

			if (expectComma)
				Expect(Advance(), TokenType::Comma);

			ShaderAst::AttributeType attributeType = ParseIdentifierAsAttributeType();

			std::string arg;
			if (Peek().type == TokenType::OpenParenthesis)
			{
				Consume();

				if (Peek().type == TokenType::Identifier)
					arg = std::get<std::string>(Advance().data);

				Expect(Advance(), TokenType::ClosingParenthesis);
			}

			expectComma = true;

			attributes.push_back({
				attributeType,
				std::move(arg)
			});
		}

		Expect(Advance(), TokenType::ClosingAttribute);

		const Token& nextToken = Peek();
		switch (nextToken.type)
		{
			case TokenType::FunctionDeclaration:
				m_context->root->statements.push_back(ParseFunctionDeclaration(std::move(attributes)));
				break;

			default:
				throw UnexpectedToken{};
		}
	}

	std::vector<ShaderAst::StatementPtr> Parser::ParseFunctionBody()
	{
		return ParseStatementList();
	}

	ShaderAst::StatementPtr Parser::ParseFunctionDeclaration(std::vector<ShaderAst::Attribute> attributes)
	{
		Expect(Advance(), TokenType::FunctionDeclaration);

		std::string functionName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::OpenParenthesis);

		std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters;

		bool firstParameter = true;
		for (;;)
		{
			const Token& t = Peek();
			ExpectNot(t, TokenType::EndOfStream);

			if (t.type == TokenType::ClosingParenthesis)
				break;

			if (!firstParameter)
				Expect(Advance(), TokenType::Comma);

			parameters.push_back(ParseFunctionParameter());
			firstParameter = false;
		}

		Expect(Advance(), TokenType::ClosingParenthesis);

		ShaderAst::ShaderExpressionType returnType = ShaderAst::BasicType::Void;
		if (Peek().type == TokenType::FunctionReturn)
		{
			Consume();
			returnType = ParseIdentifierAsType();
		}

		Expect(Advance(), TokenType::OpenCurlyBracket);

		std::vector<ShaderAst::StatementPtr> functionBody = ParseFunctionBody();

		Expect(Advance(), TokenType::ClosingCurlyBracket);

		return ShaderBuilder::DeclareFunction(std::move(attributes), std::move(functionName), std::move(parameters), std::move(functionBody), std::move(returnType));
	}

	ShaderAst::DeclareFunctionStatement::Parameter Parser::ParseFunctionParameter()
	{
		std::string parameterName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Colon);

		ShaderAst::ShaderExpressionType parameterType = ParseIdentifierAsType();

		return { parameterName, parameterType };
	}

	ShaderAst::StatementPtr Parser::ParseReturnStatement()
	{
		Expect(Advance(), TokenType::Return);

		ShaderAst::ExpressionPtr expr;
		if (Peek().type != TokenType::Semicolon)
			expr = ParseExpression();

		return ShaderBuilder::Return(std::move(expr));
	}

	ShaderAst::StatementPtr Parser::ParseStatement()
	{
		const Token& token = Peek();

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

		Expect(Advance(), TokenType::Semicolon);

		return statement;
	}

	std::vector<ShaderAst::StatementPtr> Parser::ParseStatementList()
	{
		std::vector<ShaderAst::StatementPtr> statements;
		while (Peek().type != TokenType::ClosingCurlyBracket)
		{
			ExpectNot(Peek(), TokenType::EndOfStream);
			statements.push_back(ParseStatement());
		}

		return statements;
	}

	ShaderAst::StatementPtr Parser::ParseVariableDeclaration()
	{
		Expect(Advance(), TokenType::Let);

		std::string variableName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Colon);

		ShaderAst::ShaderExpressionType variableType = ParseIdentifierAsType();

		ShaderAst::ExpressionPtr expression;
		if (Peek().type == TokenType::Assign)
		{
			Consume();
			expression = ParseExpression();
		}

		return ShaderBuilder::DeclareVariable(std::move(variableName), std::move(variableType), std::move(expression));
	}

	ShaderAst::ExpressionPtr Parser::ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs)
	{
		for (;;)
		{
			const Token& currentOp = Peek();
			ExpectNot(currentOp, TokenType::EndOfStream);

			int tokenPrecedence = GetTokenPrecedence(currentOp.type);
			if (tokenPrecedence < exprPrecedence)
				return lhs;

			Consume();
			ShaderAst::ExpressionPtr rhs = ParsePrimaryExpression();

			const Token& nextOp = Peek();

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
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		return ShaderBuilder::Identifier(identifier);
	}

	ShaderAst::ExpressionPtr Parser::ParseIntegerExpression()
	{
		const Token& integerToken = Expect(Advance(), TokenType::Identifier);
		return ShaderBuilder::Constant(static_cast<Nz::Int32>(std::get<long long>(integerToken.data)));
	}

	ShaderAst::ExpressionPtr Parser::ParseParenthesisExpression()
	{
		Expect(Advance(), TokenType::OpenParenthesis);
		ShaderAst::ExpressionPtr expression = ParseExpression();
		Expect(Advance(), TokenType::ClosingParenthesis);

		return expression;
	}

	ShaderAst::ExpressionPtr Parser::ParsePrimaryExpression()
	{
		const Token& token = Peek();
		switch (token.type)
		{
			case TokenType::BoolFalse:
				Consume();
				return ShaderBuilder::Constant(false);

			case TokenType::BoolTrue:
				Consume();
				return ShaderBuilder::Constant(true);

			case TokenType::FloatingPointValue:
				Consume();
				return ShaderBuilder::Constant(float(std::get<double>(token.data))); //< FIXME

			case TokenType::Identifier:
				return ParseIdentifier();

			case TokenType::IntegerValue:
				return ParseIntegerExpression();

			case TokenType::OpenParenthesis:
				return ParseParenthesisExpression();

			default:
				throw UnexpectedToken{};
		}
	}

	ShaderAst::AttributeType Parser::ParseIdentifierAsAttributeType()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = identifierToAttributeType.find(identifier);
		if (it == identifierToAttributeType.end())
			throw UnknownAttribute{};

		return it->second;
	}

	const std::string& Parser::ParseIdentifierAsName()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = identifierToBasicType.find(identifier);
		if (it != identifierToBasicType.end())
			throw ReservedKeyword{};

		return identifier;
	}

	ShaderAst::ShaderExpressionType Parser::ParseIdentifierAsType()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = identifierToBasicType.find(identifier);
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
