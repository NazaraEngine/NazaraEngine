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
		template<typename T, typename U> void UniqueInsert(std::unordered_map<T, U>& map, T key, U value)
		{
			assert(map.find(key) == map.end());
			map.emplace(std::move(key), std::move(value));
		}
	}

	struct IndexRemapperVisitor::Context
	{
		const IndexRemapperVisitor::Callbacks* callbacks;
		std::unordered_map<std::size_t, std::size_t> newAliasIndices;
		std::unordered_map<std::size_t, std::size_t> newConstIndices;
		std::unordered_map<std::size_t, std::size_t> newFuncIndices;
		std::unordered_map<std::size_t, std::size_t> newStructIndices;
		std::unordered_map<std::size_t, std::size_t> newVarIndices;
	};

	StatementPtr IndexRemapperVisitor::Clone(Statement& statement, const Callbacks& callbacks)
	{
		assert(callbacks.aliasIndexGenerator);
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

	StatementPtr IndexRemapperVisitor::Clone(DeclareAliasStatement& node)
	{
		DeclareAliasStatementPtr clone = StaticUniquePointerCast<DeclareAliasStatement>(AstCloner::Clone(node));

		if (clone->aliasIndex)
		{
			std::size_t newAliasIndex = m_context->callbacks->aliasIndexGenerator(*clone->aliasIndex);
			UniqueInsert(m_context->newAliasIndices, *clone->aliasIndex, newAliasIndex);
			clone->aliasIndex = newAliasIndex;
		}

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareConstStatement& node)
	{
		DeclareConstStatementPtr clone = StaticUniquePointerCast<DeclareConstStatement>(AstCloner::Clone(node));

		if (clone->constIndex)
		{
			std::size_t newConstIndex = m_context->callbacks->constIndexGenerator(*clone->constIndex);
			UniqueInsert(m_context->newConstIndices, *clone->constIndex, newConstIndex);
			clone->constIndex = newConstIndex;
		}

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareExternalStatement& node)
	{
		DeclareExternalStatementPtr clone = StaticUniquePointerCast<DeclareExternalStatement>(AstCloner::Clone(node));

		for (auto& extVar : clone->externalVars)
		{
			if (extVar.varIndex)
			{
				std::size_t newVarIndex = m_context->callbacks->varIndexGenerator(*extVar.varIndex);
				UniqueInsert(m_context->newVarIndices, *extVar.varIndex, newVarIndex);
				extVar.varIndex = newVarIndex;
			}
		}

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareFunctionStatement& node)
	{
		DeclareFunctionStatementPtr clone = StaticUniquePointerCast<DeclareFunctionStatement>(AstCloner::Clone(node));

		if (clone->funcIndex)
		{
			std::size_t newFuncIndex = m_context->callbacks->funcIndexGenerator(*clone->funcIndex);
			UniqueInsert(m_context->newFuncIndices, *clone->funcIndex, newFuncIndex);
			clone->funcIndex = newFuncIndex;
		}

		if (!clone->parameters.empty())
		{
			for (auto& parameter : node.parameters)
			{
				if (parameter.varIndex)
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
		DeclareStructStatementPtr clone = StaticUniquePointerCast<DeclareStructStatement>(AstCloner::Clone(node));

		if (clone->structIndex)
		{
			std::size_t newStructIndex = m_context->callbacks->structIndexGenerator(*clone->structIndex);
			UniqueInsert(m_context->newStructIndices, *clone->structIndex, newStructIndex);
			clone->structIndex = newStructIndex;
		}

		for (auto& structMember : clone->description.members)
			HandleType(structMember.type);

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareVariableStatement& node)
	{
		DeclareVariableStatementPtr clone = StaticUniquePointerCast<DeclareVariableStatement>(AstCloner::Clone(node));

		if (clone->varIndex)
		{
			std::size_t newVarIndex = m_context->callbacks->varIndexGenerator(*clone->varIndex);
			UniqueInsert(m_context->newConstIndices, *clone->varIndex, newVarIndex);
			clone->varIndex = newVarIndex;
		}

		HandleType(node.varType);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(AliasValueExpression& node)
	{
		AliasValueExpressionPtr clone = StaticUniquePointerCast<AliasValueExpression>(AstCloner::Clone(node));

		if (clone->aliasId)
			clone->aliasId = Retrieve(m_context->newAliasIndices, clone->aliasId);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(ConstantExpression& node)
	{
		ConstantExpressionPtr clone = StaticUniquePointerCast<ConstantExpression>(AstCloner::Clone(node));

		if (clone->constantId)
			clone->constantId = Retrieve(m_context->newConstIndices, clone->constantId);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(FunctionExpression& node)
	{
		FunctionExpressionPtr clone = StaticUniquePointerCast<FunctionExpression>(AstCloner::Clone(node));

		if (clone->funcId)
			clone->funcId = Retrieve(m_context->newFuncIndices, clone->funcId);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(StructTypeExpression& node)
	{
		StructTypeExpressionPtr clone = StaticUniquePointerCast<StructTypeExpression>(AstCloner::Clone(node));

		if (clone->structTypeId)
			clone->structTypeId = Retrieve(m_context->newStructIndices, clone->structTypeId);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(VariableValueExpression& node)
	{
		VariableValueExpressionPtr clone = StaticUniquePointerCast<VariableValueExpression>(AstCloner::Clone(node));

		if (clone->variableId)
			clone->variableId = Retrieve(m_context->newVarIndices, clone->variableId);

		return clone;
	}

	void IndexRemapperVisitor::HandleType(ExpressionValue<ExpressionType>& exprType)
	{
		if (!exprType.IsResultingValue())
			return;

		const auto& resultingType = exprType.GetResultingValue();
		exprType = RemapType(resultingType);
	}

	ExpressionType IndexRemapperVisitor::RemapType(const ExpressionType& exprType)
	{
		if (IsAliasType(exprType))
		{
			const AliasType& aliasType = std::get<AliasType>(exprType);

			AliasType remappedAliasType;
			remappedAliasType.aliasIndex = Retrieve(m_context->newAliasIndices, aliasType.aliasIndex);
			remappedAliasType.targetType = std::make_unique<ContainedType>();
			remappedAliasType.targetType->type = RemapType(aliasType.targetType->type);

			return remappedAliasType;
		}
		else if (IsArrayType(exprType))
		{
			const ArrayType& arrayType = std::get<ArrayType>(exprType);

			ArrayType remappedArrayType;
			remappedArrayType.containedType = std::make_unique<ContainedType>();
			remappedArrayType.containedType->type = RemapType(arrayType.containedType->type);
			remappedArrayType.length = arrayType.length;

			return remappedArrayType;
		}
		else if (IsFunctionType(exprType))
		{
			std::size_t newFuncIndex = Retrieve(m_context->newFuncIndices, std::get<FunctionType>(exprType).funcIndex);
			return FunctionType{ newFuncIndex };
		}
		else if (IsMethodType(exprType))
		{
			const MethodType& methodType = std::get<MethodType>(exprType);

			MethodType remappedMethodType;
			remappedMethodType.methodIndex = methodType.methodIndex;
			remappedMethodType.objectType = std::make_unique<ContainedType>();
			remappedMethodType.objectType->type = RemapType(methodType.objectType->type);

			return remappedMethodType;
		}
		else if (IsStructType(exprType))
		{
			std::size_t newStructIndex = Retrieve(m_context->newStructIndices, std::get<StructType>(exprType).structIndex);
			return StructType{ newStructIndex };
		}
		else if (IsUniformType(exprType))
		{
			UniformType uniformType;
			uniformType.containedType.structIndex = Retrieve(m_context->newStructIndices, std::get<UniformType>(exprType).containedType.structIndex);
			return uniformType;
		}
		else
			return exprType;
	}
}
