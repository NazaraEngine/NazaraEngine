// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline SanitizeVisitor::SanitizeVisitor() :
	m_nextFuncIndex(0)
	{
	}

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

	inline std::size_t SanitizeVisitor::RegisterFunction(std::string name)
	{
		return m_nextFuncIndex++;
	}

	inline std::size_t SanitizeVisitor::RegisterOption(std::string name, ExpressionType type)
	{
		std::size_t optionIndex = m_options.size();
		m_options.emplace_back(std::move(type));

		m_identifiersInScope.push_back({
			std::move(name),
			Option {
				optionIndex
			}
		});

		return optionIndex;
	}

	inline std::size_t SanitizeVisitor::RegisterStruct(std::string name, StructDescription description)
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

	inline std::size_t SanitizeVisitor::RegisterVariable(std::string name, ExpressionType type)
	{
		std::size_t varIndex = m_variables.size();
		m_variables.emplace_back(std::move(type));

		m_identifiersInScope.push_back({
			std::move(name),
			Variable {
				varIndex
			}
		});

		return varIndex;
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
