// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_UTILS_HPP
#define NAZARA_SHADER_AST_UTILS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>
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

			void Visit(AccessIdentifierExpression& node) override;
			void Visit(AccessIndexExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CallFunctionExpression& node) override;
			void Visit(CallMethodExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(ConstantIndexExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SelectOptionExpression& node) override;
			void Visit(SwizzleExpression& node) override;
			void Visit(VariableExpression& node) override;
			void Visit(UnaryExpression& node) override;

			ExpressionCategory m_expressionCategory;
	};

	inline ExpressionCategory GetExpressionCategory(Expression& expression);
}

#include <Nazara/Shader/Ast/AstUtils.inl>

#endif
