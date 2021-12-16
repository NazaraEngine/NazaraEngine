// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERLANGPARSER_HPP
#define NAZARA_SHADER_SHADERLANGPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
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

			ShaderAst::StatementPtr Parse(const std::vector<Token>& tokens);

		private:
			// Flow control
			const Token& Advance();
			void Consume(std::size_t count = 1);
			std::optional<ShaderAst::ExpressionType> DecodeType(const std::string& identifier);
			void EnterScope();
			const Token& Expect(const Token& token, TokenType type);
			const Token& ExpectNot(const Token& token, TokenType type);
			const Token& Expect(TokenType type);
			void LeaveScope();
			bool IsVariableInScope(const std::string_view& identifier) const;
			void RegisterVariable(std::string identifier);
			const Token& Peek(std::size_t advance = 0);

			std::vector<ShaderAst::Attribute> ParseAttributes();
			void ParseVariableDeclaration(std::string& name, ShaderAst::ExpressionType& type, ShaderAst::ExpressionPtr& initialValue);

			// Statements
			ShaderAst::StatementPtr ParseBranchStatement();
			ShaderAst::StatementPtr ParseConstStatement();
			ShaderAst::StatementPtr ParseDiscardStatement();
			ShaderAst::StatementPtr ParseExternalBlock(std::vector<ShaderAst::Attribute> attributes = {});
			std::vector<ShaderAst::StatementPtr> ParseFunctionBody();
			ShaderAst::StatementPtr ParseFunctionDeclaration(std::vector<ShaderAst::Attribute> attributes = {});
			ShaderAst::DeclareFunctionStatement::Parameter ParseFunctionParameter();
			ShaderAst::StatementPtr ParseOptionDeclaration();
			ShaderAst::StatementPtr ParseReturnStatement();
			ShaderAst::StatementPtr ParseSingleStatement();
			ShaderAst::StatementPtr ParseStatement();
			std::vector<ShaderAst::StatementPtr> ParseStatementList();
			ShaderAst::StatementPtr ParseStructDeclaration(std::vector<ShaderAst::Attribute> attributes = {});
			ShaderAst::StatementPtr ParseVariableDeclaration();
			ShaderAst::StatementPtr ParseWhileStatement();

			// Expressions
			ShaderAst::ExpressionPtr ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs);
			ShaderAst::ExpressionPtr ParseConstSelectExpression();
			ShaderAst::ExpressionPtr ParseExpression();
			ShaderAst::ExpressionPtr ParseFloatingPointExpression();
			ShaderAst::ExpressionPtr ParseIdentifier();
			ShaderAst::ExpressionPtr ParseIntegerExpression();
			std::vector<ShaderAst::ExpressionPtr> ParseParameters();
			ShaderAst::ExpressionPtr ParseParenthesisExpression();
			ShaderAst::ExpressionPtr ParsePrimaryExpression();
			ShaderAst::ExpressionPtr ParseVariableAssignation();

			ShaderAst::AttributeType ParseIdentifierAsAttributeType();
			const std::string& ParseIdentifierAsName();
			ShaderAst::PrimitiveType ParsePrimitiveType();
			ShaderAst::ExpressionType ParseType();

			static int GetTokenPrecedence(TokenType token);

			struct Context
			{
				std::size_t tokenCount;
				std::size_t tokenIndex = 0;
				std::vector<std::size_t> scopeSizes;
				std::vector<std::string> identifiersInScope;
				std::unique_ptr<ShaderAst::MultiStatement> root;
				const Token* tokens;
			};

			Context* m_context;
	};

	inline ShaderAst::StatementPtr Parse(const std::string_view& source);
	inline ShaderAst::StatementPtr Parse(const std::vector<Token>& tokens);
	NAZARA_SHADER_API ShaderAst::StatementPtr Parse(const std::filesystem::path& sourcePath);
}

#include <Nazara/Shader/ShaderLangParser.inl>

#endif // NAZARA_SHADER_SHADERLANGPARSER_HPP
