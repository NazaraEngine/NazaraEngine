// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/TransformVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline TransformVisitor::TransformVisitor() :
	m_nextFuncIndex(0),
	m_nextVarIndex(0)
	{
	}

	inline auto TransformVisitor::FindIdentifier(const std::string_view& identifierName) const -> const Identifier*
	{
		auto it = std::find_if(m_identifiersInScope.rbegin(), m_identifiersInScope.rend(), [&](const Identifier& identifier) { return identifier.name == identifierName; });
		if (it == m_identifiersInScope.rend())
			return nullptr;

		return &*it;
	}

	inline std::size_t TransformVisitor::RegisterFunction(std::string name)
	{
		std::size_t funcIndex = m_nextFuncIndex++;
		return funcIndex;
	}


	inline std::size_t TransformVisitor::RegisterStruct(std::string name, StructDescription description)
	{
		std::size_t structIndex = m_structs.size();
		m_structs.emplace_back(std::move(description));

		m_identifiersInScope.push_back({
			std::move(name),
			Struct {
				structIndex
			}
		});

		return structIndex;
	}

	inline std::size_t TransformVisitor::RegisterVariable(std::string name)
	{
		std::size_t varIndex = m_nextVarIndex++;

		m_identifiersInScope.push_back({
			std::move(name),
			Variable {
				varIndex
			}
		});

		return varIndex;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
