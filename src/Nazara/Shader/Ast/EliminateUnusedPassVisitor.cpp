// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <unordered_map>
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

		struct UsageChecker : AstRecursiveVisitor
		{
			struct UsageSet;

			void Resolve()
			{
				Resolve(globalUsage);
			}

			void Resolve(const UsageSet& usageSet)
			{
				resolvedUsage.usedFunctions |= usageSet.usedFunctions;
				resolvedUsage.usedStructs |= usageSet.usedStructs;
				resolvedUsage.usedVariables |= usageSet.usedVariables;

				for (std::size_t funcIndex = usageSet.usedFunctions.FindFirst(); funcIndex != usageSet.usedFunctions.npos; funcIndex = usageSet.usedFunctions.FindNext(funcIndex))
					Resolve(Retrieve(functionUsages, funcIndex));

				for (std::size_t structIndex = usageSet.usedStructs.FindFirst(); structIndex != usageSet.usedStructs.npos; structIndex = usageSet.usedStructs.FindNext(structIndex))
					Resolve(Retrieve(structUsages, structIndex));

				for (std::size_t varIndex = usageSet.usedVariables.FindFirst(); varIndex != usageSet.usedVariables.npos; varIndex = usageSet.usedVariables.FindNext(varIndex))
					Resolve(Retrieve(variableUsages, varIndex));
			}

			using AstRecursiveVisitor::Visit;

			void Visit(CallFunctionExpression& node) override
			{
				const auto& targetFuncType = GetExpressionType(node);
				assert(std::holds_alternative<FunctionType>(targetFuncType));

				const auto& funcType = std::get<FunctionType>(targetFuncType);

				assert(currentFunctionIndex);
				UsageSet& usageSet = Retrieve(functionUsages, *currentFunctionIndex);
				usageSet.usedFunctions.UnboundedSet(funcType.funcIndex);
			}

			void Visit(DeclareExternalStatement& node) override
			{
				for (const auto& externalVar : node.externalVars)
				{
					assert(externalVar.varIndex);
					std::size_t varIndex = *externalVar.varIndex;

					assert(variableUsages.find(varIndex) == variableUsages.end());
					UsageSet& usageSet = variableUsages[varIndex];

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

			void Visit(DeclareFunctionStatement& node) override
			{
				assert(node.funcIndex);
				assert(functionUsages.find(*node.funcIndex) == functionUsages.end());
				UsageSet& usageSet = functionUsages[*node.funcIndex];

				// Register struct used in parameters or return type
				if (!node.parameters.empty())
				{
					assert(node.varIndex);
					std::size_t parameterVarIndex = *node.varIndex;
					for (auto& parameter : node.parameters)
					{
						// Since parameters must always be defined, their type isn't a dependency of parameter variables
						assert(variableUsages.find(parameterVarIndex) == variableUsages.end());
						variableUsages.emplace(parameterVarIndex, UsageSet{});

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
					globalUsage.usedFunctions.UnboundedSet(*node.funcIndex);

				currentFunctionIndex = node.funcIndex;
				AstRecursiveVisitor::Visit(node);
				currentFunctionIndex = {};
			}

			void Visit(DeclareStructStatement& node) override
			{
				assert(node.structIndex);
				assert(structUsages.find(*node.structIndex) == structUsages.end());
				UsageSet& usageSet = structUsages[*node.structIndex];

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

			void Visit(DeclareVariableStatement& node) override
			{
				assert(node.varIndex);
				assert(variableUsages.find(*node.varIndex) == variableUsages.end());
				UsageSet& usageSet = variableUsages[*node.varIndex];

				const auto& varType = node.varType.GetResultingValue();
				if (IsStructType(varType))
				{
					const auto& structType = std::get<StructType>(varType);
					usageSet.usedStructs.UnboundedSet(structType.structIndex);
				}

				currentVariableDeclIndex = node.varIndex;
				AstRecursiveVisitor::Visit(node);
				currentVariableDeclIndex = {};
			}

			void Visit(VariableExpression& node) override
			{
				assert(currentFunctionIndex);
				if (currentVariableDeclIndex)
				{
					UsageSet& usageSet = Retrieve(variableUsages, *currentVariableDeclIndex);
					usageSet.usedVariables.UnboundedSet(node.variableId);
				}
				else
				{
					UsageSet& usageSet = Retrieve(functionUsages, *currentFunctionIndex);
					usageSet.usedVariables.UnboundedSet(node.variableId);
				}
			}

			struct UsageSet
			{
				Bitset<> usedFunctions;
				Bitset<> usedStructs;
				Bitset<> usedVariables;
			};

			std::optional<std::size_t> currentFunctionIndex;
			std::optional<std::size_t> currentVariableDeclIndex;
			std::unordered_map<std::size_t, UsageSet> functionUsages;
			std::unordered_map<std::size_t, UsageSet> structUsages;
			std::unordered_map<std::size_t, UsageSet> variableUsages;
			UsageSet globalUsage;
			UsageSet resolvedUsage;
		};
	}

	struct EliminateUnusedPassVisitor::Context
	{
		UsageChecker usageChecker;
	};

	StatementPtr EliminateUnusedPassVisitor::Process(Statement& statement)
	{
		Context context;
		statement.Visit(context.usageChecker);
		context.usageChecker.Resolve();

		m_context = &context;
		CallOnExit onExit([this]()
		{
			m_context = nullptr;
		});

		return Clone(statement);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareExternalStatement& node)
	{
		bool isUsed = false;
		for (const auto& externalVar : node.externalVars)
		{
			assert(externalVar.varIndex);
			std::size_t varIndex = *externalVar.varIndex;

			if (IsVariableUsed(varIndex))
			{
				isUsed = true;
				break;
			}
		}

		if (!isUsed)
			return ShaderBuilder::NoOp();

		auto clonedNode = AstCloner::Clone(node);

		auto& externalStatement = static_cast<DeclareExternalStatement&>(*clonedNode);
		for (auto it = externalStatement.externalVars.begin(); it != externalStatement.externalVars.end(); )
		{
			const auto& externalVar = *it;
			assert(externalVar.varIndex);
			std::size_t varIndex = *externalVar.varIndex;

			if (!IsVariableUsed(varIndex))
				it = externalStatement.externalVars.erase(it);
			else
				++it;
		}

		return clonedNode;
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareFunctionStatement& node)
	{
		assert(node.funcIndex);
		if (!IsFunctionUsed(*node.funcIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareStructStatement& node)
	{
		assert(node.structIndex);
		if (!IsStructUsed(*node.structIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareVariableStatement& node)
	{
		assert(node.varIndex);
		if (!IsVariableUsed(*node.varIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	bool EliminateUnusedPassVisitor::IsFunctionUsed(std::size_t varIndex) const
	{
		assert(m_context);
		return m_context->usageChecker.resolvedUsage.usedFunctions.UnboundedTest(varIndex);
	}

	bool EliminateUnusedPassVisitor::IsStructUsed(std::size_t varIndex) const
	{
		assert(m_context);
		return m_context->usageChecker.resolvedUsage.usedStructs.UnboundedTest(varIndex);
	}

	bool EliminateUnusedPassVisitor::IsVariableUsed(std::size_t varIndex) const
	{
		assert(m_context);
		return m_context->usageChecker.resolvedUsage.usedVariables.UnboundedTest(varIndex);
	}
}
