// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_DEPENDENCYCHECKERVISITOR_HPP
#define NAZARA_SHADER_AST_DEPENDENCYCHECKERVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API DependencyCheckerVisitor : public AstRecursiveVisitor
	{
		public:
			struct UsageSet;

			DependencyCheckerVisitor() = default;
			DependencyCheckerVisitor(const DependencyCheckerVisitor&) = delete;
			DependencyCheckerVisitor(DependencyCheckerVisitor&&) = delete;
			~DependencyCheckerVisitor() = default;

			inline const UsageSet& GetUsage() const;

			inline void Resolve();

			using AstRecursiveVisitor::Visit;

			void Visit(CallFunctionExpression& node) override;
			void Visit(VariableExpression& node) override;

			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;

			DependencyCheckerVisitor& operator=(const DependencyCheckerVisitor&) = delete;
			DependencyCheckerVisitor& operator=(DependencyCheckerVisitor&&) = delete;

			struct UsageSet
			{
				Bitset<> usedFunctions;
				Bitset<> usedStructs;
				Bitset<> usedVariables;
			};

		private:
			void Resolve(const UsageSet& usageSet);

			std::optional<std::size_t> m_currentFunctionIndex;
			std::optional<std::size_t> m_currentVariableDeclIndex;
			std::unordered_map<std::size_t, UsageSet> m_functionUsages;
			std::unordered_map<std::size_t, UsageSet> m_structUsages;
			std::unordered_map<std::size_t, UsageSet> m_variableUsages;
			UsageSet m_globalUsage;
			UsageSet m_resolvedUsage;
	};
}

#include <Nazara/Shader/Ast/DependencyCheckerVisitor.inl>

#endif // NAZARA_SHADER_AST_DEPENDENCYCHECKERVISITOR_HPP
