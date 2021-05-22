// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline StatementPtr SanitizeVisitor::Sanitize(const StatementPtr& statement, std::string* error)
	{
		return Sanitize(statement, {}, error);
	}

	inline auto SanitizeVisitor::FindIdentifier(const std::string_view& identifierName) const -> const Identifier*
	{
		auto it = std::find_if(m_identifiersInScope.rbegin(), m_identifiersInScope.rend(), [&](const Identifier& identifier) { return identifier.name == identifierName; });
		if (it == m_identifiersInScope.rend())
			return nullptr;

		return &*it;
	}

	inline StatementPtr Sanitize(const StatementPtr& ast, std::string* error)
	{
		SanitizeVisitor sanitizer;
		return sanitizer.Sanitize(ast, error);
	}

	inline StatementPtr Sanitize(const StatementPtr& ast, const SanitizeVisitor::Options& options, std::string* error)
	{
		SanitizeVisitor sanitizer;
		return sanitizer.Sanitize(ast, options, error);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
