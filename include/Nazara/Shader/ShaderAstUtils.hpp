// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTUTILS_HPP
#define NAZARA_SHADERASTUTILS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitor.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API ShaderAstValueCategory final : public AstExpressionVisitor
	{
		public:
			ShaderAstValueCategory() = default;
			ShaderAstValueCategory(const ShaderAstValueCategory&) = delete;
			ShaderAstValueCategory(ShaderAstValueCategory&&) = delete;
			~ShaderAstValueCategory() = default;

			ExpressionCategory GetExpressionCategory(Expression& expression);

			ShaderAstValueCategory& operator=(const ShaderAstValueCategory&) = delete;
			ShaderAstValueCategory& operator=(ShaderAstValueCategory&&) = delete;

		private:
			using AstExpressionVisitor::Visit;

			void Visit(AccessMemberIdentifierExpression& node) override;
			void Visit(AccessMemberIndexExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SwizzleExpression& node) override;
			void Visit(VariableExpression& node) override;

			ExpressionCategory m_expressionCategory;
	};

	inline ExpressionCategory GetExpressionCategory(Expression& expression);
}

#include <Nazara/Shader/ShaderAstUtils.inl>

#endif
