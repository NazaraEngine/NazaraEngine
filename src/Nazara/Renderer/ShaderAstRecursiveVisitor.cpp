// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAstRecursiveVisitor.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::AssignOp& node)
	{
		Visit(node.left);
		Visit(node.right);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::BinaryOp& node)
	{
		Visit(node.left);
		Visit(node.right);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::Branch& node)
	{
		for (auto& cond : node.condStatements)
		{
			Visit(cond.condition);
			Visit(cond.statement);
		}

		if (node.elseStatement)
			Visit(node.elseStatement);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::Cast& node)
	{
		for (auto& expr : node.expressions)
		{
			if (!expr)
				break;

			Visit(expr);
		}
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::Constant& /*node*/)
	{
		/* Nothing to do */
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::DeclareVariable& node)
	{
		if (node.expression)
			Visit(node.expression);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::Identifier& /*node*/)
	{
		/* Nothing to do */
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::IntrinsicCall& node)
	{
		for (auto& param : node.parameters)
			Visit(param);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::Sample2D& node)
	{
		Visit(node.sampler);
		Visit(node.coordinates);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);
	}

	void ShaderAstRecursiveVisitor::Visit(const ShaderNodes::SwizzleOp& node)
	{
		Visit(node.expression);
	}
}
