// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/TransformVisitor.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	StatementPtr TransformVisitor::Transform(StatementPtr& nodePtr)
	{
		StatementPtr clone;

		PushScope(); //< Global scope
		{
			clone = AstCloner::Clone(nodePtr);
		}
		PopScope();

		return clone;
	}

	void TransformVisitor::Visit(BranchStatement& node)
	{
		for (auto& cond : node.condStatements)
		{
			PushScope();
			{
				cond.condition->Visit(*this);
				cond.statement->Visit(*this);
			}
			PopScope();
		}

		if (node.elseStatement)
		{
			PushScope();
			{
				node.elseStatement->Visit(*this);
			}
			PopScope();
		}
	}

	void TransformVisitor::Visit(ConditionalStatement& node)
	{
		PushScope();
		{
			AstCloner::Visit(node);
		}
		PopScope();
	}

	ExpressionType TransformVisitor::ResolveType(const ExpressionType& exprType)
	{
		return std::visit([&](auto&& arg) -> ExpressionType
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, NoType> ||
			              std::is_same_v<T, PrimitiveType> ||
			              std::is_same_v<T, MatrixType> ||
			              std::is_same_v<T, SamplerType> ||
			              std::is_same_v<T, StructType> ||
			              std::is_same_v<T, VectorType>)
			{
				return exprType;
			}
			else if constexpr (std::is_same_v<T, IdentifierType>)
			{
				const Identifier* identifier = FindIdentifier(arg.name);
				assert(identifier);
				assert(std::holds_alternative<Struct>(identifier->value));

				return StructType{ std::get<Struct>(identifier->value).structIndex };
			}
			else if constexpr (std::is_same_v<T, UniformType>)
			{
				return std::visit([&](auto&& containedArg)
				{
					ExpressionType resolvedType = ResolveType(containedArg);
					assert(std::holds_alternative<StructType>(resolvedType));

					return UniformType{ std::get<StructType>(resolvedType) };
				}, arg.containedType);
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, exprType);
	}

	void TransformVisitor::Visit(DeclareExternalStatement& node)
	{
		for (auto& extVar : node.externalVars)
		{
			extVar.type = ResolveType(extVar.type);

			std::size_t varIndex = RegisterVariable(extVar.name);
			if (!node.varIndex)
				node.varIndex = varIndex;
		}

		AstCloner::Visit(node);
	}

	void TransformVisitor::Visit(DeclareFunctionStatement& node)
	{
		node.funcIndex = m_nextFuncIndex++;
		node.returnType = ResolveType(node.returnType);
		for (auto& parameter : node.parameters)
			parameter.type = ResolveType(parameter.type);

		PushScope();
		{
			for (auto& parameter : node.parameters)
			{
				std::size_t varIndex = RegisterVariable(parameter.name);
				if (!node.varIndex)
					node.varIndex = varIndex;
			}

			AstCloner::Visit(node);
		}
		PopScope();
	}

	void TransformVisitor::Visit(DeclareStructStatement& node)
	{
		node.structIndex = RegisterStruct(node.description.name, node.description);

		AstCloner::Visit(node);
	}

	void TransformVisitor::Visit(DeclareVariableStatement& node)
	{
		node.varType = ResolveType(node.varType);
		node.varIndex = RegisterVariable(node.varName);

		AstCloner::Visit(node);
	}

	void TransformVisitor::Visit(MultiStatement& node)
	{
		PushScope();
		{
			AstCloner::Visit(node);
		}
		PopScope();
	}

	ExpressionPtr TransformVisitor::Clone(AccessMemberIdentifierExpression& node)
	{
		auto accessMemberIndex = std::make_unique<AccessMemberIndexExpression>();
		accessMemberIndex->structExpr = CloneExpression(node.structExpr);
		accessMemberIndex->cachedExpressionType = node.cachedExpressionType;
		accessMemberIndex->memberIndices.resize(node.memberIdentifiers.size());

		ExpressionType exprType = GetExpressionType(*node.structExpr);
		for (std::size_t i = 0; i < node.memberIdentifiers.size(); ++i)
		{
			exprType = ResolveType(exprType);
			assert(std::holds_alternative<StructType>(exprType));

			std::size_t structIndex = std::get<StructType>(exprType).structIndex;
			assert(structIndex < m_structs.size());
			const StructDescription& structDesc = m_structs[structIndex];

			auto it = std::find_if(structDesc.members.begin(), structDesc.members.end(), [&](const auto& member) { return member.name == node.memberIdentifiers[i]; });
			assert(it != structDesc.members.end());

			accessMemberIndex->memberIndices[i] = std::distance(structDesc.members.begin(), it);
			exprType = it->type;
		}

		return accessMemberIndex;
	}

	ExpressionPtr TransformVisitor::Clone(CastExpression& node)
	{
		ExpressionPtr expr = AstCloner::Clone(node);

		CastExpression* castExpr = static_cast<CastExpression*>(expr.get());
		castExpr->targetType = ResolveType(castExpr->targetType);

		return expr;
	}

	ExpressionPtr TransformVisitor::Clone(IdentifierExpression& node)
	{
		const Identifier* identifier = FindIdentifier(node.identifier);
		assert(identifier);
		assert(std::holds_alternative<Variable>(identifier->value));

		auto varExpr = std::make_unique<VariableExpression>();
		varExpr->cachedExpressionType = node.cachedExpressionType;
		varExpr->variableId = std::get<Variable>(identifier->value).varIndex;

		return varExpr;
	}

	ExpressionPtr TransformVisitor::CloneExpression(ExpressionPtr& expr)
	{
		ExpressionPtr exprPtr = AstCloner::CloneExpression(expr);
		if (exprPtr)
		{
			assert(exprPtr->cachedExpressionType);
			*exprPtr->cachedExpressionType = ResolveType(*exprPtr->cachedExpressionType);
		}

		return exprPtr;
	}

	void TransformVisitor::PushScope()
	{
		m_scopeSizes.push_back(m_identifiersInScope.size());
	}

	void TransformVisitor::PopScope()
	{
		assert(!m_scopeSizes.empty());
		m_identifiersInScope.resize(m_scopeSizes.back());
		m_scopeSizes.pop_back();
	}
}
