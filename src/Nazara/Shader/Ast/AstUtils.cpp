// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionCategory ShaderAstValueCategory::GetExpressionCategory(Expression& expression)
	{
		expression.Visit(*this);
		return m_expressionCategory;
	}

	void ShaderAstValueCategory::Visit(AccessIdentifierExpression& node)
	{
		node.expr->Visit(*this);
	}

	void ShaderAstValueCategory::Visit(AccessIndexExpression& node)
	{
		node.expr->Visit(*this);
	}

	void ShaderAstValueCategory::Visit(AssignExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(BinaryExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(CallFunctionExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(CallMethodExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(CastExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ConditionalExpression& node)
	{
		node.truePath->Visit(*this);
		ExpressionCategory trueExprCategory = m_expressionCategory;

		node.falsePath->Visit(*this);
		ExpressionCategory falseExprCategory = m_expressionCategory;

		if (trueExprCategory == ExpressionCategory::RValue || falseExprCategory == ExpressionCategory::RValue)
			m_expressionCategory = ExpressionCategory::RValue;
		else
			m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(ConstantValueExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ConstantExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(FunctionExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(IdentifierExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(IntrinsicExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(IntrinsicFunctionExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(StructTypeExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(SwizzleExpression& node)
	{
		if (IsPrimitiveType(GetExpressionType(node)) && node.componentCount > 1)
			// Swizzling more than a component on a primitive produces a rvalue (a.xxxx cannot be assigned)
			m_expressionCategory = ExpressionCategory::RValue;
		else
		{
			bool isRVaLue = false;

			std::array<bool, 4> used;
			used.fill(false);

			for (std::size_t i = 0; i < node.componentCount; ++i)
			{
				if (used[node.components[i]])
				{
					// Swizzling the same component multiple times produces a rvalue (a.xx cannot be assigned)
					isRVaLue = true;
					break;
				}

				used[node.components[i]] = true;
			}

			if (isRVaLue)
				m_expressionCategory = ExpressionCategory::RValue;
			else
				node.expression->Visit(*this);
		}
	}

	void ShaderAstValueCategory::Visit(VariableValueExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(UnaryExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}
}
