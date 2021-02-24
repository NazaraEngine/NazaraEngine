// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_LANGPARSER_HPP
#define NAZARA_SHADER_LANGPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>

namespace Nz::ShaderLang
{
	class ExpectedToken : public std::exception
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

			void Parse(const std::vector<Token>& tokens);

		private:
			const Token& Advance();
			void Expect(const Token& token, TokenType type);
			void ExpectNext(TokenType type);
			void ParseFunctionBody();
			void ParseFunctionDeclaration();
			void ParseFunctionParameter();
			const Token& PeekNext();

			struct Context
			{
				std::size_t tokenCount;
				std::size_t tokenIndex = 0;
				const Token* tokens;
			};

			Context* m_context;
	};
}

#include <Nazara/Shader/ShaderLangParser.inl>

#endif
