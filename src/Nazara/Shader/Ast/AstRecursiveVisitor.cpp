// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	void AstRecursiveVisitor::Visit(AccessMemberIdentifierExpression& node)
	{
		node.structExpr->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(AccessMemberIndexExpression& node)
	{
		node.structExpr->Visit(*this);
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

	void AstRecursiveVisitor::Visit(ConstantExpression& /*node*/)
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

	void AstRecursiveVisitor::Visit(SelectOptionExpression& node)
	{
		node.truePath->Visit(*this);
		node.falsePath->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(SwizzleExpression& node)
	{
		node.expression->Visit(*this);
	}

	void AstRecursiveVisitor::Visit(VariableExpression& /*node*/)
	{
		/* Nothing to do */
	}

	void AstRecursiveVisitor::Visit(UnaryExpression& node)
	{
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
		if (node.initialValue)
			node.initialValue->Visit(*this);
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
}
