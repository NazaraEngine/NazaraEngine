// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstRecursiveVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::AssignOp& node)
	{
		Visit(node.left);
		Visit(node.right);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::BinaryOp& node)
	{
		Visit(node.left);
		Visit(node.right);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::Branch& node)
	{
		for (auto& cond : node.condStatements)
		{
			Visit(cond.condition);
			Visit(cond.statement);
		}

		if (node.elseStatement)
			Visit(node.elseStatement);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::Cast& node)
	{
		for (auto& expr : node.expressions)
		{
			if (!expr)
				break;

			Visit(expr);
		}
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::ConditionalExpression& node)
	{
		Visit(node.truePath);
		Visit(node.falsePath);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::ConditionalStatement& node)
	{
		Visit(node.statement);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::Constant& /*node*/)
	{
		/* Nothing to do */
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::DeclareVariable& node)
	{
		if (node.expression)
			Visit(node.expression);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::Identifier& /*node*/)
	{
		/* Nothing to do */
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::IntrinsicCall& node)
	{
		for (auto& param : node.parameters)
			Visit(param);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::Sample2D& node)
	{
		Visit(node.sampler);
		Visit(node.coordinates);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);
	}

	void ShaderAstRecursiveVisitor::Visit(ShaderNodes::SwizzleOp& node)
	{
		Visit(node.expression);
	}
}
