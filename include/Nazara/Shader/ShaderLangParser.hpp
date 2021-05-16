// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_LANGPARSER_HPP
#define NAZARA_SHADER_LANGPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <filesystem>

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
			ShaderAst::ExpressionType DecodeType(const std::string& identifier);
			void EnterScope();
			const Token& Expect(const Token& token, TokenType type);
			const Token& ExpectNot(const Token& token, TokenType type);
			const Token& Expect(TokenType type);
			void LeaveScope();
			bool IsVariableInScope(const std::string_view& identifier) const;
			void RegisterVariable(std::string identifier);
			const Token& Peek(std::size_t advance = 0);

			std::vector<ShaderAst::Attribute> ParseAttributes();

			// Statements
			ShaderAst::StatementPtr ParseExternalBlock(std::vector<ShaderAst::Attribute> attributes = {});
			std::vector<ShaderAst::StatementPtr> ParseFunctionBody();
			ShaderAst::StatementPtr ParseFunctionDeclaration(std::vector<ShaderAst::Attribute> attributes = {});
			ShaderAst::DeclareFunctionStatement::Parameter ParseFunctionParameter();
			ShaderAst::StatementPtr ParseOptionDeclaration();
			ShaderAst::StatementPtr ParseStructDeclaration(std::vector<ShaderAst::Attribute> attributes = {});
			ShaderAst::StatementPtr ParseReturnStatement();
			ShaderAst::StatementPtr ParseStatement();
			std::vector<ShaderAst::StatementPtr> ParseStatementList();
			ShaderAst::StatementPtr ParseVariableDeclaration();

			// Expressions
			ShaderAst::ExpressionPtr ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs);
			ShaderAst::ExpressionPtr ParseExpression();
			ShaderAst::ExpressionPtr ParseFloatingPointExpression();
			ShaderAst::ExpressionPtr ParseIdentifier();
			ShaderAst::ExpressionPtr ParseIntegerExpression();
			std::vector<ShaderAst::ExpressionPtr> ParseParameters();
			ShaderAst::ExpressionPtr ParseParenthesisExpression();
			ShaderAst::ExpressionPtr ParsePrimaryExpression();
			ShaderAst::ExpressionPtr ParseSelectOptExpression();
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

	inline ShaderAst::StatementPtr Parse(const std::vector<Token>& tokens);
	NAZARA_SHADER_API ShaderAst::StatementPtr Parse(const std::filesystem::path& sourcePath);
}

#include <Nazara/Shader/ShaderLangParser.inl>

#endif
