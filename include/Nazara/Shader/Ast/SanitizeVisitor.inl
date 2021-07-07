// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline StatementPtr SanitizeVisitor::Sanitize(Statement& statement, std::string* error)
	{
		return Sanitize(statement, {}, error);
	}

	inline StatementPtr Sanitize(Statement& ast, std::string* error)
	{
		SanitizeVisitor sanitizer;
		return sanitizer.Sanitize(ast, error);
	}

	inline StatementPtr Sanitize(Statement& ast, const SanitizeVisitor::Options& options, std::string* error)
	{
		SanitizeVisitor sanitizer;
		return sanitizer.Sanitize(ast, options, error);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
