// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/Nodes.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline const ExpressionType& GetExpressionType(Expression& expr)
	{
		assert(expr.cachedExpressionType);
		return expr.cachedExpressionType.value();
	}

	inline ExpressionType& GetExpressionTypeMut(Expression& expr)
	{
		assert(expr.cachedExpressionType);
		return expr.cachedExpressionType.value();
	}

	inline const ExpressionType& ResolveAlias(const ExpressionType& exprType)
	{
		if (IsAliasType(exprType))
		{
			const AliasType& alias = std::get<AliasType>(exprType);
			return alias.targetType->type;
		}
		else
			return exprType;
	}

	inline bool IsExpression(NodeType nodeType)
	{
		switch (nodeType)
		{
#define NAZARA_SHADERAST_EXPRESSION(Node) case NodeType::Node: return true;
#include <Nazara/Shader/Ast/AstNodeList.hpp>

		default:
			return false;
		}
	}

	inline bool IsStatement(NodeType nodeType)
	{
		switch (nodeType)
		{
#define NAZARA_SHADERAST_STATEMENT(Node) case NodeType::Node: return true;
#include <Nazara/Shader/Ast/AstNodeList.hpp>

		default:
			return false;
		}
	}
}

#include <Nazara/Shader/DebugOff.hpp>
