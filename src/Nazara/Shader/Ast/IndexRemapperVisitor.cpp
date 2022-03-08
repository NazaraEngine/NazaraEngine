// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/IndexRemapperVisitor.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <unordered_map>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		template<typename T> const T& Retrieve(const std::unordered_map<std::size_t, T>& map, std::size_t id)
		{
			auto it = map.find(id);
			assert(it != map.end());
			return it->second;
		}

		template<typename T, typename U> void UniqueInsert(std::unordered_map<T, U>& map, T key, U value)
		{
			assert(map.find(key) == map.end());
			map.emplace(std::move(key), std::move(value));
		}

		template<typename T, typename U>
		std::unique_ptr<T> static_unique_pointer_cast(std::unique_ptr<U>&& ptr)
		{
			return std::unique_ptr<T>(SafeCast<T*>(ptr.release()));
		}
	}

	struct IndexRemapperVisitor::Context
	{
		const IndexRemapperVisitor::Callbacks* callbacks;
		std::unordered_map<std::size_t, std::size_t> newConstIndices;
		std::unordered_map<std::size_t, std::size_t> newFuncIndices;
		std::unordered_map<std::size_t, std::size_t> newStructIndices;
		std::unordered_map<std::size_t, std::size_t> newVarIndices;
	};

	StatementPtr IndexRemapperVisitor::Clone(Statement& statement, const Callbacks& callbacks)
	{
		assert(callbacks.constIndexGenerator);
		assert(callbacks.funcIndexGenerator);
		assert(callbacks.structIndexGenerator);
		//assert(callbacks.typeIndexGenerator);
		assert(callbacks.varIndexGenerator);

		Context context;
		context.callbacks = &callbacks;
		m_context = &context;

		return AstCloner::Clone(statement);
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareConstStatement& node)
	{
		DeclareConstStatementPtr clone = static_unique_pointer_cast<DeclareConstStatement>(AstCloner::Clone(node));

		assert(clone->constIndex);
		std::size_t newConstIndex = m_context->callbacks->constIndexGenerator(*clone->constIndex);
		UniqueInsert(m_context->newConstIndices, *clone->constIndex, newConstIndex);
		clone->constIndex = newConstIndex;

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareExternalStatement& node)
	{
		DeclareExternalStatementPtr clone = static_unique_pointer_cast<DeclareExternalStatement>(AstCloner::Clone(node));

		for (auto& extVar : clone->externalVars)
		{
			assert(extVar.varIndex);
			std::size_t newVarIndex = m_context->callbacks->varIndexGenerator(*extVar.varIndex);
			UniqueInsert(m_context->newVarIndices, *extVar.varIndex, newVarIndex);
			extVar.varIndex = newVarIndex;
		}

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareFunctionStatement& node)
	{
		DeclareFunctionStatementPtr clone = static_unique_pointer_cast<DeclareFunctionStatement>(AstCloner::Clone(node));

		assert(clone->funcIndex);
		std::size_t newFuncIndex = m_context->callbacks->funcIndexGenerator(*clone->funcIndex);
		UniqueInsert(m_context->newFuncIndices, *clone->funcIndex, newFuncIndex);
		clone->funcIndex = newFuncIndex;

		if (!clone->parameters.empty())
		{
			for (auto& parameter : node.parameters)
			{
				assert(parameter.varIndex);
				parameter.varIndex = Retrieve(m_context->newVarIndices, *parameter.varIndex);

				HandleType(parameter.type);
			}
		}

		if (node.returnType.HasValue())
			HandleType(node.returnType);

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareStructStatement& node)
	{
		DeclareStructStatementPtr clone = static_unique_pointer_cast<DeclareStructStatement>(AstCloner::Clone(node));

		assert(clone->structIndex);
		std::size_t newStructIndex = m_context->callbacks->structIndexGenerator(*clone->structIndex);
		UniqueInsert(m_context->newStructIndices, *clone->structIndex, newStructIndex);
		clone->structIndex = newStructIndex;

		for (auto& structMember : clone->description.members)
			HandleType(structMember.type);

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareVariableStatement& node)
	{
		DeclareVariableStatementPtr clone = static_unique_pointer_cast<DeclareVariableStatement>(AstCloner::Clone(node));

		assert(clone->varIndex);
		std::size_t newVarIndex = m_context->callbacks->varIndexGenerator(*clone->varIndex);
		UniqueInsert(m_context->newConstIndices, *clone->varIndex, newVarIndex);
		clone->varIndex = newVarIndex;

		HandleType(node.varType);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(FunctionExpression& node)
	{
		FunctionExpressionPtr clone = static_unique_pointer_cast<FunctionExpression>(AstCloner::Clone(node));

		assert(clone->funcId);
		clone->funcId = Retrieve(m_context->newFuncIndices, clone->funcId);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(StructTypeExpression& node)
	{
		StructTypeExpressionPtr clone = static_unique_pointer_cast<StructTypeExpression>(AstCloner::Clone(node));

		assert(clone->structTypeId);
		clone->structTypeId = Retrieve(m_context->newStructIndices, clone->structTypeId);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(VariableExpression& node)
	{
		VariableExpressionPtr clone = static_unique_pointer_cast<VariableExpression>(AstCloner::Clone(node));

		assert(clone->variableId);
		clone->variableId = Retrieve(m_context->newVarIndices, clone->variableId);

		return clone;
	}

	void IndexRemapperVisitor::HandleType(ExpressionValue<ExpressionType>& exprType)
	{
		const auto& resultingType = exprType.GetResultingValue();
		if (IsStructType(resultingType))
		{
			std::size_t newStructIndex = Retrieve(m_context->newStructIndices, std::get<StructType>(resultingType).structIndex);
			exprType = ExpressionType{ StructType{ newStructIndex } };
		}
	}
}
