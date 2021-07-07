// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTOPTIMISER_HPP
#define NAZARA_SHADERASTOPTIMISER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <optional>
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
				std::optional<UInt64> enabledOptions = 0;
			};

		protected:
			ExpressionPtr Clone(BinaryExpression& node) override;
			ExpressionPtr Clone(CastExpression& node) override;
			ExpressionPtr Clone(ConditionalExpression& node) override;
			ExpressionPtr Clone(ConstantIndexExpression& node) override;
			ExpressionPtr Clone(UnaryExpression& node) override;
			StatementPtr Clone(BranchStatement& node) override;
			StatementPtr Clone(ConditionalStatement& node) override;

			template<BinaryType Type> ExpressionPtr PropagateBinaryConstant(std::unique_ptr<ConstantExpression>&& lhs, std::unique_ptr<ConstantExpression>&& rhs);
			template<typename TargetType> ExpressionPtr PropagateSingleValueCast(std::unique_ptr<ConstantExpression>&& operand);
			template<UnaryType Type> ExpressionPtr PropagateUnaryConstant(std::unique_ptr<ConstantExpression>&& operand);
			template<typename TargetType> ExpressionPtr PropagateVec2Cast(TargetType v1, TargetType v2);
			template<typename TargetType> ExpressionPtr PropagateVec3Cast(TargetType v1, TargetType v2, TargetType v3);
			template<typename TargetType> ExpressionPtr PropagateVec4Cast(TargetType v1, TargetType v2, TargetType v3, TargetType v4);

		private:
			Options m_options;
	};

	inline ExpressionPtr Optimize(Expression& expr);
	inline ExpressionPtr Optimize(Expression& expr, const AstOptimizer::Options& options);
	inline StatementPtr Optimize(Statement& ast);
	inline StatementPtr Optimize(Statement& ast, const AstOptimizer::Options& options);
}

#include <Nazara/Shader/Ast/AstOptimizer.inl>

#endif
