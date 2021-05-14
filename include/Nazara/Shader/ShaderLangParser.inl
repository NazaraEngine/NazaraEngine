// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	inline Parser::Parser() :
	m_context(nullptr)
	{
	}

	inline ShaderAst::StatementPtr Parse(const std::vector<Token>& tokens)
	{
		Parser parser;
		return parser.Parse(tokens);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
