// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/DependencyCheckerVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	void DependencyCheckerVisitor::Process(Statement& statement, const Config& config)
	{
		m_config = config;
		statement.Visit(*this);
	}

	auto DependencyCheckerVisitor::GetContextUsageSet() -> UsageSet&
	{
		if (m_currentAliasDeclIndex)
			return Retrieve(m_aliasUsages, *m_currentAliasDeclIndex);
		else if (m_currentVariableDeclIndex)
			return Retrieve(m_variableUsages, *m_currentVariableDeclIndex);
		else
		{
			assert(m_currentFunctionIndex);
			return Retrieve(m_functionUsages, *m_currentFunctionIndex);
		}
	}

	void DependencyCheckerVisitor::Resolve(const UsageSet& usageSet)
	{
		m_resolvedUsage.usedFunctions |= usageSet.usedFunctions;
		m_resolvedUsage.usedStructs |= usageSet.usedStructs;
		m_resolvedUsage.usedVariables |= usageSet.usedVariables;

		for (std::size_t aliasIndex = usageSet.usedAliases.FindFirst(); aliasIndex != usageSet.usedAliases.npos; aliasIndex = usageSet.usedAliases.FindNext(aliasIndex))
			Resolve(Retrieve(m_aliasUsages, aliasIndex));

		for (std::size_t funcIndex = usageSet.usedFunctions.FindFirst(); funcIndex != usageSet.usedFunctions.npos; funcIndex = usageSet.usedFunctions.FindNext(funcIndex))
			Resolve(Retrieve(m_functionUsages, funcIndex));

		for (std::size_t structIndex = usageSet.usedStructs.FindFirst(); structIndex != usageSet.usedStructs.npos; structIndex = usageSet.usedStructs.FindNext(structIndex))
			Resolve(Retrieve(m_structUsages, structIndex));

		for (std::size_t varIndex = usageSet.usedVariables.FindFirst(); varIndex != usageSet.usedVariables.npos; varIndex = usageSet.usedVariables.FindNext(varIndex))
			Resolve(Retrieve(m_variableUsages, varIndex));
	}

	void DependencyCheckerVisitor::Visit(DeclareAliasStatement& node)
	{
		assert(node.aliasIndex);
		assert(m_aliasUsages.find(*node.aliasIndex) == m_aliasUsages.end());
		m_aliasUsages.emplace(*node.aliasIndex, UsageSet{});

		assert(node.aliasIndex);
		m_currentAliasDeclIndex = *node.aliasIndex;
		AstRecursiveVisitor::Visit(node);
		m_currentAliasDeclIndex = {};
	}

	void DependencyCheckerVisitor::Visit(DeclareExternalStatement& node)
	{
		for (const auto& externalVar : node.externalVars)
		{
			assert(externalVar.varIndex);
			std::size_t varIndex = *externalVar.varIndex;

			assert(m_variableUsages.find(varIndex) == m_variableUsages.end());
			UsageSet& usageSet = m_variableUsages[varIndex];

			const auto& exprType = externalVar.type.GetResultingValue();

			if (IsUniformType(exprType))
			{
				const UniformType& uniformType = std::get<UniformType>(exprType);
				usageSet.usedStructs.UnboundedSet(uniformType.containedType.structIndex);
			}

			++varIndex;
		}

		AstRecursiveVisitor::Visit(node);
	}

	void DependencyCheckerVisitor::Visit(DeclareFunctionStatement& node)
	{
		assert(node.funcIndex);
		assert(m_functionUsages.find(*node.funcIndex) == m_functionUsages.end());
		UsageSet& usageSet = m_functionUsages[*node.funcIndex];

		// Register struct used in parameters or return type
		if (!node.parameters.empty())
		{
			for (auto& parameter : node.parameters)
			{
				assert(parameter.varIndex);

				// Since parameters must always be defined, their type isn't a dependency of parameter variables
				assert(m_variableUsages.find(*parameter.varIndex) == m_variableUsages.end());
				m_variableUsages.emplace(*parameter.varIndex, UsageSet{});

				const auto& exprType = parameter.type.GetResultingValue();
				if (IsStructType(exprType))
				{
					std::size_t structIndex = std::get<ShaderAst::StructType>(exprType).structIndex;
					usageSet.usedStructs.UnboundedSet(structIndex);
				}
			}
		}

		if (node.returnType.HasValue())
		{
			const auto& returnExprType = node.returnType.GetResultingValue();
			if (IsStructType(returnExprType))
			{
				std::size_t structIndex = std::get<ShaderAst::StructType>(returnExprType).structIndex;
				usageSet.usedStructs.UnboundedSet(structIndex);
			}
		}

		if (node.entryStage.HasValue())
		{
			ShaderStageType shaderStage = node.entryStage.GetResultingValue();
			if (m_config.usedShaderStages & shaderStage)
				m_globalUsage.usedFunctions.UnboundedSet(*node.funcIndex);
		}

		m_currentFunctionIndex = node.funcIndex;
		AstRecursiveVisitor::Visit(node);
		m_currentFunctionIndex = {};
	}

	void DependencyCheckerVisitor::Visit(DeclareStructStatement& node)
	{
		assert(node.structIndex);
		assert(m_structUsages.find(*node.structIndex) == m_structUsages.end());
		UsageSet& usageSet = m_structUsages[*node.structIndex];

		for (const auto& structMember : node.description.members)
		{
			const auto& memberExprType = structMember.type.GetResultingValue();
			if (IsStructType(memberExprType))
			{
				std::size_t structIndex = std::get<ShaderAst::StructType>(memberExprType).structIndex;
				usageSet.usedStructs.UnboundedSet(structIndex);
			}
		}

		AstRecursiveVisitor::Visit(node);
	}

	void DependencyCheckerVisitor::Visit(DeclareVariableStatement& node)
	{
		assert(node.varIndex);
		assert(m_variableUsages.find(*node.varIndex) == m_variableUsages.end());
		UsageSet& usageSet = m_variableUsages[*node.varIndex];

		const auto& varType = node.varType.GetResultingValue();
		if (IsStructType(varType))
		{
			const auto& structType = std::get<StructType>(varType);
			usageSet.usedStructs.UnboundedSet(structType.structIndex);
		}

		m_currentVariableDeclIndex = node.varIndex;
		AstRecursiveVisitor::Visit(node);
		m_currentVariableDeclIndex = {};
	}

	void DependencyCheckerVisitor::Visit(AliasValueExpression& node)
	{
		UsageSet& usageSet = GetContextUsageSet();
		usageSet.usedAliases.UnboundedSet(node.aliasId);
	}

	void DependencyCheckerVisitor::Visit(FunctionExpression& node)
	{
		UsageSet& usageSet = GetContextUsageSet();
		usageSet.usedFunctions.UnboundedSet(node.funcId);
	}

	void DependencyCheckerVisitor::Visit(StructTypeExpression& node)
	{
		UsageSet& usageSet = GetContextUsageSet();
		usageSet.usedStructs.UnboundedSet(node.structTypeId);
	}

	void DependencyCheckerVisitor::Visit(VariableValueExpression& node)
	{
		UsageSet& usageSet = GetContextUsageSet();
		usageSet.usedVariables.UnboundedSet(node.variableId);
	}
}
