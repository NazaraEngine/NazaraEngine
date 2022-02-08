// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	template<typename T>
	ExpressionValue<T> AstCloner::Clone(const ExpressionValue<T>& expressionValue)
	{
		if (!expressionValue.HasValue())
			return {};

		if (expressionValue.IsExpression())
			return CloneExpression(expressionValue.GetExpression());
		else
		{
			assert(expressionValue.IsResultingValue());
			return expressionValue.GetResultingValue();
		}
	}

	inline ExpressionValue<ExpressionType> AstCloner::Clone(const ExpressionValue<ExpressionType>& expressionValue)
	{
		return CloneType(expressionValue);
	}

	ExpressionPtr AstCloner::CloneExpression(const ExpressionPtr& expr)
	{
		if (!expr)
			return nullptr;

		return CloneExpression(*expr);
	}

	StatementPtr AstCloner::CloneStatement(const StatementPtr& statement)
	{
		if (!statement)
			return nullptr;

		return CloneStatement(*statement);
	}


	template<typename T>
	ExpressionValue<T> Clone(const ExpressionValue<T>& attribute)
	{
		AstCloner cloner;
		return cloner.Clone(attribute);
	}

	inline ExpressionPtr Clone(Expression& node)
	{
		AstCloner cloner;
		return cloner.Clone(node);
	}

	inline StatementPtr Clone(Statement& node)
	{
		AstCloner cloner;
		return cloner.Clone(node);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
