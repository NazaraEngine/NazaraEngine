// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangErrors.hpp>
#include <cassert>
#include <regex>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		std::unordered_map<std::string, ShaderAst::DepthWriteMode> s_depthWriteModes = {
			{ "greater",   ShaderAst::DepthWriteMode::Greater },
			{ "less",      ShaderAst::DepthWriteMode::Less },
			{ "replace",   ShaderAst::DepthWriteMode::Replace },
			{ "unchanged", ShaderAst::DepthWriteMode::Unchanged },
		};

		std::unordered_map<std::string, ShaderAst::AttributeType> s_identifierToAttributeType = {
			{ "binding",              ShaderAst::AttributeType::Binding },
			{ "builtin",              ShaderAst::AttributeType::Builtin },
			{ "cond",                 ShaderAst::AttributeType::Cond },
			{ "depth_write",          ShaderAst::AttributeType::DepthWrite },
			{ "early_fragment_tests", ShaderAst::AttributeType::EarlyFragmentTests },
			{ "entry",                ShaderAst::AttributeType::Entry },
			{ "export",               ShaderAst::AttributeType::Export },
			{ "layout",               ShaderAst::AttributeType::Layout },
			{ "location",             ShaderAst::AttributeType::Location },
			{ "nzsl_version",         ShaderAst::AttributeType::LangVersion },
			{ "set",                  ShaderAst::AttributeType::Set },
			{ "unroll",               ShaderAst::AttributeType::Unroll },
			{ "uuid",                 ShaderAst::AttributeType::Uuid },
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

		std::unordered_map<std::string, ShaderAst::LoopUnroll> s_unrollModes = {
			{ "always", ShaderAst::LoopUnroll::Always },
			{ "hint",   ShaderAst::LoopUnroll::Hint },
			{ "never",  ShaderAst::LoopUnroll::Never }
		};

		template<typename T>
		void HandleUniqueAttribute(const Token& token, ShaderAst::AttributeType attributeType, ShaderAst::ExpressionValue<T>& targetAttribute, ShaderAst::ExprValue::Param&& param)
		{
			if (targetAttribute.HasValue())
				throw ParserAttributeMultipleUniqueError{ SourceLocation{ token.line, token.column, token.file }, attributeType }; //< TODO: Improve source location for attributes

			if (param)
				targetAttribute = std::move(*param);
			else
				throw ParserAttributeMissingParameterError{ SourceLocation{ token.line, token.column, token.file }, attributeType }; //< TODO: Improve source location for attributes
		}

		template<typename T>
		void HandleUniqueAttribute(const Token& token, ShaderAst::AttributeType attributeType, ShaderAst::ExpressionValue<T>& targetAttribute, ShaderAst::ExprValue::Param&& param, T defaultValue)
		{
			if (targetAttribute.HasValue())
				throw ParserAttributeMultipleUniqueError{ SourceLocation{ token.line, token.column, token.file }, attributeType }; //< TODO: Improve source location for attributes

			if (param)
				targetAttribute = std::move(*param);
			else
				targetAttribute = std::move(defaultValue);
		}

		template<typename T>
		void HandleUniqueStringAttribute(const Token& token, ShaderAst::AttributeType attributeType, const std::unordered_map<std::string, T>& map, ShaderAst::ExpressionValue<T>& targetAttribute, ShaderAst::ExprValue::Param&& param, std::optional<T> defaultValue = {})
		{
			if (targetAttribute.HasValue())
				throw ParserAttributeMultipleUniqueError{ SourceLocation{ token.line, token.column, token.file }, attributeType }; //< TODO: Improve source location for attributes

			//FIXME: This should be handled with global values at sanitization stage
			if (param)
			{
				const ShaderAst::ExpressionPtr& expr = *param;
				if (expr->GetType() != ShaderAst::NodeType::IdentifierExpression)
					throw ParserAttributeParameterIdentifierError{ SourceLocation{ token.line, token.column, token.file }, attributeType };

				const std::string& exprStr = static_cast<ShaderAst::IdentifierExpression&>(*expr).identifier;

				auto it = map.find(exprStr);
				if (it == map.end())
					throw ParserAttributeInvalidParameterError{ SourceLocation{ token.line, token.column, token.file }, exprStr, attributeType };

				targetAttribute = it->second;
			}
			else
			{
				if (!defaultValue)
					throw ParserAttributeMissingParameterError{ SourceLocation{ token.line, token.column, token.file }, attributeType }; //< TODO: Improve source location for attributes

				targetAttribute = defaultValue.value();
			}
		}
	}

	ShaderAst::ModulePtr Parser::Parse(const std::vector<Token>& tokens)
	{
		Context context;
		context.tokenCount = tokens.size();
		context.tokens = tokens.data();

		m_context = &context;

		std::vector<ShaderAst::ExprValue> attributes;

		for (;;)
		{
			ShaderAst::StatementPtr statement = ParseRootStatement();
			if (!m_context->module)
			{
				const Token& nextToken = Peek();
				throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file } };
			}

			if (!statement)
				break;

			m_context->module->rootNode->statements.push_back(std::move(statement));
		}

		return std::move(context.module);
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
			throw ParserExpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, type, token.type };

		return token;
	}

	const Token& Parser::ExpectNot(const Token& token, TokenType type)
	{
		if (token.type == type)
			throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, type };

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

	std::vector<ShaderAst::ExprValue> Parser::ParseAttributes()
	{
		std::vector<ShaderAst::ExprValue> attributes;

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

			ShaderAst::ExprValue::Param arg;
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

	void Parser::ParseModuleStatement(std::vector<ShaderAst::ExprValue> attributes)
	{
		if (m_context->parsingImportedModule)
		{
			const Token& token = Peek();
			throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };
		}

		const Token& moduleToken = Expect(Advance(), TokenType::Module);

		std::optional<UInt32> moduleVersion;
		std::optional<Uuid> moduleId;
		
		for (auto&& [attributeType, arg] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::LangVersion:
				{
					// Version parsing
					if (moduleVersion.has_value())
						throw ParserAttributeMultipleUniqueError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType }; //< TODO: Improve source location for attributes

					if (!arg)
						throw ParserAttributeMissingParameterError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType }; //< TODO: Improve source location for attributes

					const ShaderAst::ExpressionPtr& expr = *arg;
					if (expr->GetType() != ShaderAst::NodeType::ConstantValueExpression)
						throw ParserAttributeExpectStringError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType };

					auto& constantValue = SafeCast<ShaderAst::ConstantValueExpression&>(*expr);
					if (ShaderAst::GetConstantType(constantValue.value) != ShaderAst::ExpressionType{ ShaderAst::PrimitiveType::String })
						throw ParserAttributeExpectStringError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType };

					const std::string& versionStr = std::get<std::string>(constantValue.value);

					std::regex versionRegex(R"(^(\d+)(\.(\d+)(\.(\d+))?)?$)", std::regex::ECMAScript);

					std::smatch versionMatch;
					if (!std::regex_match(versionStr, versionMatch, versionRegex))
						throw ParserInvalidVersionError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, versionStr };

					assert(versionMatch.size() == 6);

					std::uint32_t version = 0;
					version += std::stoi(versionMatch[1]) * 100;

					if (versionMatch.length(3) > 0)
						version += std::stoi(versionMatch[3]) * 10;

					if (versionMatch.length(5) > 0)
						version += std::stoi(versionMatch[5]) * 1;

					moduleVersion = version;
					break;
				}

				case ShaderAst::AttributeType::Uuid:
				{
					if (moduleId.has_value())
						throw ParserAttributeMultipleUniqueError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType }; //< TODO: Improve source location for attributes

					if (!arg)
						throw ParserAttributeMissingParameterError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType }; //< TODO: Improve source location for attributes

					const ShaderAst::ExpressionPtr& expr = *arg;
					if (expr->GetType() != ShaderAst::NodeType::ConstantValueExpression)
						throw ParserAttributeExpectStringError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType };

					auto& constantValue = SafeCast<ShaderAst::ConstantValueExpression&>(*expr);
					if (ShaderAst::GetConstantType(constantValue.value) != ShaderAst::ExpressionType{ ShaderAst::PrimitiveType::String })
						throw ParserAttributeExpectStringError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType };

					const std::string& uuidStr = std::get<std::string>(constantValue.value);

					Uuid uuid = Uuid::FromString(uuidStr);
					if (uuid.IsNull())
						throw ParserInvalidUuidError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, uuidStr };

					moduleId = uuid;
					break;
				}

				default:
					throw ParserUnexpectedAttributeError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file }, attributeType };
			}
		}

		if (!moduleVersion.has_value())
			throw ParserMissingAttributeError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file } };

		if (!moduleId)
			moduleId = Uuid::Generate();

		if (m_context->module)
		{
			std::string moduleName = ParseModuleName();
			auto module = std::make_shared<ShaderAst::Module>(*moduleVersion, std::move(moduleName), *moduleId);

			// Imported module
			Expect(Advance(), TokenType::OpenCurlyBracket);

			m_context->parsingImportedModule = true;

			while (Peek().type != TokenType::ClosingCurlyBracket)
			{
				ShaderAst::StatementPtr statement = ParseRootStatement();
				if (!statement)
				{
					const Token& token = Peek();
					throw ParserUnexpectedEndOfFileError{ SourceLocation{ token.line, token.column, token.file } };
				}

				module->rootNode->statements.push_back(std::move(statement));
			}
			Consume(); //< Consume ClosingCurlyBracket

			m_context->parsingImportedModule = false;

			auto& importedModule = m_context->module->importedModules.emplace_back();
			importedModule.module = std::move(module);
			importedModule.identifier = importedModule.module->metadata->moduleName;
		}
		else
		{
			std::string moduleName;
			if (Peek().type == TokenType::Identifier)
				moduleName = ParseModuleName();

			auto module = std::make_shared<ShaderAst::Module>(*moduleVersion, std::move(moduleName), *moduleId);

			// First declaration
			Expect(Advance(), TokenType::Semicolon);

			if (m_context->module)
				throw ParserDuplicateModuleError{ SourceLocation{ moduleToken.line, moduleToken.column, moduleToken.file } };

			m_context->module = std::move(module);
		}
	}

	void Parser::ParseVariableDeclaration(std::string& name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType>& type, ShaderAst::ExpressionPtr& initialValue)
	{
		name = ParseIdentifierAsName();

		if (Peek().type == TokenType::Colon)
		{
			Expect(Advance(), TokenType::Colon);

			type = ParseType();
		}

		if (!type.HasValue() || Peek().type == TokenType::Assign)
		{
			Expect(Advance(), TokenType::Assign);
			initialValue = ParseExpression();
		}

		Expect(Advance(), TokenType::Semicolon);
	}

	ShaderAst::ExpressionPtr Parser::BuildIdentifierAccess(ShaderAst::ExpressionPtr lhs, ShaderAst::ExpressionPtr rhs)
	{
		if (rhs->GetType() == ShaderAst::NodeType::IdentifierExpression)
			return ShaderBuilder::AccessMember(std::move(lhs), { std::move(SafeCast<ShaderAst::IdentifierExpression&>(*rhs).identifier) });
		else
			return BuildIndexAccess(std::move(lhs), std::move(rhs));
	}

	ShaderAst::ExpressionPtr Parser::BuildIndexAccess(ShaderAst::ExpressionPtr lhs, ShaderAst::ExpressionPtr rhs)
	{
		return ShaderBuilder::AccessIndex(std::move(lhs), std::move(rhs));
	}

	ShaderAst::ExpressionPtr Parser::BuildBinary(ShaderAst::BinaryType binaryType, ShaderAst::ExpressionPtr lhs, ShaderAst::ExpressionPtr rhs)
	{
		return ShaderBuilder::Binary(binaryType, std::move(lhs), std::move(rhs));
	}

	ShaderAst::StatementPtr Parser::ParseAliasDeclaration()
	{
		Expect(Advance(), TokenType::Alias);

		std::string name = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Assign);

		ShaderAst::ExpressionPtr expr = ParseExpression();

		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::DeclareAlias(std::move(name), std::move(expr));
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
		{
			Consume();
			branch->elseStatement = ParseStatement();
		}

		return branch;
	}

	ShaderAst::StatementPtr Parser::ParseConstStatement()
	{
		Expect(Advance(), TokenType::Const);

		const Token& token = Peek();
		switch (token.type)
		{
			case TokenType::Identifier:
			{
				std::string constName;
				ShaderAst::ExpressionValue<ShaderAst::ExpressionType> constType;
				ShaderAst::ExpressionPtr initialValue;

				ParseVariableDeclaration(constName, constType, initialValue);

				return ShaderBuilder::DeclareConst(std::move(constName), std::move(constType), std::move(initialValue));
			}

			case TokenType::If:
			{
				auto branch = ParseBranchStatement();
				SafeCast<ShaderAst::BranchStatement&>(*branch).isConst = true;

				return branch;
			}

			default:
				throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };
		}
	}

	ShaderAst::StatementPtr Parser::ParseDiscardStatement()
	{
		Expect(Advance(), TokenType::Discard);
		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::Discard();
	}

	ShaderAst::StatementPtr Parser::ParseExternalBlock(std::vector<ShaderAst::ExprValue> attributes)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		const Token& externalToken = Expect(Advance(), TokenType::External);
		Expect(Advance(), TokenType::OpenCurlyBracket);

		std::unique_ptr<ShaderAst::DeclareExternalStatement> externalStatement = std::make_unique<ShaderAst::DeclareExternalStatement>();

		ShaderAst::ExpressionValue<bool> condition;

		for (auto&& [attributeType, arg] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Cond:
					HandleUniqueAttribute(externalToken, attributeType, condition, std::move(arg));
					break;

				case ShaderAst::AttributeType::Set:
					HandleUniqueAttribute(externalToken, attributeType, externalStatement->bindingSet, std::move(arg));
					break;

				default:
					throw ParserUnexpectedAttributeError{ SourceLocation{ externalToken.line, externalToken.column, externalToken.file }, attributeType };
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
							HandleUniqueAttribute(externalToken, attributeType, extVar.bindingIndex, std::move(arg));
							break;

						case ShaderAst::AttributeType::Set:
							HandleUniqueAttribute(externalToken, attributeType, extVar.bindingSet, std::move(arg));
							break;

						default:
							throw ParserUnexpectedAttributeError{ SourceLocation{ token.line, token.column, token.file }, attributeType };
					}
				}
			}

			extVar.name = ParseIdentifierAsName();
			Expect(Advance(), TokenType::Colon);
			extVar.type = ParseType();
		}

		Expect(Advance(), TokenType::ClosingCurlyBracket);

		if (condition.HasValue())
			return ShaderBuilder::ConditionalStatement(std::move(condition).GetExpression(), std::move(externalStatement));
		else
			return externalStatement;
	}

	ShaderAst::StatementPtr Parser::ParseForDeclaration(std::vector<ShaderAst::ExprValue> attributes)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		const Token& forToken = Expect(Advance(), TokenType::For);

		std::string varName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::In);

		ShaderAst::ExpressionPtr expr = ParseExpression();

		if (Peek().type == TokenType::Arrow)
		{
			// Numerical for
			Consume();

			ShaderAst::ExpressionPtr toExpr = ParseExpression();

			ShaderAst::ExpressionPtr stepExpr;
			if (Peek().type == TokenType::Colon)
			{
				Consume();
				stepExpr = ParseExpression();
			}

			ShaderAst::StatementPtr statement = ParseStatement();

			auto forNode = ShaderBuilder::For(std::move(varName), std::move(expr), std::move(toExpr), std::move(stepExpr), std::move(statement));

			// TODO: Deduplicate code
			for (auto&& [attributeType, arg] : attributes)
			{
				switch (attributeType)
				{
					case ShaderAst::AttributeType::Unroll:
						HandleUniqueStringAttribute(forToken, attributeType, s_unrollModes, forNode->unroll, std::move(arg), std::make_optional(ShaderAst::LoopUnroll::Always));
						break;

					default:
						throw ParserUnexpectedAttributeError{ SourceLocation{ forToken.line, forToken.column, forToken.file }, attributeType };
				}
			}

			return forNode;
		}
		else
		{
			// For each
			ShaderAst::StatementPtr statement = ParseStatement();

			auto forEachNode = ShaderBuilder::ForEach(std::move(varName), std::move(expr), std::move(statement));

			// TODO: Deduplicate code
			for (auto&& [attributeType, arg] : attributes)
			{
				switch (attributeType)
				{
					case ShaderAst::AttributeType::Unroll:
						HandleUniqueStringAttribute(forToken, attributeType, s_unrollModes, forEachNode->unroll, std::move(arg), std::make_optional(ShaderAst::LoopUnroll::Always));
						break;

					default:
						throw ParserUnexpectedAttributeError{ SourceLocation{ forToken.line, forToken.column, forToken.file }, attributeType };
				}
			}

			return forEachNode;
		}
	}

	std::vector<ShaderAst::StatementPtr> Parser::ParseFunctionBody()
	{
		return ParseStatementList();
	}

	ShaderAst::StatementPtr Parser::ParseFunctionDeclaration(std::vector<ShaderAst::ExprValue> attributes)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		const auto& funcToken = Expect(Advance(), TokenType::FunctionDeclaration);

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

		ShaderAst::ExpressionValue<ShaderAst::ExpressionType> returnType;
		if (Peek().type == TokenType::Arrow)
		{
			Consume();
			returnType = ParseType();
		}

		std::vector<ShaderAst::StatementPtr> functionBody = ParseFunctionBody();

		auto func = ShaderBuilder::DeclareFunction(std::move(functionName), std::move(parameters), std::move(functionBody), std::move(returnType));

		ShaderAst::ExpressionValue<bool> condition;

		for (auto&& [attributeType, attributeParam] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Cond:
					HandleUniqueAttribute(funcToken, attributeType, condition, std::move(attributeParam));
					break;

				case ShaderAst::AttributeType::Entry:
					HandleUniqueStringAttribute(funcToken, attributeType, s_entryPoints, func->entryStage, std::move(attributeParam));
					break;

				case ShaderAst::AttributeType::Export:
					HandleUniqueAttribute(funcToken, attributeType, func->isExported, std::move(attributeParam), true);
					break;

				case ShaderAst::AttributeType::DepthWrite:
					HandleUniqueStringAttribute(funcToken, attributeType, s_depthWriteModes, func->depthWrite, std::move(attributeParam));
					break;

				case ShaderAst::AttributeType::EarlyFragmentTests:
					HandleUniqueAttribute(funcToken, attributeType, func->earlyFragmentTests, std::move(attributeParam), true);
					break;

				default:
					throw ParserUnexpectedAttributeError{ SourceLocation{ funcToken.line, funcToken.column, funcToken.file }, attributeType };
			}
		}

		if (condition.HasValue())
			return ShaderBuilder::ConditionalStatement(std::move(condition).GetExpression(), std::move(func));
		else
			return func;
	}

	ShaderAst::DeclareFunctionStatement::Parameter Parser::ParseFunctionParameter()
	{
		ShaderAst::DeclareFunctionStatement::Parameter parameter;

		parameter.name = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Colon);

		parameter.type = ParseType();

		return parameter;
	}

	ShaderAst::StatementPtr Parser::ParseImportStatement()
	{
		Expect(Advance(), TokenType::Import);

		std::string moduleName = ParseModuleName();

		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::Import(std::move(moduleName));
	}

	ShaderAst::StatementPtr Parser::ParseOptionDeclaration()
	{
		Expect(Advance(), TokenType::Option);

		std::string optionName = ParseIdentifierAsName();

		Expect(Advance(), TokenType::Colon);

		ShaderAst::ExpressionPtr optionType = ParseType();

		ShaderAst::ExpressionPtr initialValue;
		if (Peek().type == TokenType::Assign)
		{
			Consume();

			initialValue = ParseExpression();
		}

		Expect(Advance(), TokenType::Semicolon);

		return ShaderBuilder::DeclareOption(std::move(optionName), std::move(optionType), std::move(initialValue));
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

	ShaderAst::StatementPtr Parser::ParseRootStatement(std::vector<ShaderAst::ExprValue> attributes)
	{
		const Token& nextToken = Peek();
		switch (nextToken.type)
		{
			case TokenType::Alias:
				if (!attributes.empty())
					throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file }, nextToken.type };

				return ParseAliasDeclaration();

			case TokenType::Const:
				if (!attributes.empty())
					throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file }, nextToken.type };

				return ParseConstStatement();

			case TokenType::EndOfStream:
				if (!attributes.empty())
					throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file }, nextToken.type };

				return {};

			case TokenType::External:
				return ParseExternalBlock(std::move(attributes));

			case TokenType::Import:
				if (!attributes.empty())
					throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file }, nextToken.type };

				return ParseImportStatement();

			case TokenType::OpenSquareBracket:
				assert(attributes.empty());
				return ParseRootStatement(ParseAttributes());

			case TokenType::Module:
				ParseModuleStatement(std::move(attributes));
				return ParseRootStatement();

			case TokenType::Option:
			{
				if (!attributes.empty())
					throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file }, nextToken.type };

				return ParseOptionDeclaration();
			}

			case TokenType::FunctionDeclaration:
				return ParseFunctionDeclaration(std::move(attributes));

			case TokenType::Struct:
				return ParseStructDeclaration(std::move(attributes));

			default:
				throw ParserUnexpectedTokenError{ SourceLocation{ nextToken.line, nextToken.column, nextToken.file }, nextToken.type };
		}
	}

	ShaderAst::StatementPtr Parser::ParseSingleStatement()
	{
		std::vector<ShaderAst::ExprValue> attributes;
		ShaderAst::StatementPtr statement;
		do 
		{
			const Token& token = Peek();
			switch (token.type)
			{
				case TokenType::Const:
					if (!attributes.empty())
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

					statement = ParseConstStatement();
					break;

				case TokenType::Discard:
					if (!attributes.empty())
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

					statement = ParseDiscardStatement();
					break;

				case TokenType::For:
					statement = ParseForDeclaration(std::move(attributes));
					attributes.clear();
					break;

				case TokenType::Let:
					if (!attributes.empty())
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

					statement = ParseVariableDeclaration();
					break;

				case TokenType::Identifier:
					if (!attributes.empty())
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

					statement = ShaderBuilder::ExpressionStatement(ParseVariableAssignation());
					Expect(Advance(), TokenType::Semicolon);
					break;

				case TokenType::If:
					if (!attributes.empty())
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

					statement = ParseBranchStatement();
					break;

				case TokenType::OpenSquareBracket:
					assert(attributes.empty());
					attributes = ParseAttributes();
					break;

				case TokenType::Return:
					if (!attributes.empty())
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

					statement = ParseReturnStatement();
					break;

				case TokenType::While:
					statement = ParseWhileStatement(std::move(attributes));
					attributes.clear();
					break;

				default:
					throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };
			}
		}
		while (!statement); //< small trick to repeat parsing once we got attributes

		return statement;
	}

	ShaderAst::StatementPtr Parser::ParseStatement()
	{
		if (Peek().type == TokenType::OpenCurlyBracket)
			return ShaderBuilder::Scoped(ShaderBuilder::MultiStatement(ParseStatementList()));
		else
			return ParseSingleStatement();
	}

	std::vector<ShaderAst::StatementPtr> Parser::ParseStatementList()
	{
		Expect(Advance(), TokenType::OpenCurlyBracket);

		std::vector<ShaderAst::StatementPtr> statements;
		while (Peek().type != TokenType::ClosingCurlyBracket)
		{
			ExpectNot(Peek(), TokenType::EndOfStream);
			statements.push_back(ParseStatement());
		}
		Consume(); //< Consume closing curly bracket

		return statements;
	}
	
	ShaderAst::StatementPtr Parser::ParseStructDeclaration(std::vector<ShaderAst::ExprValue> attributes)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		const auto& structToken = Expect(Advance(), TokenType::Struct);

		ShaderAst::StructDescription description;
		description.name = ParseIdentifierAsName();
		
		ShaderAst::ExpressionValue<bool> condition;
		ShaderAst::ExpressionValue<bool> exported;

		for (auto&& [attributeType, attributeParam] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Cond:
					HandleUniqueAttribute(structToken, attributeType, condition, std::move(attributeParam));
					break;

				case ShaderAst::AttributeType::Export:
					HandleUniqueAttribute(structToken, attributeType, exported, std::move(attributeParam), true);
					break;

				case ShaderAst::AttributeType::Layout:
					HandleUniqueStringAttribute(structToken, attributeType, s_layoutMapping, description.layout, std::move(attributeParam));
					break;

				default:
					throw ParserUnexpectedAttributeError{ SourceLocation{ structToken.line, structToken.column, structToken.file }, attributeType };
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
							HandleUniqueStringAttribute(token, attributeType, s_builtinMapping, structField.builtin, std::move(arg));
							break;

						case ShaderAst::AttributeType::Cond:
							HandleUniqueAttribute(token, attributeType, structField.cond, std::move(arg));
							break;

						case ShaderAst::AttributeType::Location:
							HandleUniqueAttribute(token, attributeType, structField.locationIndex, std::move(arg));
							break;

						default:
							throw ParserUnexpectedAttributeError{ SourceLocation{ token.line, token.column, token.file }, attributeType };
					}
				}
			}

			structField.name = ParseIdentifierAsName();

			Expect(Advance(), TokenType::Colon);

			structField.type = ParseType();
		}

		Expect(Advance(), TokenType::ClosingCurlyBracket);

		if (condition.HasValue())
			return ShaderBuilder::ConditionalStatement(std::move(condition).GetExpression(), ShaderBuilder::DeclareStruct(std::move(description), std::move(exported)));
		else
			return ShaderBuilder::DeclareStruct(std::move(description), std::move(exported));
	}

	ShaderAst::ExpressionPtr Parser::ParseVariableAssignation()
	{
		// Variable expression
		ShaderAst::ExpressionPtr left = ParseExpression();

		// Assignation type 
		ShaderAst::AssignType assignType;

		const Token& token = Peek();
		switch (token.type)
		{
			case TokenType::Assign: assignType = ShaderAst::AssignType::Simple; break;
			case TokenType::DivideAssign: assignType = ShaderAst::AssignType::CompoundDivide; break;
			case TokenType::LogicalAndAssign: assignType = ShaderAst::AssignType::CompoundLogicalAnd; break;
			case TokenType::LogicalOrAssign: assignType = ShaderAst::AssignType::CompoundLogicalOr; break;
			case TokenType::MultiplyAssign: assignType = ShaderAst::AssignType::CompoundMultiply; break;
			case TokenType::MinusAssign: assignType = ShaderAst::AssignType::CompoundSubtract; break;
			case TokenType::PlusAssign: assignType = ShaderAst::AssignType::CompoundAdd; break;

			default:
				throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file } };
		}

		Consume();

		// Value expression
		ShaderAst::ExpressionPtr right = ParseExpression();

		return ShaderBuilder::Assign(assignType, std::move(left), std::move(right));
	}

	ShaderAst::StatementPtr Parser::ParseVariableDeclaration()
	{
		Expect(Advance(), TokenType::Let);

		std::string variableName;
		ShaderAst::ExpressionValue<ShaderAst::ExpressionType> variableType;
		ShaderAst::ExpressionPtr expression;

		ParseVariableDeclaration(variableName, variableType, expression);

		return ShaderBuilder::DeclareVariable(std::move(variableName), std::move(variableType), std::move(expression));
	}

	ShaderAst::StatementPtr Parser::ParseWhileStatement(std::vector<ShaderAst::ExprValue> attributes)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		const Token& whileToken = Expect(Advance(), TokenType::While);

		Expect(Advance(), TokenType::OpenParenthesis);

		ShaderAst::ExpressionPtr condition = ParseExpression();

		Expect(Advance(), TokenType::ClosingParenthesis);

		ShaderAst::StatementPtr body = ParseStatement();

		auto whileStatement = ShaderBuilder::While(std::move(condition), std::move(body));

		for (auto&& [attributeType, arg] : attributes)
		{
			switch (attributeType)
			{
				case ShaderAst::AttributeType::Unroll:
					HandleUniqueStringAttribute(whileToken, attributeType, s_unrollModes, whileStatement->unroll, std::move(arg), std::make_optional(ShaderAst::LoopUnroll::Always));
					break;

				default:
					throw ParserUnexpectedAttributeError{ SourceLocation{ whileToken.line, whileToken.column, whileToken.file }, attributeType };
			}
		}

		return whileStatement;
	}

	ShaderAst::ExpressionPtr Parser::ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs)
	{
		for (;;)
		{
			const Token& token = Peek();
			TokenType currentTokenType = token.type;
			if (currentTokenType == TokenType::EndOfStream)
				throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };

			int tokenPrecedence = GetTokenPrecedence(currentTokenType);
			if (tokenPrecedence < exprPrecedence)
				return lhs;

			if (currentTokenType == TokenType::OpenParenthesis)
			{
				Consume();

				// Function call
				auto parameters = ParseExpressionList(TokenType::ClosingParenthesis);
				lhs = ShaderBuilder::CallFunction(std::move(lhs), std::move(parameters));
				continue;
			}

			if (currentTokenType == TokenType::OpenSquareBracket)
			{
				Consume();

				// Indices
				auto parameters = ParseExpressionList(TokenType::ClosingSquareBracket);
				lhs = ShaderBuilder::AccessIndex(std::move(lhs), std::move(parameters));
				continue;
			}

			Consume();
			ShaderAst::ExpressionPtr rhs = ParsePrimaryExpression();

			const Token& nextOp = Peek();

			int nextTokenPrecedence = GetTokenPrecedence(nextOp.type);
			if (tokenPrecedence < nextTokenPrecedence)
				rhs = ParseBinOpRhs(tokenPrecedence + 1, std::move(rhs));

			lhs = [&]
			{
				switch (currentTokenType)
				{
					case TokenType::Dot:
						return BuildIdentifierAccess(std::move(lhs), std::move(rhs));

					case TokenType::Divide:            return BuildBinary(ShaderAst::BinaryType::Divide,     std::move(lhs), std::move(rhs));
					case TokenType::Equal:             return BuildBinary(ShaderAst::BinaryType::CompEq,     std::move(lhs), std::move(rhs));
					case TokenType::LessThan:          return BuildBinary(ShaderAst::BinaryType::CompLt,     std::move(lhs), std::move(rhs));
					case TokenType::LessThanEqual:     return BuildBinary(ShaderAst::BinaryType::CompLe,     std::move(lhs), std::move(rhs));
					case TokenType::LogicalAnd:        return BuildBinary(ShaderAst::BinaryType::LogicalAnd, std::move(lhs), std::move(rhs));
					case TokenType::LogicalOr:         return BuildBinary(ShaderAst::BinaryType::LogicalOr,  std::move(lhs), std::move(rhs));
					case TokenType::GreaterThan:       return BuildBinary(ShaderAst::BinaryType::CompGt,     std::move(lhs), std::move(rhs));
					case TokenType::GreaterThanEqual:  return BuildBinary(ShaderAst::BinaryType::CompGe,     std::move(lhs), std::move(rhs));
					case TokenType::Minus:             return BuildBinary(ShaderAst::BinaryType::Subtract,   std::move(lhs), std::move(rhs));
					case TokenType::Multiply:          return BuildBinary(ShaderAst::BinaryType::Multiply,   std::move(lhs), std::move(rhs));
					case TokenType::NotEqual:          return BuildBinary(ShaderAst::BinaryType::CompNe,     std::move(lhs), std::move(rhs));
					case TokenType::Plus:              return BuildBinary(ShaderAst::BinaryType::Add,        std::move(lhs), std::move(rhs));
					default:
						throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };
				}
			}();
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

	std::vector<ShaderAst::ExpressionPtr> Parser::ParseExpressionList(TokenType terminationToken)
	{
		std::vector<ShaderAst::ExpressionPtr> parameters;
		bool first = true;
		while (Peek().type != terminationToken)
		{
			if (!first)
				Expect(Advance(), TokenType::Comma);

			first = false;
			parameters.push_back(ParseExpression());
		}

		Expect(Advance(), terminationToken);

		return parameters;
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
		return ShaderBuilder::Constant(SafeCast<Int32>(std::get<long long>(integerToken.data))); //< FIXME
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
				return ParseIdentifier();

			case TokenType::IntegerValue:
				return ParseIntegerExpression();

			case TokenType::Minus:
			{
				Consume();
				ShaderAst::ExpressionPtr expr = ParseExpression();

				return ShaderBuilder::Unary(ShaderAst::UnaryType::Minus, std::move(expr));
			}

			case TokenType::Plus:
			{
				Consume();
				ShaderAst::ExpressionPtr expr = ParseExpression();

				return ShaderBuilder::Unary(ShaderAst::UnaryType::Plus, std::move(expr));
			}

			case TokenType::Not:
			{
				Consume();
				ShaderAst::ExpressionPtr expr = ParseExpression();

				return ShaderBuilder::Unary(ShaderAst::UnaryType::LogicalNot, std::move(expr));
			}

			case TokenType::OpenParenthesis:
				return ParseParenthesisExpression();

			case TokenType::StringValue:
				return ParseStringExpression();

			default:
				throw ParserUnexpectedTokenError{ SourceLocation{ token.line, token.column, token.file }, token.type };
		}
	}

	ShaderAst::ExpressionPtr Parser::ParseStringExpression()
	{
		const Token& litteralToken = Expect(Advance(), TokenType::StringValue);
		return ShaderBuilder::Constant(std::get<std::string>(litteralToken.data));
	}

	ShaderAst::AttributeType Parser::ParseIdentifierAsAttributeType()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		const std::string& identifier = std::get<std::string>(identifierToken.data);

		auto it = s_identifierToAttributeType.find(identifier);
		if (it == s_identifierToAttributeType.end())
			throw ParserUnknownAttributeError{ SourceLocation{ identifierToken.line, identifierToken.column, identifierToken.file } };

		return it->second;
	}

	const std::string& Parser::ParseIdentifierAsName()
	{
		const Token& identifierToken = Expect(Advance(), TokenType::Identifier);
		return std::get<std::string>(identifierToken.data);
	}

	std::string Parser::ParseModuleName()
	{
		std::string moduleName = ParseIdentifierAsName();
		while (Peek().type == TokenType::Dot)
		{
			Consume();
			moduleName += '.';
			moduleName += ParseIdentifierAsName();
		}

		return moduleName;
	}

	ShaderAst::ExpressionPtr Parser::ParseType()
	{
		// Handle () as no type
		if (Peek().type == TokenType::OpenParenthesis)
		{
			Consume();
			Expect(Advance(), TokenType::ClosingParenthesis);

			return ShaderBuilder::Constant(ShaderAst::NoValue{});
		}

		return ParseExpression();
	}

	int Parser::GetTokenPrecedence(TokenType token)
	{
		switch (token)
		{
			case TokenType::Divide:            return 80;
			case TokenType::Dot:               return 150;
			case TokenType::Equal:             return 50;
			case TokenType::LessThan:          return 40;
			case TokenType::LessThanEqual:     return 40;
			case TokenType::LogicalAnd:        return 20;
			case TokenType::LogicalOr:         return 10;
			case TokenType::GreaterThan:       return 40;
			case TokenType::GreaterThanEqual:  return 40;
			case TokenType::Multiply:          return 80;
			case TokenType::Minus:             return 60;
			case TokenType::NotEqual:          return 50;
			case TokenType::Plus:              return 60;
			case TokenType::OpenSquareBracket: return 100;
			case TokenType::OpenParenthesis:   return 100;
			default: return -1;
		}
	}

	ShaderAst::ModulePtr ParseFromFile(const std::filesystem::path& sourcePath)
	{
		File file(sourcePath);
		if (!file.Open(OpenMode::ReadOnly | OpenMode::Text))
		{
			NazaraError("Failed to open \"" + sourcePath.generic_u8string() + '"');
			return {};
		}

		std::size_t length = static_cast<std::size_t>(file.GetSize());
		if (length == 0)
			return {};

		std::vector<Nz::UInt8> source(length);
		if (file.Read(&source[0], length) != length)
		{
			NazaraError("Failed to read shader file");
			return {};
		}

		return Parse(std::string_view(reinterpret_cast<const char*>(source.data()), source.size()), sourcePath.generic_u8string());
	}
}
