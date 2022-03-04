// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstConstantPropagationVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline ExpressionPtr AstConstantPropagationVisitor::Process(Expression& expression)
	{
		m_options = {};
		return CloneExpression(expression);
	}

	inline ExpressionPtr AstConstantPropagationVisitor::Process(Expression& expression, const Options& options)
	{
		m_options = options;
		return CloneExpression(expression);
	}

	inline StatementPtr AstConstantPropagationVisitor::Process(Statement& statement)
	{
		m_options = {};
		return CloneStatement(statement);
	}

	inline StatementPtr AstConstantPropagationVisitor::Process(Statement& statement, const Options& options)
	{
		m_options = options;
		return CloneStatement(statement);
	}

	inline ExpressionPtr PropagateConstants(Expression& ast)
	{
		AstConstantPropagationVisitor optimize;
		return optimize.Process(ast);
	}

	inline ExpressionPtr PropagateConstants(Expression& ast, const AstConstantPropagationVisitor::Options& options)
	{
		AstConstantPropagationVisitor optimize;
		return optimize.Process(ast, options);
	}

	inline ModulePtr PropagateConstants(const Module& shaderModule)
	{
		AstConstantPropagationVisitor optimize;
		return optimize.Process(shaderModule);
	}

	inline ModulePtr PropagateConstants(const Module& shaderModule, const AstConstantPropagationVisitor::Options& options)
	{
		AstConstantPropagationVisitor optimize;
		return optimize.Process(shaderModule, options);
	}

	inline StatementPtr PropagateConstants(Statement& ast)
	{
		AstConstantPropagationVisitor optimize;
		return optimize.Process(ast);
	}

	inline StatementPtr PropagateConstants(Statement& ast, const AstConstantPropagationVisitor::Options& options)
	{
		AstConstantPropagationVisitor optimize;
		return optimize.Process(ast, options);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
