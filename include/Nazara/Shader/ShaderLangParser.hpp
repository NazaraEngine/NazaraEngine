// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERLANGPARSER_HPP
#define NAZARA_SHADER_SHADERLANGPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/Ast/Module.hpp>
#include <filesystem>
#include <optional>

namespace Nz::ShaderLang
{
	class AttributeError : public std::runtime_error
	{
		public:
			using runtime_error::runtime_error;
	};
	
	class ExpectedToken : public std::exception
	{
		public:
			using exception::exception;
	};
	
	class DuplicateIdentifier : public std::runtime_error
	{
		public:
			using runtime_error::runtime_error;
	};

	class DuplicateModule : public std::runtime_error
	{
		public:
			using runtime_error::runtime_error;
	};

	class ReservedKeyword : public std::exception
	{
		public:
			using exception::exception;
	};
	
	class UnknownAttribute : public std::exception
	{
		public:
			using exception::exception;
	};

	class UnknownType : public std::exception
	{
		public:
			using exception::exception;
	};

	class UnexpectedToken : public std::exception
	{
		public:
			using exception::exception;
	};

	class NAZARA_SHADER_API Parser
	{
		public:
			inline Parser();
			~Parser() = default;

			ShaderAst::ModulePtr Parse(const std::vector<Token>& tokens);

		private:
			// Flow control
			const Token& Advance();
			void Consume(std::size_t count = 1);
			const Token& Expect(const Token& token, TokenType type);
			const Token& ExpectNot(const Token& token, TokenType type);
			const Token& Expect(TokenType type);
			const Token& Peek(std::size_t advance = 0);

			std::vector<ShaderAst::ExprValue> ParseAttributes();
			void ParseModuleStatement(std::vector<ShaderAst::ExprValue> attributes);
			void ParseVariableDeclaration(std::string& name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType>& type, ShaderAst::ExpressionPtr& initialValue);

			ShaderAst::ExpressionPtr BuildIdentifierAccess(ShaderAst::ExpressionPtr lhs, ShaderAst::ExpressionPtr rhs);
			ShaderAst::ExpressionPtr BuildIndexAccess(ShaderAst::ExpressionPtr lhs, ShaderAst::ExpressionPtr rhs);
			ShaderAst::ExpressionPtr BuildBinary(ShaderAst::BinaryType binaryType, ShaderAst::ExpressionPtr lhs, ShaderAst::ExpressionPtr rhs);

			// Statements
			ShaderAst::StatementPtr ParseAliasDeclaration();
			ShaderAst::StatementPtr ParseBranchStatement();
			ShaderAst::StatementPtr ParseConstStatement();
			ShaderAst::StatementPtr ParseDiscardStatement();
			ShaderAst::StatementPtr ParseExternalBlock(std::vector<ShaderAst::ExprValue> attributes = {});
			ShaderAst::StatementPtr ParseForDeclaration(std::vector<ShaderAst::ExprValue> attributes = {});
			std::vector<ShaderAst::StatementPtr> ParseFunctionBody();
			ShaderAst::StatementPtr ParseFunctionDeclaration(std::vector<ShaderAst::ExprValue> attributes = {});
			ShaderAst::DeclareFunctionStatement::Parameter ParseFunctionParameter();
			ShaderAst::StatementPtr ParseImportStatement();
			ShaderAst::StatementPtr ParseOptionDeclaration();
			ShaderAst::StatementPtr ParseReturnStatement();
			ShaderAst::StatementPtr ParseRootStatement(std::vector<ShaderAst::ExprValue> attributes = {});
			ShaderAst::StatementPtr ParseSingleStatement();
			ShaderAst::StatementPtr ParseStatement();
			std::vector<ShaderAst::StatementPtr> ParseStatementList();
			ShaderAst::StatementPtr ParseStructDeclaration(std::vector<ShaderAst::ExprValue> attributes = {});
			ShaderAst::StatementPtr ParseVariableDeclaration();
			ShaderAst::StatementPtr ParseWhileStatement(std::vector<ShaderAst::ExprValue> attributes);

			// Expressions
			ShaderAst::ExpressionPtr ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs);
			ShaderAst::ExpressionPtr ParseConstSelectExpression();
			ShaderAst::ExpressionPtr ParseExpression();
			std::vector<ShaderAst::ExpressionPtr> ParseExpressionList(TokenType terminationToken);
			ShaderAst::ExpressionPtr ParseFloatingPointExpression();
			ShaderAst::ExpressionPtr ParseIdentifier();
			ShaderAst::ExpressionPtr ParseIntegerExpression();
			ShaderAst::ExpressionPtr ParseParenthesisExpression();
			ShaderAst::ExpressionPtr ParsePrimaryExpression();
			ShaderAst::ExpressionPtr ParseStringExpression();
			ShaderAst::ExpressionPtr ParseVariableAssignation();

			ShaderAst::AttributeType ParseIdentifierAsAttributeType();
			const std::string& ParseIdentifierAsName();
			std::string ParseModuleName();
			ShaderAst::ExpressionPtr ParseType();

			static int GetTokenPrecedence(TokenType token);

			struct Context
			{
				std::size_t tokenCount;
				std::size_t tokenIndex = 0;
				ShaderAst::ModulePtr module;
				const Token* tokens;
				bool parsingImportedModule = false;
			};

			Context* m_context;
	};

	inline ShaderAst::ModulePtr Parse(const std::string_view& source);
	inline ShaderAst::ModulePtr Parse(const std::vector<Token>& tokens);
	NAZARA_SHADER_API ShaderAst::ModulePtr ParseFromFile(const std::filesystem::path& sourcePath);
}

#include <Nazara/Shader/ShaderLangParser.inl>

#endif // NAZARA_SHADER_SHADERLANGPARSER_HPP
