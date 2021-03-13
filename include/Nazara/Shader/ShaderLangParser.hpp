// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_LANGPARSER_HPP
#define NAZARA_SHADER_LANGPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderNodes.hpp>

namespace Nz::ShaderLang
{
	class ExpectedToken : public std::exception
	{
		public:
			using exception::exception;
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
			const Token& Expect(const Token& token, TokenType type);
			const Token& ExpectNot(const Token& token, TokenType type);
			const Token& Expect(TokenType type);
			const Token& Peek(std::size_t advance = 0);

			void HandleAttributes();

			// Statements
			std::vector<ShaderAst::StatementPtr> ParseFunctionBody();
			ShaderAst::StatementPtr ParseFunctionDeclaration(std::vector<ShaderAst::Attribute> attributes = {});
			ShaderAst::DeclareFunctionStatement::Parameter ParseFunctionParameter();
			ShaderAst::StatementPtr ParseReturnStatement();
			ShaderAst::StatementPtr ParseStatement();
			std::vector<ShaderAst::StatementPtr> ParseStatementList();
			ShaderAst::StatementPtr ParseVariableDeclaration();

			// Expressions
			ShaderAst::ExpressionPtr ParseBinOpRhs(int exprPrecedence, ShaderAst::ExpressionPtr lhs);
			ShaderAst::ExpressionPtr ParseExpression();
			ShaderAst::ExpressionPtr ParseIdentifier();
			ShaderAst::ExpressionPtr ParseIntegerExpression();
			ShaderAst::ExpressionPtr ParseParenthesisExpression();
			ShaderAst::ExpressionPtr ParsePrimaryExpression();

			ShaderAst::AttributeType ParseIdentifierAsAttributeType();
			const std::string& ParseIdentifierAsName();
			ShaderAst::ShaderExpressionType ParseIdentifierAsType();

			static int GetTokenPrecedence(TokenType token);

			struct Context
			{
				std::unique_ptr<ShaderAst::MultiStatement> root;
				std::size_t tokenCount;
				std::size_t tokenIndex = 0;
				const Token* tokens;
			};

			Context* m_context;
	};
}

#include <Nazara/Shader/ShaderLangParser.inl>

#endif
