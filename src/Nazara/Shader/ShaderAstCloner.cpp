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

	void AstCloner::Visit(AccessMemberExpression& node)
	{
		auto clone = std::make_unique<AccessMemberExpression>();
		clone->memberIdentifiers = node.memberIdentifiers;
		clone->structExpr = CloneExpression(node.structExpr);

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(AssignExpression& node)
	{
		auto clone = std::make_unique<AssignExpression>();
		clone->op = node.op;
		clone->left = CloneExpression(node.left);
		clone->right = CloneExpression(node.right);

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(BinaryExpression& node)
	{
		auto clone = std::make_unique<BinaryExpression>();
		clone->op = node.op;
		clone->left = CloneExpression(node.left);
		clone->right = CloneExpression(node.right);

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(CastExpression& node)
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

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(ConditionalExpression& node)
	{
		auto clone = std::make_unique<ConditionalExpression>();
		clone->conditionName = node.conditionName;
		clone->falsePath = CloneExpression(node.falsePath);
		clone->truePath = CloneExpression(node.truePath);

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(ConstantExpression& node)
	{
		auto clone = std::make_unique<ConstantExpression>();
		clone->value = node.value;

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(IdentifierExpression& node)
	{
		auto clone = std::make_unique<IdentifierExpression>();
		clone->identifier = node.identifier;

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(IntrinsicExpression& node)
	{
		auto clone = std::make_unique<IntrinsicExpression>();
		clone->intrinsic = node.intrinsic;

		clone->parameters.reserve(node.parameters.size());
		for (auto& parameter : node.parameters)
			clone->parameters.push_back(CloneExpression(parameter));

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(SwizzleExpression& node)
	{
		auto clone = std::make_unique<SwizzleExpression>();
		clone->componentCount = node.componentCount;
		clone->components = node.components;
		clone->expression = CloneExpression(node.expression);

		PushExpression(std::move(clone));
	}

	void AstCloner::Visit(BranchStatement& node)
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

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(ConditionalStatement& node)
	{
		auto clone = std::make_unique<ConditionalStatement>();
		clone->conditionName = node.conditionName;
		clone->statement = CloneStatement(node.statement);

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(DeclareFunctionStatement& node)
	{
		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->attributes = node.attributes;
		clone->name = node.name;
		clone->parameters = node.parameters;
		clone->returnType = node.returnType;

		clone->statements.reserve(node.statements.size());
		for (auto& statement : node.statements)
			clone->statements.push_back(CloneStatement(statement));

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(DeclareStructStatement& node)
	{
		auto clone = std::make_unique<DeclareStructStatement>();
		clone->description = node.description;

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(DeclareVariableStatement& node)
	{
		auto clone = std::make_unique<DeclareVariableStatement>();
		clone->varName = node.varName;
		clone->varType = node.varType;
		clone->initialExpression = CloneExpression(node.initialExpression);

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(DiscardStatement& /*node*/)
	{
		PushStatement(std::make_unique<DiscardStatement>());
	}

	void AstCloner::Visit(ExpressionStatement& node)
	{
		auto clone = std::make_unique<ExpressionStatement>();
		clone->expression = CloneExpression(node.expression);

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(MultiStatement& node)
	{
		auto clone = std::make_unique<MultiStatement>();
		clone->statements.reserve(node.statements.size());
		for (auto& statement : node.statements)
			clone->statements.push_back(CloneStatement(statement));

		PushStatement(std::move(clone));
	}

	void AstCloner::Visit(NoOpStatement& /*node*/)
	{
		PushStatement(std::make_unique<NoOpStatement>());
	}

	void AstCloner::Visit(ReturnStatement& node)
	{
		auto clone = std::make_unique<ReturnStatement>();
		clone->returnExpr = CloneExpression(node.returnExpr);

		PushStatement(std::move(clone));
	}

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
