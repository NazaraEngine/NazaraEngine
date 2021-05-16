// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstOptimizer.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		template<typename T, typename U>
		std::unique_ptr<T> static_unique_pointer_cast(std::unique_ptr<U>&& ptr)
		{
			return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
		}

		template <typename T>
		struct is_complete_helper
		{
			template <typename U> static auto test(U*)->std::integral_constant<bool, sizeof(U) == sizeof(U)>;
			static auto test(...) -> std::false_type;

			using type = decltype(test((T*)0));
		};

		template <typename T>
		struct is_complete : is_complete_helper<T>::type {};

		template<typename T>
		inline constexpr bool is_complete_v = is_complete<T>::value;

		/*************************************************************************************************/

		template<BinaryType Type, typename T1, typename T2>
		struct BinaryConstantPropagation;

		// CompEq
		template<typename T1, typename T2>
		struct BinaryCompEqBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs == rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompEq;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompEq, T1, T2>
		{
			using Op = BinaryCompEq<T1, T2>;
		};

		// CompGe
		template<typename T1, typename T2>
		struct BinaryCompGeBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs >= rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompGe;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompGe, T1, T2>
		{
			using Op = BinaryCompGe<T1, T2>;
		};

		// CompGt
		template<typename T1, typename T2>
		struct BinaryCompGtBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs > rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompGt;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompGt, T1, T2>
		{
			using Op = BinaryCompGt<T1, T2>;
		};

		// CompLe
		template<typename T1, typename T2>
		struct BinaryCompLeBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs <= rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompLe;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompLe, T1, T2>
		{
			using Op = BinaryCompLe<T1, T2>;
		};

		// CompLt
		template<typename T1, typename T2>
		struct BinaryCompLtBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs < rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompLt;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompLt, T1, T2>
		{
			using Op = BinaryCompLe<T1, T2>;
		};

		// CompNe
		template<typename T1, typename T2>
		struct BinaryCompNeBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs != rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompNe;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompNe, T1, T2>
		{
			using Op = BinaryCompNe<T1, T2>;
		};

		// Addition
		template<typename T1, typename T2>
		struct BinaryAdditionBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs + rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryAddition;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Add, T1, T2>
		{
			using Op = BinaryAddition<T1, T2>;
		};

		// Division
		template<typename T1, typename T2>
		struct BinaryDivisionBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs / rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryDivision;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Divide, T1, T2>
		{
			using Op = BinaryDivision<T1, T2>;
		};

		// Multiplication
		template<typename T1, typename T2>
		struct BinaryMultiplicationBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs * rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryMultiplication;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Multiply, T1, T2>
		{
			using Op = BinaryMultiplication<T1, T2>;
		};

		// Subtraction
		template<typename T1, typename T2>
		struct BinarySubtractionBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs - rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinarySubtraction;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Subtract, T1, T2>
		{
			using Op = BinarySubtraction<T1, T2>;
		};
		
		/*************************************************************************************************/

		template<UnaryType Type, typename T>
		struct UnaryConstantPropagation;

		// LogicalNot
		template<typename T>
		struct UnaryLogicalNotBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T& arg)
			{
				return ShaderBuilder::Constant(!arg);
			}
		};

		template<typename T>
		struct UnaryLogicalNot;

		template<typename T>
		struct UnaryConstantPropagation<UnaryType::LogicalNot, T>
		{
			using Op = UnaryLogicalNot<T>;
		};

		// Minus
		template<typename T>
		struct UnaryMinusBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T& arg)
			{
				return ShaderBuilder::Constant(-arg);
			}
		};

		template<typename T>
		struct UnaryMinus;

		template<typename T>
		struct UnaryConstantPropagation<UnaryType::Minus, T>
		{
			using Op = UnaryMinus<T>;
		};

		// Plus
		template<typename T>
		struct UnaryPlusBase
		{
			std::unique_ptr<ConstantExpression> operator()(const T& arg)
			{
				return ShaderBuilder::Constant(arg);
			}
		};

		template<typename T>
		struct UnaryPlus;

		template<typename T>
		struct UnaryConstantPropagation<UnaryType::Plus, T>
		{
			using Op = UnaryPlus<T>;
		};

#define EnableOptimisation(Op, ...) template<> struct Op<__VA_ARGS__> : Op##Base<__VA_ARGS__> {}

		// Binary

		EnableOptimisation(BinaryCompEq, bool, bool);
		EnableOptimisation(BinaryCompEq, double, double);
		EnableOptimisation(BinaryCompEq, float, float);
		EnableOptimisation(BinaryCompEq, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryCompEq, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryCompEq, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryCompEq, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryCompEq, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryCompEq, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryCompEq, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryCompGe, bool, bool);
		EnableOptimisation(BinaryCompGe, double, double);
		EnableOptimisation(BinaryCompGe, float, float);
		EnableOptimisation(BinaryCompGe, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryCompGe, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryCompGe, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryCompGe, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryCompGe, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryCompGe, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryCompGe, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryCompGt, bool, bool);
		EnableOptimisation(BinaryCompGt, double, double);
		EnableOptimisation(BinaryCompGt, float, float);
		EnableOptimisation(BinaryCompGt, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryCompGt, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryCompGt, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryCompGt, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryCompGt, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryCompGt, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryCompGt, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryCompLe, bool, bool);
		EnableOptimisation(BinaryCompLe, double, double);
		EnableOptimisation(BinaryCompLe, float, float);
		EnableOptimisation(BinaryCompLe, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryCompLe, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryCompLe, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryCompLe, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryCompLe, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryCompLe, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryCompLe, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryCompLt, bool, bool);
		EnableOptimisation(BinaryCompLt, double, double);
		EnableOptimisation(BinaryCompLt, float, float);
		EnableOptimisation(BinaryCompLt, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryCompLt, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryCompLt, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryCompLt, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryCompLt, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryCompLt, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryCompLt, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryCompNe, bool, bool);
		EnableOptimisation(BinaryCompNe, double, double);
		EnableOptimisation(BinaryCompNe, float, float);
		EnableOptimisation(BinaryCompNe, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryCompNe, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryCompNe, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryCompNe, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryCompNe, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryCompNe, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryCompNe, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryAddition, double, double);
		EnableOptimisation(BinaryAddition, float, float);
		EnableOptimisation(BinaryAddition, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryAddition, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryAddition, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryAddition, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryAddition, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryAddition, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryAddition, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryDivision, double, double);
		EnableOptimisation(BinaryDivision, double, Nz::Vector2d);
		EnableOptimisation(BinaryDivision, double, Nz::Vector3d);
		EnableOptimisation(BinaryDivision, double, Nz::Vector4d);
		EnableOptimisation(BinaryDivision, float, float);
		EnableOptimisation(BinaryDivision, float, Nz::Vector2f);
		EnableOptimisation(BinaryDivision, float, Nz::Vector3f);
		EnableOptimisation(BinaryDivision, float, Nz::Vector4f);
		EnableOptimisation(BinaryDivision, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryDivision, Nz::Int32, Nz::Vector2i32);
		EnableOptimisation(BinaryDivision, Nz::Int32, Nz::Vector3i32);
		EnableOptimisation(BinaryDivision, Nz::Int32, Nz::Vector4i32);
		EnableOptimisation(BinaryDivision, Nz::Vector2f, float);
		EnableOptimisation(BinaryDivision, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryDivision, Nz::Vector3f, float);
		EnableOptimisation(BinaryDivision, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryDivision, Nz::Vector4f, float);
		EnableOptimisation(BinaryDivision, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryDivision, Nz::Vector2d, double);
		EnableOptimisation(BinaryDivision, Nz::Vector2d, Nz::Vector2d);
		EnableOptimisation(BinaryDivision, Nz::Vector3d, double);
		EnableOptimisation(BinaryDivision, Nz::Vector3d, Nz::Vector3d);
		EnableOptimisation(BinaryDivision, Nz::Vector4d, double);
		EnableOptimisation(BinaryDivision, Nz::Vector4d, Nz::Vector4d);
		EnableOptimisation(BinaryDivision, Nz::Vector2i32, Nz::Int32);
		EnableOptimisation(BinaryDivision, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryDivision, Nz::Vector3i32, Nz::Int32);
		EnableOptimisation(BinaryDivision, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryDivision, Nz::Vector4i32, Nz::Int32);
		EnableOptimisation(BinaryDivision, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinaryMultiplication, double, double);
		EnableOptimisation(BinaryMultiplication, double, Nz::Vector2d);
		EnableOptimisation(BinaryMultiplication, double, Nz::Vector3d);
		EnableOptimisation(BinaryMultiplication, double, Nz::Vector4d);
		EnableOptimisation(BinaryMultiplication, float, float);
		EnableOptimisation(BinaryMultiplication, float, Nz::Vector2f);
		EnableOptimisation(BinaryMultiplication, float, Nz::Vector3f);
		EnableOptimisation(BinaryMultiplication, float, Nz::Vector4f);
		EnableOptimisation(BinaryMultiplication, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinaryMultiplication, Nz::Int32, Nz::Vector2i32);
		EnableOptimisation(BinaryMultiplication, Nz::Int32, Nz::Vector3i32);
		EnableOptimisation(BinaryMultiplication, Nz::Int32, Nz::Vector4i32);
		EnableOptimisation(BinaryMultiplication, Nz::Vector2f, float);
		EnableOptimisation(BinaryMultiplication, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinaryMultiplication, Nz::Vector3f, float);
		EnableOptimisation(BinaryMultiplication, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinaryMultiplication, Nz::Vector4f, float);
		EnableOptimisation(BinaryMultiplication, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinaryMultiplication, Nz::Vector2d, double);
		EnableOptimisation(BinaryMultiplication, Nz::Vector2d, Nz::Vector2d);
		EnableOptimisation(BinaryMultiplication, Nz::Vector3d, double);
		EnableOptimisation(BinaryMultiplication, Nz::Vector3d, Nz::Vector3d);
		EnableOptimisation(BinaryMultiplication, Nz::Vector4d, double);
		EnableOptimisation(BinaryMultiplication, Nz::Vector4d, Nz::Vector4d);
		EnableOptimisation(BinaryMultiplication, Nz::Vector2i32, Nz::Int32);
		EnableOptimisation(BinaryMultiplication, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinaryMultiplication, Nz::Vector3i32, Nz::Int32);
		EnableOptimisation(BinaryMultiplication, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinaryMultiplication, Nz::Vector4i32, Nz::Int32);
		EnableOptimisation(BinaryMultiplication, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(BinarySubtraction, double, double);
		EnableOptimisation(BinarySubtraction, float, float);
		EnableOptimisation(BinarySubtraction, Nz::Int32, Nz::Int32);
		EnableOptimisation(BinarySubtraction, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(BinarySubtraction, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(BinarySubtraction, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(BinarySubtraction, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(BinarySubtraction, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(BinarySubtraction, Nz::Vector4i32, Nz::Vector4i32);

		// Unary

		EnableOptimisation(UnaryLogicalNot, bool);

		EnableOptimisation(UnaryMinus, double);
		EnableOptimisation(UnaryMinus, float);
		EnableOptimisation(UnaryMinus, Nz::Int32);
		EnableOptimisation(UnaryMinus, Nz::Vector2f);
		EnableOptimisation(UnaryMinus, Nz::Vector3f);
		EnableOptimisation(UnaryMinus, Nz::Vector4f);
		EnableOptimisation(UnaryMinus, Nz::Vector2i32);
		EnableOptimisation(UnaryMinus, Nz::Vector3i32);
		EnableOptimisation(UnaryMinus, Nz::Vector4i32);

		EnableOptimisation(UnaryPlus, double);
		EnableOptimisation(UnaryPlus, float);
		EnableOptimisation(UnaryPlus, Nz::Int32);
		EnableOptimisation(UnaryPlus, Nz::Vector2f);
		EnableOptimisation(UnaryPlus, Nz::Vector3f);
		EnableOptimisation(UnaryPlus, Nz::Vector4f);
		EnableOptimisation(UnaryPlus, Nz::Vector2i32);
		EnableOptimisation(UnaryPlus, Nz::Vector3i32);
		EnableOptimisation(UnaryPlus, Nz::Vector4i32);

#undef EnableOptimisation
	}

	StatementPtr AstOptimizer::Optimise(const StatementPtr& statement)
	{
		m_enabledOptions.reset();
		return CloneStatement(statement);
	}

	StatementPtr AstOptimizer::Optimise(const StatementPtr& statement, UInt64 enabledConditions)
	{
		m_enabledOptions = enabledConditions;

		return CloneStatement(statement);
	}

	ExpressionPtr AstOptimizer::Clone(BinaryExpression& node)
	{
		auto lhs = CloneExpression(node.left);
		auto rhs = CloneExpression(node.right);

		if (lhs->GetType() == NodeType::ConstantExpression && rhs->GetType() == NodeType::ConstantExpression)
		{
			auto lhsConstant = static_unique_pointer_cast<ConstantExpression>(std::move(lhs));
			auto rhsConstant = static_unique_pointer_cast<ConstantExpression>(std::move(rhs));

			ExpressionPtr optimized;
			switch (node.op)
			{
				case BinaryType::Add:
					optimized = PropagateConstant<BinaryType::Add>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::Subtract:
					optimized = PropagateConstant<BinaryType::Subtract>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::Multiply:
					optimized = PropagateConstant<BinaryType::Multiply>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::Divide:
					optimized = PropagateConstant<BinaryType::Divide>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::CompEq:
					optimized = PropagateConstant<BinaryType::CompEq>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::CompGe:
					optimized = PropagateConstant<BinaryType::CompGe>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::CompGt:
					optimized = PropagateConstant<BinaryType::CompGt>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::CompLe:
					optimized = PropagateConstant<BinaryType::CompLe>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::CompLt:
					optimized = PropagateConstant<BinaryType::CompLt>(std::move(lhsConstant), std::move(rhsConstant));
					break;

				case BinaryType::CompNe:
					optimized = PropagateConstant<BinaryType::CompNe>(std::move(lhsConstant), std::move(rhsConstant));
					break;
			}

			if (optimized)
				return optimized;
		}

		auto binary = ShaderBuilder::Binary(node.op, std::move(lhs), std::move(rhs));
		binary->cachedExpressionType = node.cachedExpressionType;

		return binary;
	}

	StatementPtr AstOptimizer::Clone(BranchStatement& node)
	{
		std::vector<BranchStatement::ConditionalStatement> statements;
		StatementPtr elseStatement;

		for (auto& condStatement : node.condStatements)
		{
			auto cond = CloneExpression(condStatement.condition);

			if (cond->GetType() == NodeType::ConstantExpression)
			{
				auto& constant = static_cast<ConstantExpression&>(*cond);

				assert(constant.cachedExpressionType);
				const ExpressionType& constantType = constant.cachedExpressionType.value();

				assert(IsPrimitiveType(constantType));
				assert(std::get<PrimitiveType>(constantType) == PrimitiveType::Boolean);

				bool cValue = std::get<bool>(constant.value);
				if (!cValue)
					continue;

				if (statements.empty())
				{
					// First condition is true, dismiss the branch
					return AstCloner::Clone(condStatement.statement);
				}
				else
				{
					// Some condition after the first one is true, make it the else statement and stop there
					elseStatement = CloneStatement(condStatement.statement);
					break;
				}
			}
			else
			{
				auto& c = statements.emplace_back();
				c.condition = std::move(cond);
				c.statement = CloneStatement(condStatement.statement);
			}
		}

		if (statements.empty())
		{
			// All conditions have been removed, replace by else statement or no-op
			if (node.elseStatement)
				return AstCloner::Clone(node.elseStatement);
			else
				return ShaderBuilder::NoOp();
		}

		if (!elseStatement)
			elseStatement = CloneStatement(node.elseStatement);

		return ShaderBuilder::Branch(std::move(statements), std::move(elseStatement));
	}

	ExpressionPtr AstOptimizer::Clone(ConditionalExpression& node)
	{
		if (!m_enabledOptions)
			return AstCloner::Clone(node);

		if (TestBit<UInt64>(*m_enabledOptions, node.optionIndex))
			return AstCloner::Clone(node.truePath);
		else
			return AstCloner::Clone(node.falsePath);
	}

	ExpressionPtr AstOptimizer::Clone(UnaryExpression& node)
	{
		auto expr = CloneExpression(node.expression);

		if (expr->GetType() == NodeType::ConstantExpression)
		{
			auto constantExpr = static_unique_pointer_cast<ConstantExpression>(std::move(expr));

			ExpressionPtr optimized;
			switch (node.op)
			{
				case UnaryType::LogicalNot:
					optimized = PropagateConstant<UnaryType::LogicalNot>(std::move(constantExpr));
					break;

				case UnaryType::Minus:
					optimized = PropagateConstant<UnaryType::Minus>(std::move(constantExpr));
					break;

				case UnaryType::Plus:
					optimized = PropagateConstant<UnaryType::Plus>(std::move(constantExpr));
					break;
			}

			if (optimized)
				return optimized;
		}

		auto unary = ShaderBuilder::Unary(node.op, std::move(expr));
		unary->cachedExpressionType = node.cachedExpressionType;

		return unary;
	}

	StatementPtr AstOptimizer::Clone(ConditionalStatement& node)
	{
		if (!m_enabledOptions)
			return AstCloner::Clone(node);

		if (TestBit<UInt64>(*m_enabledOptions, node.optionIndex))
			return AstCloner::Clone(node);
		else
			return ShaderBuilder::NoOp();
	}

	template<BinaryType Type>
	ExpressionPtr AstOptimizer::PropagateConstant(std::unique_ptr<ConstantExpression>&& lhs, std::unique_ptr<ConstantExpression>&& rhs)
	{
		std::unique_ptr<ConstantExpression> optimized;
		std::visit([&](auto&& arg1)
		{
			using T1 = std::decay_t<decltype(arg1)>;

			std::visit([&](auto&& arg2)
			{
				using T2 = std::decay_t<decltype(arg2)>;
				using PCType = BinaryConstantPropagation<Type, T1, T2>;

				if constexpr (is_complete_v<PCType>)
				{
					using Op = typename PCType::Op;
					if constexpr (is_complete_v<Op>)
						optimized = Op{}(arg1, arg2);
				}

			}, rhs->value);
		}, lhs->value);

		if (optimized)
			optimized->cachedExpressionType = optimized->GetExpressionType();

		return optimized;
	}

	template<UnaryType Type>
	ExpressionPtr AstOptimizer::PropagateConstant(std::unique_ptr<ConstantExpression>&& operand)
	{
		std::unique_ptr<ConstantExpression> optimized;
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			using PCType = UnaryConstantPropagation<Type, T>;

			if constexpr (is_complete_v<PCType>)
			{
				using Op = typename PCType::Op;
				if constexpr (is_complete_v<Op>)
					optimized = Op{}(arg);
			}
		}, operand->value);

		if (optimized)
			optimized->cachedExpressionType = optimized->GetExpressionType();

		return optimized;
	}
}
