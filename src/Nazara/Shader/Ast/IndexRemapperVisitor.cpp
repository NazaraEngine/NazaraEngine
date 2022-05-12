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
		const IndexRemapperVisitor::Options* options;
		std::unordered_map<std::size_t, std::size_t> newAliasIndices;
		std::unordered_map<std::size_t, std::size_t> newConstIndices;
		std::unordered_map<std::size_t, std::size_t> newFuncIndices;
		std::unordered_map<std::size_t, std::size_t> newStructIndices;
		std::unordered_map<std::size_t, std::size_t> newVarIndices;
	};

	StatementPtr IndexRemapperVisitor::Clone(Statement& statement, const Options& options)
	{
		assert(options.aliasIndexGenerator);
		assert(options.constIndexGenerator);
		assert(options.funcIndexGenerator);
		assert(options.structIndexGenerator);
		//assert(options.typeIndexGenerator);
		assert(options.varIndexGenerator);

		Context context;
		context.options = &options;
		m_context = &context;

		return AstCloner::Clone(statement);
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareAliasStatement& node)
	{
		DeclareAliasStatementPtr clone = StaticUniquePointerCast<DeclareAliasStatement>(AstCloner::Clone(node));

		if (clone->aliasIndex)
		{
			std::size_t newAliasIndex = m_context->options->aliasIndexGenerator(*clone->aliasIndex);
			UniqueInsert(m_context->newAliasIndices, *clone->aliasIndex, newAliasIndex);
			clone->aliasIndex = newAliasIndex;
		}
		else if (m_context->options->forceIndexGeneration)
			clone->aliasIndex = m_context->options->aliasIndexGenerator(std::numeric_limits<std::size_t>::max());

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareConstStatement& node)
	{
		DeclareConstStatementPtr clone = StaticUniquePointerCast<DeclareConstStatement>(AstCloner::Clone(node));

		if (clone->constIndex)
		{
			std::size_t newConstIndex = m_context->options->constIndexGenerator(*clone->constIndex);
			UniqueInsert(m_context->newConstIndices, *clone->constIndex, newConstIndex);
			clone->constIndex = newConstIndex;
		}
		else if (m_context->options->forceIndexGeneration)
			clone->constIndex = m_context->options->constIndexGenerator(std::numeric_limits<std::size_t>::max());

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareExternalStatement& node)
	{
		DeclareExternalStatementPtr clone = StaticUniquePointerCast<DeclareExternalStatement>(AstCloner::Clone(node));

		for (auto& extVar : clone->externalVars)
		{
			if (extVar.varIndex)
			{
				std::size_t newVarIndex = m_context->options->varIndexGenerator(*extVar.varIndex);
				UniqueInsert(m_context->newVarIndices, *extVar.varIndex, newVarIndex);
				extVar.varIndex = newVarIndex;
			}
			else if (m_context->options->forceIndexGeneration)
				extVar.varIndex = m_context->options->varIndexGenerator(std::numeric_limits<std::size_t>::max());
		}

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareFunctionStatement& node)
	{
		DeclareFunctionStatementPtr clone = StaticUniquePointerCast<DeclareFunctionStatement>(AstCloner::Clone(node));

		if (clone->funcIndex)
		{
			std::size_t newFuncIndex = m_context->options->funcIndexGenerator(*clone->funcIndex);
			UniqueInsert(m_context->newFuncIndices, *clone->funcIndex, newFuncIndex);
			clone->funcIndex = newFuncIndex;
		}
		else if (m_context->options->forceIndexGeneration)
			clone->funcIndex = m_context->options->funcIndexGenerator(std::numeric_limits<std::size_t>::max());

		if (!clone->parameters.empty())
		{
			for (auto& parameter : clone->parameters)
			{
				if (parameter.varIndex)
					parameter.varIndex = Retrieve(m_context->newVarIndices, *parameter.varIndex);
				else if (m_context->options->forceIndexGeneration)
					parameter.varIndex = m_context->options->varIndexGenerator(std::numeric_limits<std::size_t>::max());

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
			std::size_t newStructIndex = m_context->options->structIndexGenerator(*clone->structIndex);
			UniqueInsert(m_context->newStructIndices, *clone->structIndex, newStructIndex);
			clone->structIndex = newStructIndex;
		}
		else if (m_context->options->forceIndexGeneration)
			clone->structIndex = m_context->options->structIndexGenerator(std::numeric_limits<std::size_t>::max());

		for (auto& structMember : clone->description.members)
			HandleType(structMember.type);

		return clone;
	}

	StatementPtr IndexRemapperVisitor::Clone(DeclareVariableStatement& node)
	{
		DeclareVariableStatementPtr clone = StaticUniquePointerCast<DeclareVariableStatement>(AstCloner::Clone(node));

		if (clone->varIndex)
		{
			std::size_t newVarIndex = m_context->options->varIndexGenerator(*clone->varIndex);
			UniqueInsert(m_context->newConstIndices, *clone->varIndex, newVarIndex);
			clone->varIndex = newVarIndex;
		}
		else if (m_context->options->forceIndexGeneration)
			clone->varIndex = m_context->options->varIndexGenerator(std::numeric_limits<std::size_t>::max());

		HandleType(node.varType);

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(AliasValueExpression& node)
	{
		AliasValueExpressionPtr clone = StaticUniquePointerCast<AliasValueExpression>(AstCloner::Clone(node));

		if (clone->aliasId)
			clone->aliasId = Retrieve(m_context->newAliasIndices, clone->aliasId);
		else if (m_context->options->forceIndexGeneration)
			clone->aliasId = m_context->options->aliasIndexGenerator(std::numeric_limits<std::size_t>::max());

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(ConstantExpression& node)
	{
		ConstantExpressionPtr clone = StaticUniquePointerCast<ConstantExpression>(AstCloner::Clone(node));

		if (clone->constantId)
			clone->constantId = Retrieve(m_context->newConstIndices, clone->constantId);
		else if (m_context->options->forceIndexGeneration)
			clone->constantId = m_context->options->constIndexGenerator(std::numeric_limits<std::size_t>::max());

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(FunctionExpression& node)
	{
		FunctionExpressionPtr clone = StaticUniquePointerCast<FunctionExpression>(AstCloner::Clone(node));

		if (clone->funcId)
			clone->funcId = Retrieve(m_context->newFuncIndices, clone->funcId);
		else if (m_context->options->forceIndexGeneration)
			clone->funcId = m_context->options->funcIndexGenerator(std::numeric_limits<std::size_t>::max());

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(StructTypeExpression& node)
	{
		StructTypeExpressionPtr clone = StaticUniquePointerCast<StructTypeExpression>(AstCloner::Clone(node));

		if (clone->structTypeId)
			clone->structTypeId = Retrieve(m_context->newStructIndices, clone->structTypeId);
		else if (m_context->options->forceIndexGeneration)
			clone->structTypeId = m_context->options->structIndexGenerator(std::numeric_limits<std::size_t>::max());

		return clone;
	}

	ExpressionPtr IndexRemapperVisitor::Clone(VariableValueExpression& node)
	{
		VariableValueExpressionPtr clone = StaticUniquePointerCast<VariableValueExpression>(AstCloner::Clone(node));

		if (clone->variableId)
			clone->variableId = Retrieve(m_context->newVarIndices, clone->variableId);
		else if (m_context->options->forceIndexGeneration)
			clone->variableId = m_context->options->varIndexGenerator(std::numeric_limits<std::size_t>::max());

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
