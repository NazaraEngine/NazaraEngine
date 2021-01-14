// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstUtils.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	ShaderNodes::ExpressionCategory ShaderAstValueCategory::GetExpressionCategory(const ShaderNodes::ExpressionPtr& expression)
	{
		Visit(expression);
		return m_expressionCategory;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::AssignOp& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::BinaryOp& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::Cast& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::ConditionalExpression& node)
	{
		Visit(node.truePath);
		ShaderNodes::ExpressionCategory trueExprCategory = m_expressionCategory;
		Visit(node.falsePath);
		ShaderNodes::ExpressionCategory falseExprCategory = m_expressionCategory;

		if (trueExprCategory == ShaderNodes::ExpressionCategory::RValue || falseExprCategory == ShaderNodes::ExpressionCategory::RValue)
			m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
		else
			m_expressionCategory = ShaderNodes::ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::Constant& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::Identifier& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::IntrinsicCall& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::Sample2D& node)
	{
		m_expressionCategory = ShaderNodes::ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ShaderNodes::SwizzleOp& node)
	{
		Visit(node.expression);
	}
}
