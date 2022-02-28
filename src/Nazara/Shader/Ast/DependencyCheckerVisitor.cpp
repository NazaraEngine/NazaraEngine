// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/DependencyCheckerVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		template<typename T> T& Retrieve(std::unordered_map<std::size_t, T>& map, std::size_t id)
		{
			auto it = map.find(id);
			assert(it != map.end());
			return it->second;
		}

		template<typename T> const T& Retrieve(const std::unordered_map<std::size_t, T>& map, std::size_t id)
		{
			auto it = map.find(id);
			assert(it != map.end());
			return it->second;
		}
	}

	void DependencyCheckerVisitor::Process(Statement& statement, const Config& config)
	{
		m_config = config;
		statement.Visit(*this);
	}

	void DependencyCheckerVisitor::Visit(CallFunctionExpression& node)
	{
		const auto& targetFuncType = GetExpressionType(*node.targetFunction);
		assert(std::holds_alternative<FunctionType>(targetFuncType));

		const auto& funcType = std::get<FunctionType>(targetFuncType);

		assert(m_currentFunctionIndex);
		if (m_currentVariableDeclIndex)
		{
			UsageSet& usageSet = Retrieve(m_variableUsages, *m_currentVariableDeclIndex);
			usageSet.usedFunctions.UnboundedSet(funcType.funcIndex);
		}
		else
		{
			UsageSet& usageSet = Retrieve(m_functionUsages, *m_currentFunctionIndex);
			usageSet.usedFunctions.UnboundedSet(funcType.funcIndex);
		}

		AstRecursiveVisitor::Visit(node);
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
			assert(node.varIndex);
			std::size_t parameterVarIndex = *node.varIndex;
			for (auto& parameter : node.parameters)
			{
				// Since parameters must always be defined, their type isn't a dependency of parameter variables
				assert(m_variableUsages.find(parameterVarIndex) == m_variableUsages.end());
				m_variableUsages.emplace(parameterVarIndex, UsageSet{});

				const auto& exprType = parameter.type.GetResultingValue();
				if (IsStructType(exprType))
				{
					std::size_t structIndex = std::get<ShaderAst::StructType>(exprType).structIndex;
					usageSet.usedStructs.UnboundedSet(structIndex);
				}

				++parameterVarIndex;
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

	void DependencyCheckerVisitor::Visit(VariableExpression& node)
	{
		assert(m_currentFunctionIndex);
		if (m_currentVariableDeclIndex)
		{
			UsageSet& usageSet = Retrieve(m_variableUsages, *m_currentVariableDeclIndex);
			usageSet.usedVariables.UnboundedSet(node.variableId);
		}
		else
		{
			UsageSet& usageSet = Retrieve(m_functionUsages, *m_currentFunctionIndex);
			usageSet.usedVariables.UnboundedSet(node.variableId);
		}
	}

	void DependencyCheckerVisitor::Resolve(const UsageSet& usageSet)
	{
		m_resolvedUsage.usedFunctions |= usageSet.usedFunctions;
		m_resolvedUsage.usedStructs |= usageSet.usedStructs;
		m_resolvedUsage.usedVariables |= usageSet.usedVariables;

		for (std::size_t funcIndex = usageSet.usedFunctions.FindFirst(); funcIndex != usageSet.usedFunctions.npos; funcIndex = usageSet.usedFunctions.FindNext(funcIndex))
			Resolve(Retrieve(m_functionUsages, funcIndex));

		for (std::size_t structIndex = usageSet.usedStructs.FindFirst(); structIndex != usageSet.usedStructs.npos; structIndex = usageSet.usedStructs.FindNext(structIndex))
			Resolve(Retrieve(m_structUsages, structIndex));

		for (std::size_t varIndex = usageSet.usedVariables.FindFirst(); varIndex != usageSet.usedVariables.npos; varIndex = usageSet.usedVariables.FindNext(varIndex))
			Resolve(Retrieve(m_variableUsages, varIndex));
	}
}
