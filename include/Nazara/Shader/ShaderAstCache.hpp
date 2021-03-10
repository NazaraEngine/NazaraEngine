// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTCACHE_HPP
#define NAZARA_SHADERASTCACHE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz::ShaderAst
{
	struct AstCache
	{
		struct Variable
		{
			ShaderExpressionType type;
		};

		struct Identifier
		{
			std::string name;
			std::variant<Variable, StructDescription> value;
		};

		struct Scope
		{
			std::optional<std::size_t> parentScopeIndex;
			std::vector<Identifier> identifiers;
		};

		inline const Identifier* FindIdentifier(std::size_t startingScopeId, const std::string& identifierName) const;
		inline std::size_t GetScopeId(const Node* node) const;

		ShaderStageType stageType = ShaderStageType::Undefined;
		std::unordered_map<const Expression*, ShaderExpressionType> nodeExpressionType;
		std::unordered_map<const Node*, std::size_t> scopeIdByNode;
		std::vector<Scope> scopes;
	};
}

#include <Nazara/Shader/ShaderAstCache.inl>

#endif
