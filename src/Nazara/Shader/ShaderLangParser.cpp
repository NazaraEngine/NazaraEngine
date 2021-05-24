// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <cassert>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	namespace
	{
		std::unordered_map<std::string, ShaderAst::PrimitiveType> s_identifierToBasicType = {
			{ "bool", ShaderAst::PrimitiveType::Boolean },
			{ "i32",  ShaderAst::PrimitiveType::Int32 },
			{ "f32",  ShaderAst::PrimitiveType::Float32 },
			{ "u32",  ShaderAst::PrimitiveType::UInt32 }
		};

		std::unordered_map<std::string, ShaderAst::AttributeType> s_identifierToAttributeType = {
			{ "binding",  ShaderAst::AttributeType::Binding  },
			{ "builtin",  ShaderAst::AttributeType::Builtin  },
			{ "entry",    ShaderAst::AttributeType::Entry    },
			{ "layout",   ShaderAst::AttributeType::Layout   },
			{ "location", ShaderAst::AttributeType::Location },
			{ "opt",      ShaderAst::AttributeType::Option   },
		};

		std::unordered_map<std::string, ShaderStageType> s_entryPoints = {
			{ "frag", ShaderStageType::Fragment },
			{ "vert", ShaderStageType::Vertex },
		};

		std::unordered_map<std::string, ShaderAst::BuiltinEntry> s_builtinMapping = {
			{ "position", ShaderAst::BuiltinEntry::VertexPosition }
		};

		std::unordered_map<std::string, StructLayout> s_layoutMapping = {
			{ "std140", StructLayout::Std140 }
		};

		template<typename T, typename U>
		std::optional<T> BoundCast(U val)
		{
			if (val < std::numeric_limits<T>::min() || val > std::numeric_limits<T>::max())
				return std::nullopt;

			return static_cast<T>(val);
		}
	}

	ShaderAst::StatementPtr Parser::Parse(const std::vector<Token>& tokens)
	{
		Context context;
		context.tokenCount = tokens.size();
		context.tokens = tokens.data();

		context.root = std::make_unique<ShaderAst::MultiStatement>();

		m_context = &context;

		std::vector<ShaderAst::Attribute> attributes;

		EnterScope();

		bool reachedEndOfStream = false;
		while (!reachedEndOfStream)
		{
			const Token& nextToken = Peek();
			switch (nextToken.type)
			{
				case TokenType::EndOfStream:
					if (!attributes.empty())
						throw UnexpectedToken{};

					reachedEndOfStream = true;
					break;

				case TokenType::External:
					context.root->statements.push_back(ParseExternalBlock(std::move(attributes)));
					attributes.clear();
					break;

				case TokenType::OpenSquareBracket:
					assert(attributes.empty());
					attributes = ParseAttributes();
					break;

				case TokenType::Option:
					if (!attributes.empty())
						throw UnexpectedToken{};

					context.root->statements.push_back(ParseOptionDeclaration());
					break;

				case TokenType::FunctionDeclaration:
					context.root->statements.push_back(ParseFunctionDeclaration(std::move(attributes)));
					attributes.clear();
					break;

				case TokenType::Struct:
					context.root->statements.push_back(ParseStructDeclaration(std::move(attributes)));
					attributes.clear();
					break;

				default:
					throw UnexpectedToken{};
			}
		}

		LeaveScope();

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

	std::optional<ShaderAst::ExpressionType> Parser::DecodeType(const std::string& identifier)
	{
		if (auto it = s_identifierToBasicType.find(identifier); it != s_identifierToBasicType.end())
		{
			Consume();
			return it->second;
		}

		//FIXME: Handle this better
		if (identifier == "mat4")
		{
			Consume();

			ShaderAst::MatrixType matrixType;
			matrixType.columnCount = 4;
			matrixType.rowCount = 4;

			Expect(Advance(), TokenType::LessThan); //< '<'
			matrixType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreatherThan); //< '>'

			return matrixType;
		}
		else if (identifier == "sampler2D")
		{
			Consume();

			ShaderAst::SamplerType samplerType;
			samplerType.dim = ImageType::E2D;

			Expect(Advance(), TokenType::LessThan); //< '<'
			samplerType.sampledType = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreatherThan); //< '>'

			return samplerType;
		}
		else if (identifier == "uniform")
		{
			Consume();

			ShaderAst::UniformType uniformType;

			Expect(Advance(), TokenType::LessThan); //< '<'
			uniformType.containedType = ShaderAst::IdentifierType{ ParseIdentifierAsName() };
			Expect(Advance(), TokenType::GreatherThan); //< '>'

			return uniformType;
		}
		else if (identifier == "vec2")
		{
			Consume();

			ShaderAst::VectorType vectorType;
			vectorType.componentCount = 2;

			Expect(Advance(), TokenType::LessThan); //< '<'
			vectorType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreatherThan); //< '>'

			return vectorType;
		}
		else if (identifier == "vec3")
		{
			Consume();

			ShaderAst::VectorType vectorType;
			vectorType.componentCount = 3;

			Expect(Advance(), TokenType::LessThan); //< '<'
			vectorType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreatherThan); //< '>'

			return vectorType;
		}
		else if (identifier == "vec4")
		{
			Consume();

			ShaderAst::VectorType vectorType;
			vectorType.componentCount = 4;

			Expect(Advance(), TokenType::LessThan); //< '<'
			vectorType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreatherThan); //< '>'

			return vectorType;
		}
		else
			return std::nullopt;
	}

	void Parser::EnterScope()
	{
		m_context->scopeSizes.push_back(m_context->identifiersInScope.size());
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

	void Parser::LeaveScope()
	{
		assert(!m_context->scopeSizes.empty());
		m_context->identifiersInScope.resize(m_context->scopeSizes.back());
		m_context->scopeSizes.pop_back();
	}

	bool Parser::IsVariableInScope(const std::string_view& identifier) const
	{
		return std::find(m_context->identifiersInScope.rbegin(), m_context->identifiersInScope.rend(), identifier) != m_context->identifiersInScope.rend();
	}

	void Parser::RegisterVariable(std::string identifier)
	{
		if (IsVariableInScope(identifier))
			throw DuplicateIdentifier{ ("identifier name " + identifier + " is already taken").c_str() };

		assert(!m_context->scopeSizes.empty());
		m_context->identifiersInScope.push_back(std::move(identifier));
	}

	const Token& Parser::Peek(std::size_t advance)
	{
		assert(m_context->tokenIndex + advance < m_context->tokenCount);
		return m_context->tokens[m_context->tokenIndex + advance];
	}

	std::vector<ShaderAst::Attribute> Parser::ParseAttributes()
	{
		std::vector<ShaderAst::Attribute> attributes;

		Expect(Advance(), TokenType::OpenSquareBracket);

		bool expectComma = false;
		for (;;)
		{
			const Token& t = Peek();
			ExpectNot(t, TokenType::EndOfStream);

			if (t.type == TokenType::ClosingSquareBracket)
			{
				// Parse [attribute1] [attribute2] the same as [attribute1, attribute2]
				if (Peek(1).type == TokenType::OpenSquareBracket)
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

			ShaderAst::Attribute::Param arg;
			if (Peek().type == TokenType::OpenParenthesis)
			{
				Consume();

				const Token& n = Peek();
				if (n.type == TokenType::Identifier)
				{
					arg = std::get<std::string>(n.data);
					Consume();
				}
				else if (n.type == TokenType::IntegerValue)
				{
					arg = std::get<long long>(n.data);
					Consume();
				}

				Expect(Advance(), TokenType::ClosingParenthesis);
			}

			expectComma = true;

			attributes.push_back({
				attributeType,
				std::move(arg)
			});
		}

		Expect(Advance(), TokenType::ClosingSquareBracket);

		return attributes;
	}

	ShaderAst::StatementPtr Parser::ParseExternalBlock(std::vector<ShaderAst::Attribute> attributes)
	{
		if (!attributes.empty())
			throw AttributeError{ "unhandled attribute for external block" };

		Expect(Advance(), TokenType::External);
		Expect(Advance(), TokenType::OpenCurlyBracket);

		std::unique_ptr<ShaderAst::DeclareExternalStatement> externalStatement = std::make_unique<ShaderAst::DeclareExternalStatement>();

		bool first = true;
		for (;;)
		{
			if (!first)
			{
				const Token& nextToken = Peek();
				if (nextToken.type == TokenType::Comma)
					Consume();
				else
				{
					Expect(nextToken, TokenType::ClosingCurlyBracket);
					break;
				}
			}

			first = false;

			const Token& token = Peek();
			if (token.type == TokenType::ClosingCurlyBracket)
				break;

			auto& extVar = externalStatement->externalVars.emplace_back();

			if (token.type == TokenType::OpenSquareBracket)
			{
				for (const auto& [attributeType, arg] : ParseAttributes())
				{
					switch (attributeType)
					{
						case ShaderAst::AttributeType::Binding:
						{
							if (extVar.bindingIndex)
								throw AttributeError{ "attribute binding must be present once" };

							if (!std::holds_alternative<long long>(arg))
								throw AttributeError{ "attribute binding requires a string parameter" };

							std::optional<unsigned int> bindingIndex = BoundCast<unsigned int>(std::get<long long>(arg));
							if (!bindingIndex)
								throw AttributeError{ "invalid binding index" };

							extVar.bindingIndex = bindingIndex.value();
							break;
						}

						default:
							throw AttributeError{ "unhandled attribute for external variable" };
					}
				}
			}

			extVar.name = ParseIdentifierAsName();
			Expect(Advance(), TokenType::Colon);
			extVar.type = ParseType();

			RegisterVariable(extVar.name);
		}

		Expect(Advance(), TokenType::ClosingCurlyBracket);

		return externalStatement;
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

		ShaderAst::ExpressionType returnType;
		if (Peek().type == TokenType::FunctionReturn)
		{
			Consume();
			returnType = ParseType();
		}

		Expect(Advance(), TokenType::OpenCurlyBracket);

		EnterScope();
		for (const auto& parameter : parameters)
			RegisterVariable(parameter.name);

		std::vector<ShaderAst::StatementPtr> functionBody = ParseFunctionBody();

		LeaveScope();

		Expect(Advance(), TokenType::ClosingCurlyBracket);

		auto func = ShaderBuilder::DeclareFunction(std::move(functionName), std::move(parameters), std::move(functionBody), std::move(returnType));

		for (const auto& [attributeType, arg] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Entry:
				{
					if (func->entryStage)
						throw AttributeError{ "attribute entry must be present once" };

					if (!std::holds_alternative<std::string>(arg))
						throw AttributeError{ "attribute entry requires a string parameter" };

					const std::string& argStr = std::get<std::string>(arg);

					auto it = s_entryPoints.find(argStr);
					if (it == s_entryPoints.end())
						throw AttributeError{ ("invalid parameter " + argStr + " for entry attribute").c_str() };

					func->entryStage = it->second;
					break;
				}

				case ShaderAst::AttributeType::Option:
				{
					if (!func->optionName.empty())
						throw AttributeError{ "attribute option must be present once" };

					if (!std::holds_alternative<std::string>(arg))
						throw AttributeError{ "attribute option requires a string parameter" };

					func->optionName = std::get<std::string>(arg);
					break;
				}

				default:
					throw AttributeError{ "unhandled attribute for function" };
			}
		}

		return func;
	}

	ShaderAst::DeclareFunctionStatement::Parameter Parser::ParseFunctionParameter()
	{
		std::string parameterName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Colon);

		ShaderAst::ExpressionType parameterType = ParseType();

		return { parameterName, parameterType };
	}

	ShaderAst::StatementPtr Parser::ParseOptionDeclaration()
	{
		Expect(Advance(), TokenType::Option);

		std::string optionName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Colon);

		ShaderAst::ExpressionType optionType = ParseType();

		ShaderAst::ExpressionPtr initialValue;
		if (Peek().type == TokenType::Assign)
		{
			Consume();

			initialValue = ParseExpression();
		}

		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::DeclareOption(std::move(optionName), std::move(optionType), std::move(initialValue));
	}

	ShaderAst::StatementPtr Parser::ParseStructDeclaration(std::vector<ShaderAst::Attribute> attributes)
	{
		Expect(Advance(), TokenType::Struct);

		ShaderAst::StructDescription description;
		description.name = ParseIdentifierAsName();

		for (const auto& [attributeType, attributeParam] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Layout:
				{
					if (description.layout)
						throw AttributeError{ "attribute layout must be present once" };

					auto it = s_layoutMapping.find(std::get<std::string>(attributeParam));
					if (it == s_layoutMapping.end())
						throw AttributeError{ "unknown layout" };

					description.layout = it->second;
					break;
				}

				default:
					throw AttributeError{ "unexpected attribute" };
			}
		}

		Expect(Advance(), TokenType::OpenCurlyBracket);

		bool first = true;

		for (;;)
		{
			if (!first)
			{
				const Token& nextToken = Peek();
				if (nextToken.type == TokenType::Comma)
					Consume();
				else
				{
					Expect(nextToken, TokenType::ClosingCurlyBracket);
					break;
				}
			}

			first = false;

			const Token& token = Peek();
			if (token.type == TokenType::ClosingCurlyBracket)
				break;

			auto& structField = description.members.emplace_back();

			if (token.type == TokenType::OpenSquareBracket)
			{
				for (const auto& [attributeType, attributeParam] : ParseAttributes())
				{
					switch (attributeType)
					{
						case ShaderAst::AttributeType::Builtin:
						{
							if (structField.builtin)
								throw AttributeError{ "attribute builtin must be present once" };

							auto it = s_builtinMapping.find(std::get<std::string>(attributeParam));
							if (it == s_builtinMapping.end())
								throw AttributeError{ "unknown builtin" };

							structField.builtin = it->second;
							break;
						}

						case ShaderAst::AttributeType::Location:
						{
							if (structField.locationIndex)
								throw AttributeError{ "attribute location must be present once" };

							structField.locationIndex = BoundCast<unsigned int>(std::get<long long>(attributeParam));
							if (!structField.locationIndex)
								throw AttributeError{ "invalid location index" };

							break;
						}

						default:
							throw AttributeError{ "unexpected attribute" };
					}
				}

				if (structField.builtin && structField.locationIndex)
					throw AttributeError{ "A struct field cannot have both builtin and location attributes" };
			}

			structField.name = ParseIdentifierAsName();

			Expect(Advance(), TokenType::Colon);

			structField.type = ParseType();
		}

		Expect(Advance(), TokenType::ClosingCurlyBracket);

		return ShaderBuilder::DeclareStruct(std::move(description));
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

			case TokenType::Identifier:
				statement = ShaderBuilder::ExpressionStatement(ParseVariableAssignation());
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

	ShaderAst::ExpressionPtr Parser::ParseVariableAssignation()
	{
		ShaderAst::ExpressionPtr left = ParseExpression();
		Expect(Advance(), TokenType::Assign);

		ShaderAst::ExpressionPtr right = ParseExpression();

		return ShaderBuilder::Assign(ShaderAst::AssignType::Simple, std::move(left), std::move(right));
	}

	ShaderAst::StatementPtr Parser::ParseVariableDeclaration()
	{
		Expect(Advance(), TokenType::Let);

		std::string variableName = ParseIdentifierAsName();
		RegisterVariable(variableName);

		ShaderAst::ExpressionType variableType = ShaderAst::NoType{};
		if (Peek().type == TokenType::Colon)
		{
			Expect(Advance(), TokenType::Colon);

			variableType = ParseType();
		}

		ShaderAst::ExpressionPtr expression;
		if (IsNoType(variableType) || Peek().type == TokenType::Assign)
		{
			Expect(Advance(), TokenType::Assign);
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

			if (currentOp.type == TokenType::Dot)
			{
				std::unique_ptr<ShaderAst::AccessMemberIdentifierExpression> accessMemberNode = std::make_unique<ShaderAst::AccessMemberIdentifierExpression>();
				accessMemberNode->structExpr = std::move(lhs);

				do
				{
					Consume();

					accessMemberNode->memberIdentifiers.push_back(ParseIdentifierAsName());
				}
				while (Peek().type == TokenType::Dot);

				// FIXME
				if (!accessMemberNode->memberIdentifiers.empty() && accessMemberNode->memberIdentifiers.front() == "Sample")
				{
					if (Peek().type == TokenType::OpenParenthesis)
					{
						auto parameters = ParseParameters();
						parameters.insert(parameters.begin(), std::move(accessMemberNode->structExpr));

						lhs = ShaderBuilder::Intrinsic(ShaderAst::IntrinsicType::SampleTexture, std::move(parameters));
						continue;
					}
				}

				lhs = std::move(accessMemberNode);
				continue;
			}

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

	ShaderAst::ExpressionPtr Parser::ParseFloatingPointExpression()
	{
		const Token& floatingPointToken = Expect(Advance(), TokenType::FloatingPointValue);
		return ShaderBuilder::Constant(float(std::get<double>(floatingPointToken.data))); //< FIXME
	}

	ShaderAst::ExpressionPtr Parser::ParseIdentifier()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		return ShaderBuilder::Identifier(identifier);
	}

	ShaderAst::ExpressionPtr Parser::ParseIntegerExpression()
	{
		const Token& integerToken = Expect(Advance(), TokenType::IntegerValue);
		return ShaderBuilder::Constant(static_cast<Nz::Int32>(std::get<long long>(integerToken.data))); //< FIXME
	}

	std::vector<ShaderAst::ExpressionPtr> Parser::ParseParameters()
	{
		Expect(Advance(), TokenType::OpenParenthesis);

		std::vector<ShaderAst::ExpressionPtr> parameters;
		bool first = true;
		while (Peek().type != TokenType::ClosingParenthesis)
		{
			if (!first)
				Expect(Advance(), TokenType::Comma);

			first = false;
			parameters.push_back(ParseExpression());
		}

		Expect(Advance(), TokenType::ClosingParenthesis);

		return parameters;
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
				return ParseFloatingPointExpression();

			case TokenType::Identifier:
			{
				const std::string& identifier = std::get<std::string>(token.data);

				// Is it a cast? 
				std::optional<ShaderAst::ExpressionType> exprType = DecodeType(identifier);
				if (exprType)
					return ShaderBuilder::Cast(std::move(*exprType), ParseParameters());

				if (Peek(1).type == TokenType::OpenParenthesis)
				{
					// Function call
					Consume();
					return ShaderBuilder::CallFunction(identifier, ParseParameters());
				}
				else
					return ParseIdentifier();
			}

			case TokenType::IntegerValue:
				return ParseIntegerExpression();

			case TokenType::Minus:
			{
				Consume();
				ShaderAst::ExpressionPtr expr = ParsePrimaryExpression();

				return ShaderBuilder::Unary(ShaderAst::UnaryType::Minus, std::move(expr));
			}

			case TokenType::Plus:
			{
				Consume();
				ShaderAst::ExpressionPtr expr = ParsePrimaryExpression();

				return ShaderBuilder::Unary(ShaderAst::UnaryType::Plus, std::move(expr));
			}

			case TokenType::OpenParenthesis:
				return ParseParenthesisExpression();

			case TokenType::SelectOpt:
				return ParseSelectOptExpression();

			default:
				throw UnexpectedToken{};
		}
	}

	ShaderAst::ExpressionPtr Parser::ParseSelectOptExpression()
	{
		Expect(Advance(), TokenType::SelectOpt);
		Expect(Advance(), TokenType::OpenParenthesis);

		std::string optionName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Comma);

		ShaderAst::ExpressionPtr trueExpr = ParseExpression();

		Expect(Advance(), TokenType::Comma);

		ShaderAst::ExpressionPtr falseExpr = ParseExpression();

		Expect(Advance(), TokenType::ClosingParenthesis);

		return ShaderBuilder::SelectOption(std::move(optionName), std::move(trueExpr), std::move(falseExpr));
	}

	ShaderAst::AttributeType Parser::ParseIdentifierAsAttributeType()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = s_identifierToAttributeType.find(identifier);
		if (it == s_identifierToAttributeType.end())
			throw UnknownAttribute{};

		return it->second;
	}

	const std::string& Parser::ParseIdentifierAsName()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = s_identifierToBasicType.find(identifier);
		if (it != s_identifierToBasicType.end())
			throw ReservedKeyword{};

		return identifier;
	}

	ShaderAst::PrimitiveType Parser::ParsePrimitiveType()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = s_identifierToBasicType.find(identifier);
		if (it == s_identifierToBasicType.end())
			throw UnknownType{};

		return it->second;
	}

	ShaderAst::ExpressionType Parser::ParseType()
	{
		// Handle () as no type
		if (Peek().type == TokenType::OpenParenthesis)
		{
			Consume();
			Expect(Advance(), TokenType::ClosingParenthesis);

			return ShaderAst::NoType{};
		}

		const Token& identifierToken = Expect(Peek(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto type = DecodeType(identifier);
		if (!type)
		{
			Consume();
			return ShaderAst::IdentifierType{ identifier };
		}

		return *type;
	}

	int Parser::GetTokenPrecedence(TokenType token)
	{
		switch (token)
		{
			case TokenType::Plus:     return 20;
			case TokenType::Divide:   return 40;
			case TokenType::Multiply: return 40;
			case TokenType::Minus:    return 20;
			case TokenType::Dot:      return 50;
			default: return -1;
		}
	}

	ShaderAst::StatementPtr Parse(const std::filesystem::path& sourcePath)
	{
		File file(sourcePath);
		if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
		{
			NazaraError("Failed to open \"" + sourcePath.generic_u8string() + '"');
			return {};
		}

		std::size_t length = static_cast<std::size_t>(file.GetSize());

		std::vector<Nz::UInt8> source(length);
		if (file.Read(&source[0], length) != length)
		{
			NazaraError("Failed to read program file");
			return {};
		}

		return Parse(Tokenize(std::string_view(reinterpret_cast<const char*>(source.data()), source.size())));
	}
}
