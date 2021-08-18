// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstOptimizer.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline ExpressionPtr AstOptimizer::Optimise(Expression& expression)
	{
		m_options = {};
		return CloneExpression(expression);
	}

	inline ExpressionPtr AstOptimizer::Optimise(Expression& expression, const Options& options)
	{
		m_options = options;
		return CloneExpression(expression);
	}

	inline StatementPtr AstOptimizer::Optimise(Statement& statement)
	{
		m_options = {};
		return CloneStatement(statement);
	}

	inline StatementPtr AstOptimizer::Optimise(Statement& statement, const Options& options)
	{
		m_options = options;
		return CloneStatement(statement);
	}

	inline ExpressionPtr Optimize(Expression& ast)
	{
		AstOptimizer optimize;
		return optimize.Optimise(ast);
	}

	inline ExpressionPtr Optimize(Expression& ast, const AstOptimizer::Options& options)
	{
		AstOptimizer optimize;
		return optimize.Optimise(ast, options);
	}

	inline StatementPtr Optimize(Statement& ast)
	{
		AstOptimizer optimize;
		return optimize.Optimise(ast);
	}

	inline StatementPtr Optimize(Statement& ast, const AstOptimizer::Options& options)
	{
		AstOptimizer optimize;
		return optimize.Optimise(ast, options);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
