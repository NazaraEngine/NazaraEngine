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
			struct Config;
			struct UsageSet;

			DependencyCheckerVisitor() = default;
			DependencyCheckerVisitor(const DependencyCheckerVisitor&) = delete;
			DependencyCheckerVisitor(DependencyCheckerVisitor&&) = delete;
			~DependencyCheckerVisitor() = default;

			inline const UsageSet& GetUsage() const;

			inline void MarkFunctionAsUsed(std::size_t funcIndex);
			inline void MarkStructAsUsed(std::size_t structIndex);

			inline void Register(Statement& statement);
			void Register(Statement& statement, const Config& config);

			inline void Resolve(bool allowUnknownId = false);

			DependencyCheckerVisitor& operator=(const DependencyCheckerVisitor&) = delete;
			DependencyCheckerVisitor& operator=(DependencyCheckerVisitor&&) = delete;

			struct Config
			{
				ShaderStageTypeFlags usedShaderStages;
			};

			struct UsageSet
			{
				Bitset<> usedAliases;
				Bitset<> usedFunctions;
				Bitset<> usedStructs;
				Bitset<> usedVariables;
			};

		private:
			UsageSet& GetContextUsageSet();
			void RegisterType(UsageSet& usageSet, const ExpressionType& exprType);
			void Resolve(const UsageSet& usageSet, bool allowUnknownId);

			using AstRecursiveVisitor::Visit;

			void Visit(AliasValueExpression& node) override;
			void Visit(FunctionExpression& node) override;
			void Visit(StructTypeExpression& node) override;
			void Visit(VariableValueExpression& node) override;

			void Visit(DeclareAliasStatement& node) override;
			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;

			std::optional<std::size_t> m_currentAliasDeclIndex;
			std::optional<std::size_t> m_currentFunctionIndex;
			std::optional<std::size_t> m_currentVariableDeclIndex;
			std::unordered_map<std::size_t, UsageSet> m_aliasUsages;
			std::unordered_map<std::size_t, UsageSet> m_functionUsages;
			std::unordered_map<std::size_t, UsageSet> m_structUsages;
			std::unordered_map<std::size_t, UsageSet> m_variableUsages;
			Config m_config;
			UsageSet m_globalUsage;
			UsageSet m_resolvedUsage;
	};
}

#include <Nazara/Shader/Ast/DependencyCheckerVisitor.inl>

#endif // NAZARA_SHADER_AST_DEPENDENCYCHECKERVISITOR_HPP
