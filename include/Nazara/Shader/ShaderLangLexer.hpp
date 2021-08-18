// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_LANGLEXER_HPP
#define NAZARA_SHADER_LANGLEXER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace Nz::ShaderLang
{
	enum class TokenType
	{
#define NAZARA_SHADERLANG_TOKEN(X) X,

#include <Nazara/Shader/ShaderLangTokenList.hpp>
	};

	struct Token
	{
		unsigned int column;
		unsigned int line;
		TokenType type;
		std::variant<double, long long, std::string> data;
	};

	class BadNumber : public std::exception
	{
		using exception::exception;
	};

	class NumberOutOfRange : public std::exception
	{
		using exception::exception;
	};

	class UnrecognizedToken : public std::exception
	{
		using exception::exception;
	};

	NAZARA_SHADER_API std::vector<Token> Tokenize(const std::string_view& str);
	NAZARA_SHADER_API const char* ToString(TokenType tokenType);
	NAZARA_SHADER_API std::string ToString(const std::vector<Token>& tokens, bool pretty = true);
}

#include <Nazara/Shader/ShaderLangLexer.inl>

#endif
