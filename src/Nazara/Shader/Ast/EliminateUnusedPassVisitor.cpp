// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		template<typename T, typename U>
		std::unique_ptr<T> static_unique_pointer_cast(std::unique_ptr<U>&& ptr)
		{
			return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
		}
	}

	struct EliminateUnusedPassVisitor::Context
	{
		const DependencyCheckerVisitor::UsageSet& usageSet;
	};

	ModulePtr EliminateUnusedPassVisitor::Process(const Module& shaderModule, const DependencyCheckerVisitor::UsageSet& usageSet)
	{
		auto rootNode = static_unique_pointer_cast<MultiStatement>(Process(*shaderModule.rootNode, usageSet));
		
		return std::make_shared<Module>(shaderModule.metadata, std::move(rootNode), shaderModule.importedModules);
	}

	StatementPtr EliminateUnusedPassVisitor::Process(Statement& statement, const DependencyCheckerVisitor::UsageSet& usageSet)
	{
		Context context{
			usageSet
		};

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
		return m_context->usageSet.usedFunctions.UnboundedTest(varIndex);
	}

	bool EliminateUnusedPassVisitor::IsStructUsed(std::size_t varIndex) const
	{
		assert(m_context);
		return m_context->usageSet.usedStructs.UnboundedTest(varIndex);
	}

	bool EliminateUnusedPassVisitor::IsVariableUsed(std::size_t varIndex) const
	{
		assert(m_context);
		return m_context->usageSet.usedVariables.UnboundedTest(varIndex);
	}
}
