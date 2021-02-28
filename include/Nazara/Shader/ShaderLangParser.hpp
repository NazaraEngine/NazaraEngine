// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_LANGPARSER_HPP
#define NAZARA_SHADER_LANGPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderAst.hpp>

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

			ShaderAst Parse(const std::vector<Token>& tokens);

		private:
			// Flow control
			const Token& Advance();
			void Expect(const Token& token, TokenType type);
			const Token& ExpectNext(TokenType type);
			const Token& PeekNext();

			// Statements
			ShaderNodes::StatementPtr ParseFunctionBody();
			void ParseFunctionDeclaration();
			ShaderAst::FunctionParameter ParseFunctionParameter();
			ShaderNodes::StatementPtr ParseReturnStatement();
			ShaderNodes::StatementPtr ParseStatement();
			ShaderNodes::StatementPtr ParseStatementList();

			// Expressions
			ShaderNodes::ExpressionPtr ParseBinOpRhs(int exprPrecedence, ShaderNodes::ExpressionPtr lhs);
			ShaderNodes::ExpressionPtr ParseExpression();
			ShaderNodes::ExpressionPtr ParseIdentifier();
			ShaderNodes::ExpressionPtr ParseIntegerExpression();
			ShaderNodes::ExpressionPtr ParseParenthesisExpression();
			ShaderNodes::ExpressionPtr ParsePrimaryExpression();

			std::string ParseIdentifierAsName();
			ShaderExpressionType ParseIdentifierAsType();

			static int GetTokenPrecedence(TokenType token);

			struct Context
			{
				ShaderAst result;
				std::size_t tokenCount;
				std::size_t tokenIndex = 0;
				const Token* tokens;
			};

			Context* m_context;
	};
}

#include <Nazara/Shader/ShaderLangParser.inl>

#endif
