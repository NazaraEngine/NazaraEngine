// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	inline Parser::Parser() :
	m_context(nullptr)
	{
	}

	inline ShaderAst::ModulePtr Parse(const std::string_view& source, const std::string& filePath)
	{
		return Parse(Tokenize(source, filePath));
	}

	inline ShaderAst::ModulePtr Parse(const std::vector<Token>& tokens)
	{
		Parser parser;
		return parser.Parse(tokens);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
