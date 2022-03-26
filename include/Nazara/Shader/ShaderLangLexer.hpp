// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERLANGLEXER_HPP
#define NAZARA_SHADER_SHADERLANGLEXER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <memory>
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
		std::shared_ptr<const std::string> file;
		std::variant<double, long long, std::string> data;
	};

	NAZARA_SHADER_API std::vector<Token> Tokenize(const std::string_view& str, const std::string& filePath = std::string{});
	NAZARA_SHADER_API const char* ToString(TokenType tokenType);
	NAZARA_SHADER_API std::string ToString(const std::vector<Token>& tokens, bool pretty = true);
}

#include <Nazara/Shader/ShaderLangLexer.inl>

#endif // NAZARA_SHADER_SHADERLANGLEXER_HPP
