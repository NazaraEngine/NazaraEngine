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
		std::unordered_map<std::string, ShaderAst::DepthWriteMode> s_depthWriteModes = {
			{ "greater",   ShaderAst::DepthWriteMode::Greater },
			{ "less",      ShaderAst::DepthWriteMode::Less },
			{ "replace",   ShaderAst::DepthWriteMode::Replace },
			{ "unchanged", ShaderAst::DepthWriteMode::Unchanged },
		};

		std::unordered_map<std::string, ShaderAst::PrimitiveType> s_identifierToBasicType = {
			{ "bool", ShaderAst::PrimitiveType::Boolean },
			{ "i32",  ShaderAst::PrimitiveType::Int32 },
			{ "f32",  ShaderAst::PrimitiveType::Float32 },
			{ "u32",  ShaderAst::PrimitiveType::UInt32 }
		};

		std::unordered_map<std::string, ShaderAst::AttributeType> s_identifierToAttributeType = {
			{ "binding",              ShaderAst::AttributeType::Binding },
			{ "builtin",              ShaderAst::AttributeType::Builtin },
			{ "cond",                 ShaderAst::AttributeType::Cond },
			{ "depth_write",          ShaderAst::AttributeType::DepthWrite },
			{ "early_fragment_tests", ShaderAst::AttributeType::EarlyFragmentTests },
			{ "entry",                ShaderAst::AttributeType::Entry },
			{ "layout",               ShaderAst::AttributeType::Layout },
			{ "location",             ShaderAst::AttributeType::Location },
			{ "set",                  ShaderAst::AttributeType::Set },
		};

		std::unordered_map<std::string, ShaderStageType> s_entryPoints = {
			{ "frag", ShaderStageType::Fragment },
			{ "vert", ShaderStageType::Vertex },
		};

		std::unordered_map<std::string, ShaderAst::BuiltinEntry> s_builtinMapping = {
			{ "fragcoord", ShaderAst::BuiltinEntry::FragCoord },
			{ "fragdepth", ShaderAst::BuiltinEntry::FragDepth },
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

		template<typename T>
		void HandleUniqueAttribute(const std::string_view& attributeName, ShaderAst::AttributeValue<T>& targetAttribute, ShaderAst::Attribute::Param&& param, bool requireValue = true)
		{
			if (targetAttribute.HasValue())
				throw AttributeError{ "attribute " + std::string(attributeName) + " must be present once" };

			if (!param && requireValue)
				throw AttributeError{ "attribute " + std::string(attributeName) + " requires a parameter" };

			targetAttribute = std::move(*param);
		}

		template<typename T>
		void HandleUniqueStringAttribute(const std::string_view& attributeName, const std::unordered_map<std::string, T>& map, ShaderAst::AttributeValue<T>& targetAttribute, ShaderAst::Attribute::Param&& param)
		{
			if (targetAttribute.HasValue())
				throw AttributeError{ "attribute " + std::string(attributeName) + " must be present once" };

			//FIXME: This should be handled with global values at sanitization stage
			if (!param)
				throw AttributeError{ "attribute " + std::string(attributeName) + " requires a value" };

			const ShaderAst::ExpressionPtr& expr = *param;
			if (expr->GetType() != ShaderAst::NodeType::IdentifierExpression)
				throw AttributeError{ "attribute " + std::string(attributeName) + " can only be an identifier for now" };

			const std::string& exprStr = static_cast<ShaderAst::IdentifierExpression&>(*expr).identifier;

			auto it = map.find(exprStr);
			if (it == map.end())
				throw AttributeError{ ("invalid parameter " + exprStr + " for " + std::string(attributeName) + " attribute").c_str() };

			targetAttribute = it->second;
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
				case TokenType::Const:
					if (!attributes.empty())
						throw UnexpectedToken{};

					context.root->statements.push_back(ParseConstStatement());
					break;

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
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return matrixType;
		}
		else if (identifier == "mat3")
		{
			Consume();

			ShaderAst::MatrixType matrixType;
			matrixType.columnCount = 3;
			matrixType.rowCount = 3;

			Expect(Advance(), TokenType::LessThan); //< '<'
			matrixType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return matrixType;
		}
		else if (identifier == "sampler2D")
		{
			Consume();

			ShaderAst::SamplerType samplerType;
			samplerType.dim = ImageType::E2D;

			Expect(Advance(), TokenType::LessThan); //< '<'
			samplerType.sampledType = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return samplerType;
		}
		else if (identifier == "samplerCube")
		{
			Consume();

			ShaderAst::SamplerType samplerType;
			samplerType.dim = ImageType::Cubemap;

			Expect(Advance(), TokenType::LessThan); //< '<'
			samplerType.sampledType = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return samplerType;
		}
		else if (identifier == "uniform")
		{
			Consume();

			ShaderAst::UniformType uniformType;

			Expect(Advance(), TokenType::LessThan); //< '<'
			uniformType.containedType = ShaderAst::IdentifierType{ ParseIdentifierAsName() };
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return uniformType;
		}
		else if (identifier == "vec2")
		{
			Consume();

			ShaderAst::VectorType vectorType;
			vectorType.componentCount = 2;

			Expect(Advance(), TokenType::LessThan); //< '<'
			vectorType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return vectorType;
		}
		else if (identifier == "vec3")
		{
			Consume();

			ShaderAst::VectorType vectorType;
			vectorType.componentCount = 3;

			Expect(Advance(), TokenType::LessThan); //< '<'
			vectorType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreaterThan); //< '>'

			return vectorType;
		}
		else if (identifier == "vec4")
		{
			Consume();

			ShaderAst::VectorType vectorType;
			vectorType.componentCount = 4;

			Expect(Advance(), TokenType::LessThan); //< '<'
			vectorType.type = ParsePrimitiveType();
			Expect(Advance(), TokenType::GreaterThan); //< '>'

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

				arg = ParseExpression();

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

	void Parser::ParseVariableDeclaration(std::string& name, ShaderAst::ExpressionType& type, ShaderAst::ExpressionPtr& initialValue)
	{
		name = ParseIdentifierAsName();

		if (Peek().type == TokenType::Colon)
		{
			Expect(Advance(), TokenType::Colon);

			type = ParseType();
		}
		else
			type = ShaderAst::NoType{};

		if (IsNoType(type) || Peek().type == TokenType::Assign)
		{
			Expect(Advance(), TokenType::Assign);
			initialValue = ParseExpression();
		}

		Expect(Advance(), TokenType::Semicolon);
	}

	ShaderAst::StatementPtr Parser::ParseBranchStatement()
	{
		std::unique_ptr<ShaderAst::BranchStatement> branch = std::make_unique<ShaderAst::BranchStatement>();

		bool first = true;
		for (;;)
		{
			if (!first)
				Expect(Advance(), TokenType::Else);

			first = false;

			Expect(Advance(), TokenType::If);

			auto& condStatement = branch->condStatements.emplace_back();

			Expect(Advance(), TokenType::OpenParenthesis);

			condStatement.condition = ParseExpression();

			Expect(Advance(), TokenType::ClosingParenthesis);

			condStatement.statement = ParseStatement();

			if (Peek().type != TokenType::Else || Peek(1).type != TokenType::If)
				break;
		}

		if (Peek().type == TokenType::Else)
			branch->elseStatement = ParseStatement();

		return branch;
	}

	ShaderAst::StatementPtr Parser::ParseConstStatement()
	{
		Expect(Advance(), TokenType::Const);

		switch (Peek().type)
		{
			case TokenType::Identifier:
			{
				std::string constName;
				ShaderAst::ExpressionType constType;
				ShaderAst::ExpressionPtr initialValue;

				ParseVariableDeclaration(constName, constType, initialValue);
				RegisterVariable(constName);

				return ShaderBuilder::DeclareConst(std::move(constName), std::move(constType), std::move(initialValue));
			}

			case TokenType::If:
			{
				auto branch = ParseBranchStatement();
				static_cast<ShaderAst::BranchStatement&>(*branch).isConst = true;

				return branch;
			}

			default:
				throw UnexpectedToken{};
		}
	}

	ShaderAst::StatementPtr Parser::ParseDiscardStatement()
	{
		Expect(Advance(), TokenType::Discard);
		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::Discard();
	}

	ShaderAst::StatementPtr Parser::ParseExternalBlock(std::vector<ShaderAst::Attribute> attributes)
	{
		Expect(Advance(), TokenType::External);
		Expect(Advance(), TokenType::OpenCurlyBracket);

		std::unique_ptr<ShaderAst::DeclareExternalStatement> externalStatement = std::make_unique<ShaderAst::DeclareExternalStatement>();

		for (auto&& [attributeType, arg] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Set:
					HandleUniqueAttribute("set", externalStatement->bindingSet, std::move(arg));
					break;

				default:
					throw AttributeError{ "unhandled attribute for external block" };
			}
		}

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
				for (auto&& [attributeType, arg] : ParseAttributes())
				{
					switch (attributeType)
					{
						case ShaderAst::AttributeType::Binding:
							HandleUniqueAttribute("binding", extVar.bindingIndex, std::move(arg));
							break;

						case ShaderAst::AttributeType::Set:
							HandleUniqueAttribute("set", extVar.bindingSet, std::move(arg));
							break;

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

		EnterScope();
		for (const auto& parameter : parameters)
			RegisterVariable(parameter.name);

		std::vector<ShaderAst::StatementPtr> functionBody = ParseFunctionBody();

		LeaveScope();

		auto func = ShaderBuilder::DeclareFunction(std::move(functionName), std::move(parameters), std::move(functionBody), std::move(returnType));

		ShaderAst::AttributeValue<bool> condition;

		for (auto&& [attributeType, arg] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Cond:
					HandleUniqueAttribute("cond", condition, std::move(arg));
					break;

				case ShaderAst::AttributeType::Entry:
					HandleUniqueStringAttribute("entry", s_entryPoints, func->entryStage, std::move(arg));
					break;

				case ShaderAst::AttributeType::DepthWrite:
					HandleUniqueStringAttribute("depth_write", s_depthWriteModes, func->depthWrite, std::move(arg));
					break;

				case ShaderAst::AttributeType::EarlyFragmentTests:
					HandleUniqueAttribute("early_fragment_tests", func->earlyFragmentTests, std::move(arg), false);
					break;

				default:
					throw AttributeError{ "unhandled attribute for function" };
			}
		}

		if (condition.HasValue())
			return ShaderBuilder::ConditionalStatement(std::move(condition).GetExpression(), std::move(func));
		else
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

		for (auto&& [attributeType, attributeParam] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Layout:
					HandleUniqueStringAttribute("layout", s_layoutMapping, description.layout, std::move(attributeParam));
					break;

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
				for (auto&& [attributeType, arg] : ParseAttributes())
				{
					switch (attributeType)
					{
						case ShaderAst::AttributeType::Builtin:
							HandleUniqueStringAttribute("builtin", s_builtinMapping, structField.builtin, std::move(arg));
							break;

						case ShaderAst::AttributeType::Cond:
							HandleUniqueAttribute("cond", structField.cond, std::move(arg));
							break;

						case ShaderAst::AttributeType::Location:
							HandleUniqueAttribute("location", structField.locationIndex, std::move(arg));
							break;

						default:
							throw AttributeError{ "unexpected attribute" };
					}
				}

				if (structField.builtin.HasValue() && structField.locationIndex.HasValue())
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

		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::Return(std::move(expr));
	}

	ShaderAst::StatementPtr Parser::ParseSingleStatement()
	{
		const Token& token = Peek();

		ShaderAst::StatementPtr statement;
		switch (token.type)
		{
			case TokenType::Const:
				statement = ParseConstStatement();
				break;

			case TokenType::Discard:
				statement = ParseDiscardStatement();
				break;

			case TokenType::Let:
				statement = ParseVariableDeclaration();
				break;

			case TokenType::Identifier:
				statement = ShaderBuilder::ExpressionStatement(ParseVariableAssignation());
				Expect(Advance(), TokenType::Semicolon);
				break;

			case TokenType::If:
				statement = ParseBranchStatement();
				break;

			case TokenType::Return:
				statement = ParseReturnStatement();
				break;

			default:
				break;
		}

		return statement;
	}

	ShaderAst::StatementPtr Parser::ParseStatement()
	{
		if (Peek().type == TokenType::OpenCurlyBracket)
			return ShaderBuilder::MultiStatement(ParseStatementList());
		else
			return ParseSingleStatement();
	}

	std::vector<ShaderAst::StatementPtr> Parser::ParseStatementList()
	{
		EnterScope();

		Expect(Advance(), TokenType::OpenCurlyBracket);

		std::vector<ShaderAst::StatementPtr> statements;
		while (Peek().type != TokenType::ClosingCurlyBracket)
		{
			ExpectNot(Peek(), TokenType::EndOfStream);
			statements.push_back(ParseStatement());
		}
		Consume(); //< Consume closing curly bracket

		LeaveScope();

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

		std::string variableName;
		ShaderAst::ExpressionType variableType;
		ShaderAst::ExpressionPtr expression;

		ParseVariableDeclaration(variableName, variableType, expression);
		RegisterVariable(variableName);

		return ShaderBuilder::DeclareVariable(std::move(variableName), std::move(variableType), std::move(expression));
	}

	ShaderAst::ExpressionPtr Parser::ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs)
	{
		for (;;)
		{
			TokenType currentTokenType = Peek().type;
			if (currentTokenType == TokenType::EndOfStream)
				throw UnexpectedToken{};

			int tokenPrecedence = GetTokenPrecedence(currentTokenType);
			if (tokenPrecedence < exprPrecedence)
				return lhs;

			bool c = false;
			while (currentTokenType == TokenType::Dot || currentTokenType == TokenType::OpenSquareBracket)
			{
				c = true;

				if (currentTokenType == TokenType::Dot)
				{
					std::unique_ptr<ShaderAst::AccessIdentifierExpression> accessMemberNode = std::make_unique<ShaderAst::AccessIdentifierExpression>();
					accessMemberNode->expr = std::move(lhs);

					do
					{
						Consume();

						accessMemberNode->identifiers.push_back(ParseIdentifierAsName());
					} while (Peek().type == TokenType::Dot);

					// FIXME
					if (!accessMemberNode->identifiers.empty() && accessMemberNode->identifiers.front() == "Sample")
					{
						if (Peek().type == TokenType::OpenParenthesis)
						{
							auto parameters = ParseParameters();
							parameters.insert(parameters.begin(), std::move(accessMemberNode->expr));

							lhs = ShaderBuilder::Intrinsic(ShaderAst::IntrinsicType::SampleTexture, std::move(parameters));
							break;
						}
					}

					lhs = std::move(accessMemberNode);
				}
				else
				{
					assert(currentTokenType == TokenType::OpenSquareBracket);

					std::unique_ptr<ShaderAst::AccessIndexExpression> indexNode = std::make_unique<ShaderAst::AccessIndexExpression>();
					indexNode->expr = std::move(lhs);

					do
					{
						Consume();

						indexNode->indices.push_back(ParseExpression());

						Expect(Advance(), TokenType::ClosingSquareBracket);
					}
					while (Peek().type == TokenType::OpenSquareBracket);

					lhs = std::move(indexNode);
				}

				currentTokenType = Peek().type;
			}

			if (c)
				continue;

			Consume();
			ShaderAst::ExpressionPtr rhs = ParsePrimaryExpression();

			const Token& nextOp = Peek();

			int nextTokenPrecedence = GetTokenPrecedence(nextOp.type);
			if (tokenPrecedence < nextTokenPrecedence)
				rhs = ParseBinOpRhs(tokenPrecedence + 1, std::move(rhs));

			ShaderAst::BinaryType binaryType;
			{
				switch (currentTokenType)
				{
					case TokenType::Divide:           binaryType = ShaderAst::BinaryType::Divide; break;
					case TokenType::Equal:            binaryType = ShaderAst::BinaryType::CompEq; break;
					case TokenType::LessThan:         binaryType = ShaderAst::BinaryType::CompLt; break;
					case TokenType::LessThanEqual:    binaryType = ShaderAst::BinaryType::CompLe; break;
					case TokenType::LogicalAnd:       binaryType = ShaderAst::BinaryType::LogicalAnd; break;
					case TokenType::LogicalOr:        binaryType = ShaderAst::BinaryType::LogicalOr; break;
					case TokenType::GreaterThan:      binaryType = ShaderAst::BinaryType::CompLt; break;
					case TokenType::GreaterThanEqual: binaryType = ShaderAst::BinaryType::CompLe; break;
					case TokenType::Minus:            binaryType = ShaderAst::BinaryType::Subtract; break;
					case TokenType::Multiply:         binaryType = ShaderAst::BinaryType::Multiply; break;
					case TokenType::NotEqual:         binaryType = ShaderAst::BinaryType::CompNe; break;
					case TokenType::Plus:             binaryType = ShaderAst::BinaryType::Add; break;
					default:
						throw UnexpectedToken{};
				}
			}

			lhs = ShaderBuilder::Binary(binaryType, std::move(lhs), std::move(rhs));
		}
	}

	ShaderAst::ExpressionPtr Parser::ParseConstSelectExpression()
	{
		Expect(Advance(), TokenType::ConstSelect);
		Expect(Advance(), TokenType::OpenParenthesis);

		ShaderAst::ExpressionPtr cond = ParseExpression();

		Expect(Advance(), TokenType::Comma);

		ShaderAst::ExpressionPtr trueExpr = ParseExpression();

		Expect(Advance(), TokenType::Comma);

		ShaderAst::ExpressionPtr falseExpr = ParseExpression();

		Expect(Advance(), TokenType::ClosingParenthesis);

		return ShaderBuilder::ConditionalExpression(std::move(cond), std::move(trueExpr), std::move(falseExpr));
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

			case TokenType::ConstSelect:
				return ParseConstSelectExpression();

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

			case TokenType::Not:
			{
				Consume();
				ShaderAst::ExpressionPtr expr = ParsePrimaryExpression();

				return ShaderBuilder::Unary(ShaderAst::UnaryType::LogicalNot, std::move(expr));
			}

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
			case TokenType::Divide:            return 80;
			case TokenType::Dot:               return 100;
			case TokenType::Equal:             return 50;
			case TokenType::LessThan:          return 40;
			case TokenType::LessThanEqual:     return 40;
			case TokenType::LogicalAnd:        return 120;
			case TokenType::LogicalOr:         return 140;
			case TokenType::GreaterThan:       return 40;
			case TokenType::GreaterThanEqual:  return 40;
			case TokenType::Multiply:          return 80;
			case TokenType::Minus:             return 60;
			case TokenType::NotEqual:          return 50;
			case TokenType::Plus:              return 60;
			case TokenType::OpenSquareBracket: return 100;
			default: return -1;
		}
	}

	ShaderAst::StatementPtr Parse(const std::filesystem::path& sourcePath)
	{
		File file(sourcePath);
		if (!file.Open(OpenMode::ReadOnly | OpenMode::Text))
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

		return Parse(std::string_view(reinterpret_cast<const char*>(source.data()), source.size()));
	}
}
