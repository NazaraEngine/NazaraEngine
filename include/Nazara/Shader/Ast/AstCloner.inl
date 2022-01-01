// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	template<typename T>
	AttributeValue<T> AstCloner::Clone(const AttributeValue<T>& attribute)
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


	template<typename T>
	AttributeValue<T> Clone(const AttributeValue<T>& attribute)
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
