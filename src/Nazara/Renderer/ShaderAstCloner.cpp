// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAstCloner.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	ShaderNodes::StatementPtr ShaderAstCloner::Clone(const ShaderNodes::StatementPtr& statement)
	{
		ShaderAstVisitor::Visit(statement);

		if (!m_expressionStack.empty() || !m_variableStack.empty() || m_statementStack.size() != 1)
			throw std::runtime_error("An error occured during clone");

		return PopStatement();
	}

	void ShaderAstCloner::Visit(const ShaderNodes::ExpressionPtr& expr)
	{
		if (expr)
			ShaderAstVisitor::Visit(expr);
		else
			PushExpression(nullptr);
	}

	void ShaderAstCloner::Visit(const ShaderNodes::StatementPtr& statement)
	{
		if (statement)
			ShaderAstVisitor::Visit(statement);
		else
			PushStatement(nullptr);
	}

	void ShaderAstCloner::Visit(const ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);

		PushExpression(ShaderNodes::AccessMember::Build(PopExpression(), node.memberIndex, node.exprType));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::AssignOp& node)
	{
		Visit(node.left);
		Visit(node.right);

		auto right = PopExpression();
		auto left = PopExpression();

		PushExpression(ShaderNodes::AssignOp::Build(node.op, std::move(left), std::move(right)));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::BinaryOp& node)
	{
		Visit(node.left);
		Visit(node.right);

		auto right = PopExpression();
		auto left = PopExpression();

		PushExpression(ShaderNodes::BinaryOp::Build(node.op, std::move(left), std::move(right)));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::Branch& node)
	{
		for (auto& cond : node.condStatements)
		{
			Visit(cond.condition);
			Visit(cond.statement);
		}

		Visit(node.elseStatement);

		auto elseStatement = PopStatement();

		std::vector<ShaderNodes::Branch::ConditionalStatement> condStatements(node.condStatements.size());
		for (std::size_t i = 0; i < condStatements.size(); ++i)
		{
			auto& condStatement = condStatements[condStatements.size() - i - 1];
			condStatement.condition = PopExpression();
			condStatement.statement = PopStatement();
		}

		PushStatement(ShaderNodes::Branch::Build(std::move(condStatements), std::move(elseStatement)));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::Cast& node)
	{
		std::size_t expressionCount = 0;
		std::array<ShaderNodes::ExpressionPtr, 4> expressions;
		for (const auto& expr : node.expressions)
		{
			Visit(expr);
			expressionCount++;
		}

		for (std::size_t i = 0; i < expressionCount; ++i)
			expressions[expressionCount - i - 1] = PopExpression();

		PushExpression(ShaderNodes::Cast::Build(node.exprType, expressions.data(), expressionCount));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::Constant& node)
	{
		PushExpression(ShaderNodes::Constant::Build(node.value));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::DeclareVariable& node)
	{
		Visit(node.expression);
		Visit(node.variable);

		PushStatement(ShaderNodes::DeclareVariable::Build(PopVariable(), PopExpression()));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);

		PushStatement(ShaderNodes::ExpressionStatement::Build(PopExpression()));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::Identifier& node)
	{
		Visit(node.var);

		PushExpression(ShaderNodes::Identifier::Build(PopVariable()));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::IntrinsicCall& node)
	{
		for (auto& parameter : node.parameters)
			Visit(parameter);

		std::vector<ShaderNodes::ExpressionPtr> parameters(node.parameters.size());
		for (std::size_t i = 0; i < parameters.size(); ++i)
			parameters[parameters.size() - i - 1] = PopExpression();

		PushExpression(ShaderNodes::IntrinsicCall::Build(node.intrinsic, std::move(parameters)));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::Sample2D& node)
	{
		Visit(node.coordinates);
		Visit(node.sampler);

		auto sampler = PopExpression();
		auto coordinates = PopExpression();

		PushExpression(ShaderNodes::Sample2D::Build(std::move(sampler), std::move(coordinates)));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);

		std::vector<ShaderNodes::StatementPtr> statements(node.statements.size());
		for (std::size_t i = 0; i < statements.size(); ++i)
			statements[statements.size() - i - 1] = PopStatement();

		PushStatement(ShaderNodes::StatementBlock::Build(std::move(statements)));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::SwizzleOp& node)
	{
		PushExpression(ShaderNodes::SwizzleOp::Build(PopExpression(), node.components.data(), node.componentCount));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::BuiltinVariable& var)
	{
		PushVariable(ShaderNodes::BuiltinVariable::Build(var.entry, var.type));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::InputVariable& var)
	{
		PushVariable(ShaderNodes::InputVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::LocalVariable& var)
	{
		PushVariable(ShaderNodes::LocalVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::OutputVariable& var)
	{
		PushVariable(ShaderNodes::OutputVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::ParameterVariable& var)
	{
		PushVariable(ShaderNodes::ParameterVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(const ShaderNodes::UniformVariable& var)
	{
		PushVariable(ShaderNodes::UniformVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::PushExpression(ShaderNodes::ExpressionPtr expression)
	{
		m_expressionStack.emplace_back(std::move(expression));
	}

	void ShaderAstCloner::PushStatement(ShaderNodes::StatementPtr statement)
	{
		m_statementStack.emplace_back(std::move(statement));
	}

	void ShaderAstCloner::PushVariable(ShaderNodes::VariablePtr variable)
	{
		m_variableStack.emplace_back(std::move(variable));
	}

	ShaderNodes::ExpressionPtr ShaderAstCloner::PopExpression()
	{
		assert(!m_expressionStack.empty());

		ShaderNodes::ExpressionPtr expr = std::move(m_expressionStack.back());
		m_expressionStack.pop_back();

		return expr;
	}

	ShaderNodes::StatementPtr ShaderAstCloner::PopStatement()
	{
		assert(!m_statementStack.empty());

		ShaderNodes::StatementPtr expr = std::move(m_statementStack.back());
		m_statementStack.pop_back();

		return expr;
	}

	ShaderNodes::VariablePtr ShaderAstCloner::PopVariable()
	{
		assert(!m_variableStack.empty());

		ShaderNodes::VariablePtr var = std::move(m_variableStack.back());
		m_variableStack.pop_back();

		return var;
	}
}
