// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstScopedVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline auto AstScopedVisitor::FindIdentifier(const std::string_view& identifierName) const -> const Identifier*
	{
		auto it = std::find_if(m_identifiersInScope.rbegin(), m_identifiersInScope.rend(), [&](const Identifier& identifier) { return identifier.name == identifierName; });
		if (it == m_identifiersInScope.rend())
			return nullptr;

		return &*it;
	}

	inline void AstScopedVisitor::RegisterStruct(StructDescription structDesc)
	{
		std::string name = structDesc.name;

		m_identifiersInScope.push_back({
			std::move(name),
			std::move(structDesc)
		});
	}

	inline void AstScopedVisitor::RegisterVariable(std::string name, ExpressionType type)
	{
		m_identifiersInScope.push_back({
			std::move(name),
			Variable { std::move(type) }
		});
	}
}

#include <Nazara/Shader/DebugOff.hpp>
