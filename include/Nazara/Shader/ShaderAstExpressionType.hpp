// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTEXPRESSIONTYPE_HPP
#define NAZARA_SHADERASTEXPRESSIONTYPE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitor.hpp>
#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	struct AstCache;

	class NAZARA_SHADER_API ExpressionTypeVisitor : public AstExpressionVisitor
	{
		public:
			ExpressionTypeVisitor() = default;
			ExpressionTypeVisitor(const ExpressionTypeVisitor&) = delete;
			ExpressionTypeVisitor(ExpressionTypeVisitor&&) = delete;
			~ExpressionTypeVisitor() = default;

			ExpressionType GetExpressionType(Expression& expression, AstCache* cache);

			ExpressionTypeVisitor& operator=(const ExpressionTypeVisitor&) = delete;
			ExpressionTypeVisitor& operator=(ExpressionTypeVisitor&&) = delete;

		private:
			ExpressionType GetExpressionTypeInternal(Expression& expression);
			ExpressionType ResolveAlias(Expression& expression, ExpressionType expressionType);

			void Visit(Expression& expression);

			void Visit(AccessMemberExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SwizzleExpression& node) override;

			AstCache* m_cache;
			std::optional<ExpressionType> m_lastExpressionType;
	};

	inline ExpressionType GetExpressionType(Expression& expression, AstCache* cache = nullptr);
}

#include <Nazara/Shader/ShaderAstExpressionType.inl>

#endif
