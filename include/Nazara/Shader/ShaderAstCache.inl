// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstCache.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline auto AstCache::FindIdentifier(std::size_t startingScopeId, const std::string& identifierName) const -> const Identifier*
	{
		assert(startingScopeId < scopes.size());

		std::optional<std::size_t> scopeId = startingScopeId;
		do
		{
			const auto& scope = scopes[*scopeId];
			auto it = std::find_if(scope.identifiers.rbegin(), scope.identifiers.rend(), [&](const auto& identifier) { return identifier.name == identifierName; });
			if (it != scope.identifiers.rend())
				return &*it;

			scopeId = scope.parentScopeIndex;
		} while (scopeId);

		return nullptr;
	}

	inline std::size_t AstCache::GetScopeId(const Node* node) const
	{
		auto it = scopeIdByNode.find(node);
		assert(it == scopeIdByNode.end());

		return it->second;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
