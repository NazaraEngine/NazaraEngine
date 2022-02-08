// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTOPTIMIZER_HPP
#define NAZARA_SHADER_AST_ASTOPTIMIZER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <optional>
#include <unordered_map>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstOptimizer : public AstCloner
	{
		public:
			struct Options;

			AstOptimizer() = default;
			AstOptimizer(const AstOptimizer&) = delete;
			AstOptimizer(AstOptimizer&&) = delete;
			~AstOptimizer() = default;

			inline ExpressionPtr Optimise(Expression& expression);
			inline ExpressionPtr Optimise(Expression& expression, const Options& options);
			inline StatementPtr Optimise(Statement& statement);
			inline StatementPtr Optimise(Statement& statement, const Options& options);

			AstOptimizer& operator=(const AstOptimizer&) = delete;
			AstOptimizer& operator=(AstOptimizer&&) = delete;

			struct Options
			{
				std::function<const ConstantValue&(std::size_t constantId)> constantQueryCallback;
			};

		protected:
			ExpressionPtr Clone(BinaryExpression& node) override;
			ExpressionPtr Clone(CastExpression& node) override;
			ExpressionPtr Clone(ConditionalExpression& node) override;
			ExpressionPtr Clone(ConstantExpression& node) override;
			ExpressionPtr Clone(SwizzleExpression& node) override;
			ExpressionPtr Clone(UnaryExpression& node) override;
			StatementPtr Clone(BranchStatement& node) override;
			StatementPtr Clone(ConditionalStatement& node) override;

			template<BinaryType Type> ExpressionPtr PropagateBinaryConstant(const ConstantValueExpression& lhs, const ConstantValueExpression& rhs);
			template<typename TargetType> ExpressionPtr PropagateSingleValueCast(const ConstantValueExpression& operand);
			template<std::size_t TargetComponentCount> ExpressionPtr PropagateConstantSwizzle(const std::array<UInt32, 4>& components, const ConstantValueExpression& operand);
			template<UnaryType Type> ExpressionPtr PropagateUnaryConstant(const ConstantValueExpression& operand);
			template<typename TargetType> ExpressionPtr PropagateVec2Cast(TargetType v1, TargetType v2);
			template<typename TargetType> ExpressionPtr PropagateVec3Cast(TargetType v1, TargetType v2, TargetType v3);
			template<typename TargetType> ExpressionPtr PropagateVec4Cast(TargetType v1, TargetType v2, TargetType v3, TargetType v4);

			StatementPtr Unscope(StatementPtr node);

		private:
			Options m_options;
	};

	inline ExpressionPtr Optimize(Expression& expr);
	inline ExpressionPtr Optimize(Expression& expr, const AstOptimizer::Options& options);
	inline StatementPtr Optimize(Statement& ast);
	inline StatementPtr Optimize(Statement& ast, const AstOptimizer::Options& options);
}

#include <Nazara/Shader/Ast/AstOptimizer.inl>

#endif // NAZARA_SHADER_AST_ASTOPTIMIZER_HPP
