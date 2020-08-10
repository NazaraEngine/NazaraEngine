// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	ShaderNodes::StatementPtr ShaderAstCloner::Clone(const ShaderNodes::StatementPtr& statement)
	{
		ShaderAstVisitor::Visit(statement);

		if (!m_expressionStack.empty() || !m_variableStack.empty() || m_statementStack.size() != 1)
			throw std::runtime_error("An error occurred during clone");

		return PopStatement();
	}

	ShaderNodes::ExpressionPtr ShaderAstCloner::CloneExpression(const ShaderNodes::ExpressionPtr& expr)
	{
		if (!expr)
			return nullptr;

		ShaderAstVisitor::Visit(expr);
		return PopExpression();
	}

	ShaderNodes::StatementPtr ShaderAstCloner::CloneStatement(const ShaderNodes::StatementPtr& statement)
	{
		if (!statement)
			return nullptr;

		ShaderAstVisitor::Visit(statement);
		return PopStatement();
	}

	ShaderNodes::VariablePtr ShaderAstCloner::CloneVariable(const ShaderNodes::VariablePtr& variable)
	{
		if (!variable)
			return nullptr;

		ShaderVarVisitor::Visit(variable);
		return PopVariable();
	}

	void ShaderAstCloner::Visit(ShaderNodes::AccessMember& node)
	{
		PushExpression(ShaderNodes::AccessMember::Build(CloneExpression(node.structExpr), node.memberIndex, node.exprType));
	}

	void ShaderAstCloner::Visit(ShaderNodes::AssignOp& node)
	{
		PushExpression(ShaderNodes::AssignOp::Build(node.op, CloneExpression(node.left), CloneExpression(node.right)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::BinaryOp& node)
	{
		PushExpression(ShaderNodes::BinaryOp::Build(node.op, CloneExpression(node.left), CloneExpression(node.right)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::Branch& node)
	{
		std::vector<ShaderNodes::Branch::ConditionalStatement> condStatements;
		condStatements.reserve(node.condStatements.size());

		for (auto& cond : node.condStatements)
		{
			auto& condStatement = condStatements.emplace_back();
			condStatement.condition = CloneExpression(cond.condition);
			condStatement.statement = CloneStatement(cond.statement);
		}

		PushStatement(ShaderNodes::Branch::Build(std::move(condStatements), CloneStatement(node.elseStatement)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::Cast& node)
	{
		std::size_t expressionCount = 0;
		std::array<ShaderNodes::ExpressionPtr, 4> expressions;
		for (auto& expr : node.expressions)
		{
			if (!expr)
				break;

			expressions[expressionCount] = CloneExpression(expr);
			expressionCount++;
		}

		PushExpression(ShaderNodes::Cast::Build(node.exprType, expressions.data(), expressionCount));
	}

	void ShaderAstCloner::Visit(ShaderNodes::Constant& node)
	{
		PushExpression(ShaderNodes::Constant::Build(node.value));
	}

	void ShaderAstCloner::Visit(ShaderNodes::DeclareVariable& node)
	{
		PushStatement(ShaderNodes::DeclareVariable::Build(CloneVariable(node.variable), CloneExpression(node.expression)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::ExpressionStatement& node)
	{
		PushStatement(ShaderNodes::ExpressionStatement::Build(CloneExpression(node.expression)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::Identifier& node)
	{
		PushExpression(ShaderNodes::Identifier::Build(CloneVariable(node.var)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::IntrinsicCall& node)
	{
		std::vector<ShaderNodes::ExpressionPtr> parameters;
		parameters.reserve(node.parameters.size());

		for (auto& parameter : node.parameters)
			parameters.push_back(CloneExpression(parameter));

		PushExpression(ShaderNodes::IntrinsicCall::Build(node.intrinsic, std::move(parameters)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::Sample2D& node)
	{
		PushExpression(ShaderNodes::Sample2D::Build(CloneExpression(node.sampler), CloneExpression(node.coordinates)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::StatementBlock& node)
	{
		std::vector<ShaderNodes::StatementPtr> statements;
		statements.reserve(node.statements.size());

		for (auto& statement : node.statements)
			statements.push_back(CloneStatement(statement));

		PushStatement(ShaderNodes::StatementBlock::Build(std::move(statements)));
	}

	void ShaderAstCloner::Visit(ShaderNodes::SwizzleOp& node)
	{
		PushExpression(ShaderNodes::SwizzleOp::Build(PopExpression(), node.components.data(), node.componentCount));
	}

	void ShaderAstCloner::Visit(ShaderNodes::BuiltinVariable& var)
	{
		PushVariable(ShaderNodes::BuiltinVariable::Build(var.entry, var.type));
	}

	void ShaderAstCloner::Visit(ShaderNodes::InputVariable& var)
	{
		PushVariable(ShaderNodes::InputVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(ShaderNodes::LocalVariable& var)
	{
		PushVariable(ShaderNodes::LocalVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(ShaderNodes::OutputVariable& var)
	{
		PushVariable(ShaderNodes::OutputVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(ShaderNodes::ParameterVariable& var)
	{
		PushVariable(ShaderNodes::ParameterVariable::Build(var.name, var.type));
	}

	void ShaderAstCloner::Visit(ShaderNodes::UniformVariable& var)
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
