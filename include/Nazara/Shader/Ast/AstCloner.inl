// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	template<typename T>
	AttributeValue<T> AstCloner::CloneAttribute(const AttributeValue<T>& attribute)
	{
		if (!attribute.HasValue())
			return {};

		if (attribute.IsExpression())
			return CloneExpression(attribute.GetExpression());
		else
		{
			assert(attribute.IsResultingValue());
			return attribute.GetResultingValue();
		}
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
