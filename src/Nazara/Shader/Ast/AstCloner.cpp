// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionPtr AstCloner::Clone(Expression& expr)
	{
		expr.Visit(*this);

		assert(m_statementStack.empty() && m_expressionStack.size() == 1);
		return PopExpression();
	}

	StatementPtr AstCloner::Clone(Statement& statement)
	{
		statement.Visit(*this);

		assert(m_expressionStack.empty() && m_statementStack.size() == 1);
		return PopStatement();
	}

	ExpressionPtr AstCloner::CloneExpression(Expression& expr)
	{
		expr.Visit(*this);
		return PopExpression();
	}

	StatementPtr AstCloner::CloneStatement(Statement& statement)
	{
		statement.Visit(*this);
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
		clone->condition = CloneExpression(node.condition);
		clone->statement = CloneStatement(node.statement);

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareExternalStatement& node)
	{
		auto clone = std::make_unique<DeclareExternalStatement>();
		clone->varIndex = node.varIndex;

		clone->bindingSet = CloneAttribute(node.bindingSet);

		clone->externalVars.reserve(node.externalVars.size());
		for (const auto& var : node.externalVars)
		{
			auto& cloneVar = clone->externalVars.emplace_back();
			cloneVar.name = var.name;
			cloneVar.type = var.type;
			cloneVar.bindingIndex = CloneAttribute(var.bindingIndex);
			cloneVar.bindingSet = CloneAttribute(var.bindingSet);
		}

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareFunctionStatement& node)
	{
		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->depthWrite = CloneAttribute(node.depthWrite);
		clone->earlyFragmentTests = CloneAttribute(node.earlyFragmentTests);
		clone->entryStage = CloneAttribute(node.entryStage);
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

	StatementPtr AstCloner::Clone(DeclareOptionStatement& node)
	{
		auto clone = std::make_unique<DeclareOptionStatement>();
		clone->initialValue = CloneExpression(node.initialValue);
		clone->optIndex = node.optIndex;
		clone->optName = node.optName;
		clone->optType = node.optType;

		return clone;
	}

	StatementPtr AstCloner::Clone(DeclareStructStatement& node)
	{
		auto clone = std::make_unique<DeclareStructStatement>();
		clone->structIndex = node.structIndex;

		clone->description.layout = CloneAttribute(node.description.layout);
		clone->description.name = node.description.name;

		clone->description.members.reserve(node.description.members.size());
		for (const auto& member : node.description.members)
		{
			auto& cloneMember = clone->description.members.emplace_back();
			cloneMember.name = member.name;
			cloneMember.type = member.type;
			cloneMember.builtin = CloneAttribute(member.builtin);
			cloneMember.cond = CloneAttribute(member.cond);
			cloneMember.locationIndex = CloneAttribute(member.locationIndex);
		}

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

	ExpressionPtr AstCloner::Clone(AccessIdentifierExpression& node)
	{
		auto clone = std::make_unique<AccessIdentifierExpression>();
		clone->identifiers = node.identifiers;
		clone->expr = CloneExpression(node.expr);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(AccessIndexExpression& node)
	{
		auto clone = std::make_unique<AccessIndexExpression>();
		clone->expr = CloneExpression(node.expr);

		clone->indices.reserve(node.indices.size());
		for (auto& parameter : node.indices)
			clone->indices.push_back(CloneExpression(parameter));

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

	ExpressionPtr AstCloner::Clone(CallFunctionExpression& node)
	{
		auto clone = std::make_unique<CallFunctionExpression>();
		clone->targetFunction = node.targetFunction;

		clone->parameters.reserve(node.parameters.size());
		for (auto& parameter : node.parameters)
			clone->parameters.push_back(CloneExpression(parameter));

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(CallMethodExpression& node)
	{
		auto clone = std::make_unique<CallMethodExpression>();
		clone->methodName = node.methodName;

		clone->object = CloneExpression(node.object);

		clone->parameters.reserve(node.parameters.size());
		for (auto& parameter : node.parameters)
			clone->parameters.push_back(CloneExpression(parameter));

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
		clone->condition = CloneExpression(node.condition);
		clone->falsePath = CloneExpression(node.falsePath);
		clone->truePath = CloneExpression(node.truePath);

		clone->cachedExpressionType = node.cachedExpressionType;

		return clone;
	}

	ExpressionPtr AstCloner::Clone(ConstantIndexExpression& node)
	{
		auto clone = std::make_unique<ConstantIndexExpression>();
		clone->constantId = node.constantId;

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

	ExpressionPtr AstCloner::Clone(SelectOptionExpression& node)
	{
		auto clone = std::make_unique<SelectOptionExpression>();
		clone->optionName = node.optionName;
		clone->falsePath = CloneExpression(node.falsePath);
		clone->truePath = CloneExpression(node.truePath);

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

	ExpressionPtr AstCloner::Clone(UnaryExpression& node)
	{
		auto clone = std::make_unique<UnaryExpression>();
		clone->expression = CloneExpression(node.expression);
		clone->op = node.op;

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

#include <Nazara/Shader/Ast/AstNodeList.hpp>

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
