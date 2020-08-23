// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvStatementVisitor.hpp>
#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	void SpirvStatementVisitor::Visit(ShaderNodes::AssignOp& node)
	{
		SpirvExpressionLoad loadVisitor(m_writer);
		SpirvExpressionStore storeVisitor(m_writer);
		storeVisitor.Store(node.left, loadVisitor.EvaluateExpression(node.right));
	}

	void SpirvStatementVisitor::Visit(ShaderNodes::Branch& node)
	{
		throw std::runtime_error("not yet implemented");
	}

	void SpirvStatementVisitor::Visit(ShaderNodes::DeclareVariable& node)
	{
		if (node.expression)
		{
			assert(node.variable->GetType() == ShaderNodes::VariableType::LocalVariable);

			const auto& localVar = static_cast<const ShaderNodes::LocalVariable&>(*node.variable);

			SpirvExpressionLoad loadVisitor(m_writer);
			m_writer.WriteLocalVariable(localVar.name, loadVisitor.EvaluateExpression(node.expression));
		}
	}

	void SpirvStatementVisitor::Visit(ShaderNodes::ExpressionStatement& node)
	{
		SpirvExpressionLoad loadVisitor(m_writer);
		loadVisitor.Visit(node.expression);
	}

	void SpirvStatementVisitor::Visit(ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);
	}
}
