// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionPtr AstCloner::Clone(ExpressionPtr& expr)
	{
		expr->Visit(*this);

		assert(m_statementStack.empty() && m_expressionStack.size() == 1);
		return PopExpression();
	}

	StatementPtr AstCloner::Clone(StatementPtr& statement)
	{
		statement->Visit(*this);

		assert(m_expressionStack.empty() && m_statementStack.size() == 1);
		return PopStatement();
	}

	ExpressionPtr AstCloner::CloneExpression(ExpressionPtr& expr)
	{
		if (!expr)
			return nullptr;

		expr->Visit(*this);
		return PopExpression();
	}

	StatementPtr AstCloner::CloneStatement(StatementPtr& statement)
	{
		if (!statement)
			return nullptr;

		statement->Visit(*this);
		return PopStatement();
	}

	StatementPtr AstCloner::Clone(BranchStatement& node)
	{
		auto clone = std::make_unique<BranchStatement>();
		clone->condStatements.reserve(node.condStatements.size());

		for (auto& cond : node.condStatements)
		{
			auto& condStatement = clone->condStatements.emplace_back();
			condStatement.condition = CloneExpression(cond.condition);
			condStatement.statement = CloneStatement(cond.statement);
		}

		clone->elseStatement = CloneStatement(node.elseStatement);

		return clone;
	}

	StatementPtr AstCloner::Clone(ConditionalStatement& node)
	{
		auto clone = std::make_unique<ConditionalStatement>();
		clone->conditionName = node.conditionName;
		clone->statement = CloneStatement(node.statement);

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareExternalStatement& node)
	{
		auto clone = std::make_unique<DeclareExternalStatement>();
		clone->externalVars = node.externalVars;
		clone->varIndex = node.varIndex;

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareFunctionStatement& node)
	{
		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->entryStage = node.entryStage;
		clone->funcIndex = node.funcIndex;
		clone->name = node.name;
		clone->parameters = node.parameters;
		clone->returnType = node.returnType;
		clone->varIndex = node.varIndex;

		clone->statements.reserve(node.statements.size());
		for (auto& statement : node.statements)
			clone->statements.push_back(CloneStatement(statement));

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareStructStatement& node)
	{
		auto clone = std::make_unique<DeclareStructStatement>();
		clone->structIndex = node.structIndex;
		clone->description = node.description;

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareVariableStatement& node)
	{
		auto clone = std::make_unique<DeclareVariableStatement>();
		clone->varIndex = node.varIndex;
		clone->varName = node.varName;
		clone->varType = node.varType;
		clone->initialExpression = CloneExpression(node.initialExpression);

		return clone;
	}

	StatementPtr AstCloner::Clone(DiscardStatement& /*node*/)
	{
		return std::make_unique<DiscardStatement>();
	}

	StatementPtr AstCloner::Clone(ExpressionStatement& node)
	{
		auto clone = std::make_unique<ExpressionStatement>();
		clone->expression = CloneExpression(node.expression);

		return clone;
	}

	StatementPtr AstCloner::Clone(MultiStatement& node)
	{
		auto clone = std::make_unique<MultiStatement>();
		clone->statements.reserve(node.statements.size());
		for (auto& statement : node.statements)
			clone->statements.push_back(CloneStatement(statement));

		return clone;
	}

	StatementPtr AstCloner::Clone(NoOpStatement& /*node*/)
	{
		return std::make_unique<NoOpStatement>();
	}

	StatementPtr AstCloner::Clone(ReturnStatement& node)
	{
		auto clone = std::make_unique<ReturnStatement>();
		clone->returnExpr = CloneExpression(node.returnExpr);

		return clone;
	}

	ExpressionPtr AstCloner::Clone(AccessMemberIdentifierExpression& node)
	{
		auto clone = std::make_unique<AccessMemberIdentifierExpression>();
		clone->memberIdentifiers = node.memberIdentifiers;
		clone->structExpr = CloneExpression(node.structExpr);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(AccessMemberIndexExpression& node)
	{
		auto clone = std::make_unique<AccessMemberIndexExpression>();
		clone->memberIndices = node.memberIndices;
		clone->structExpr = CloneExpression(node.structExpr);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(AssignExpression& node)
	{
		auto clone = std::make_unique<AssignExpression>();
		clone->op = node.op;
		clone->left = CloneExpression(node.left);
		clone->right = CloneExpression(node.right);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(BinaryExpression& node)
	{
		auto clone = std::make_unique<BinaryExpression>();
		clone->op = node.op;
		clone->left = CloneExpression(node.left);
		clone->right = CloneExpression(node.right);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(CastExpression& node)
	{
		auto clone = std::make_unique<CastExpression>();
		clone->targetType = node.targetType;

		std::size_t expressionCount = 0;
		for (auto& expr : node.expressions)
		{
			if (!expr)
				break;

			clone->expressions[expressionCount++] = CloneExpression(expr);
		}

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(ConditionalExpression& node)
	{
		auto clone = std::make_unique<ConditionalExpression>();
		clone->conditionName = node.conditionName;
		clone->falsePath = CloneExpression(node.falsePath);
		clone->truePath = CloneExpression(node.truePath);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(ConstantExpression& node)
	{
		auto clone = std::make_unique<ConstantExpression>();
		clone->value = node.value;

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(IdentifierExpression& node)
	{
		auto clone = std::make_unique<IdentifierExpression>();
		clone->identifier = node.identifier;

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(IntrinsicExpression& node)
	{
		auto clone = std::make_unique<IntrinsicExpression>();
		clone->intrinsic = node.intrinsic;

		clone->parameters.reserve(node.parameters.size());
		for (auto& parameter : node.parameters)
			clone->parameters.push_back(CloneExpression(parameter));

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(SwizzleExpression& node)
	{
		auto clone = std::make_unique<SwizzleExpression>();
		clone->componentCount = node.componentCount;
		clone->components = node.components;
		clone->expression = CloneExpression(node.expression);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(VariableExpression& node)
	{
		auto clone = std::make_unique<VariableExpression>();
		clone->variableId = node.variableId;

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

#define NAZARA_SHADERAST_EXPRESSION(NodeType) void AstCloner::Visit(NodeType& node) \
	{ \
		PushExpression(Clone(node)); \
	}

#define NAZARA_SHADERAST_STATEMENT(NodeType) void AstCloner::Visit(NodeType& node) \
	{ \
		PushStatement(Clone(node)); \
	}

#include <Nazara/Shader/ShaderAstNodes.hpp>

	void AstCloner::PushExpression(ExpressionPtr expression)
	{
		m_expressionStack.emplace_back(std::move(expression));
	}

	void AstCloner::PushStatement(StatementPtr statement)
	{
		m_statementStack.emplace_back(std::move(statement));
	}

	ExpressionPtr AstCloner::PopExpression()
	{
		assert(!m_expressionStack.empty());

		ExpressionPtr expr = std::move(m_expressionStack.back());
		m_expressionStack.pop_back();

		return expr;
	}

	StatementPtr AstCloner::PopStatement()
	{
		assert(!m_statementStack.empty());

		StatementPtr expr = std::move(m_statementStack.back());
		m_statementStack.pop_back();

		return expr;
	}
}
