// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	void AstRecursiveVisitor::Visit(AccessIdentifierExpression& node)
	{
		node.expr->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(AccessIndexExpression& node)
	{
		node.expr->Visit(*this);
		for (auto& index : node.indices)
			index->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(AliasValueExpression& /*node*/)
	{
		/* nothing to do */
	}

	void AstRecursiveVisitor::Visit(AssignExpression& node)
	{
		node.left->Visit(*this);
		node.right->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(BinaryExpression& node)
	{
		node.left->Visit(*this);
		node.right->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(CallFunctionExpression& node)
	{
		for (auto& param : node.parameters)
			param->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(CallMethodExpression& node)
	{
		node.object->Visit(*this);

		for (auto& param : node.parameters)
			param->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(CastExpression& node)
	{
		for (auto& expr : node.expressions)
		{
			if (!expr)
				break;

			expr->Visit(*this);
		}
	}

	void AstRecursiveVisitor::Visit(ConditionalExpression& node)
	{
		node.truePath->Visit(*this);
		node.falsePath->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(ConstantValueExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(ConstantExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(FunctionExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(IdentifierExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(IntrinsicExpression& node)
	{
		for (auto& param : node.parameters)
			param->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(IntrinsicFunctionExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(StructTypeExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(SwizzleExpression& node)
	{
		if (node.expression)
			node.expression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(TypeExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(VariableValueExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(UnaryExpression& node)
	{
		if (node.expression)
			node.expression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(BranchStatement& node)
	{
		for (auto& cond : node.condStatements)
		{
			cond.condition->Visit(*this);
			cond.statement->Visit(*this);
		}

		if (node.elseStatement)
			node.elseStatement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(ConditionalStatement& node)
	{
		node.statement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(DeclareAliasStatement& node)
	{
		if (node.expression)
			node.expression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(DeclareConstStatement& node)
	{
		if (node.expression)
			node.expression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(DeclareExternalStatement& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(DeclareFunctionStatement& node)
	{
		for (auto& statement : node.statements)
			statement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(DeclareOptionStatement& node)
	{
		if (node.defaultValue)
			node.defaultValue->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(DeclareStructStatement& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(DeclareVariableStatement& node)
	{
		if (node.initialExpression)
			node.initialExpression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(DiscardStatement& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(ExpressionStatement& node)
	{
		node.expression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(ForStatement& node)
	{
		if (node.fromExpr)
			node.fromExpr->Visit(*this);

		if (node.toExpr)
			node.toExpr->Visit(*this);

		if (node.stepExpr)
			node.stepExpr->Visit(*this);

		if (node.statement)
			node.statement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(ForEachStatement& node)
	{
		if (node.expression)
			node.expression->Visit(*this);

		if (node.statement)
			node.statement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(ImportStatement& /*node*/)
	{
		/* nothing to do */
	}

	void AstRecursiveVisitor::Visit(MultiStatement& node)
	{
		for (auto& statement : node.statements)
			statement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(NoOpStatement& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(ReturnStatement& node)
	{
		if (node.returnExpr)
			node.returnExpr->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(ScopedStatement& node)
	{
		if (node.statement)
			node.statement->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(WhileStatement& node)
	{
		if (node.condition)
			node.condition->Visit(*this);

		if (node.body)
			node.body->Visit(*this);
	}
}
