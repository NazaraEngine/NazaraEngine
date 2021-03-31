// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstOptimizer.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderAstExpressionType.hpp>
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


		template<BinaryType Type, typename T1, typename T2>
		struct PropagateConstantType;

		// CompEq
		template<typename T1, typename T2>
		struct CompEqBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs == rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompEq;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::CompEq, T1, T2>
		{
			using Op = typename CompEq<T1, T2>;
		};

		// CompGe
		template<typename T1, typename T2>
		struct CompGeBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs >= rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompGe;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::CompGe, T1, T2>
		{
			using Op = typename CompGe<T1, T2>;
		};

		// CompGt
		template<typename T1, typename T2>
		struct CompGtBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs > rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompGt;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::CompGt, T1, T2>
		{
			using Op = typename CompGt<T1, T2>;
		};

		// CompLe
		template<typename T1, typename T2>
		struct CompLeBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs <= rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompLe;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::CompLe, T1, T2>
		{
			using Op = typename CompLe<T1, T2>;
		};

		// CompLt
		template<typename T1, typename T2>
		struct CompLtBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs < rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompLt;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::CompLt, T1, T2>
		{
			using Op = typename CompLe<T1, T2>;
		};

		// CompNe
		template<typename T1, typename T2>
		struct CompNeBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs != rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompNe;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::CompNe, T1, T2>
		{
			using Op = typename CompNe<T1, T2>;
		};

		// Addition
		template<typename T1, typename T2>
		struct AdditionBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs + rhs);
			}
		};

		template<typename T1, typename T2>
		struct Addition;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::Add, T1, T2>
		{
			using Op = typename Addition<T1, T2>;
		};

		// Division
		template<typename T1, typename T2>
		struct DivisionBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs / rhs);
			}
		};

		template<typename T1, typename T2>
		struct Division;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::Divide, T1, T2>
		{
			using Op = typename Division<T1, T2>;
		};

		// Multiplication
		template<typename T1, typename T2>
		struct MultiplicationBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs * rhs);
			}
		};

		template<typename T1, typename T2>
		struct Multiplication;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::Multiply, T1, T2>
		{
			using Op = typename Multiplication<T1, T2>;
		};

		// Subtraction
		template<typename T1, typename T2>
		struct SubtractionBase
		{
			ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs - rhs);
			}
		};

		template<typename T1, typename T2>
		struct Subtraction;

		template<typename T1, typename T2>
		struct PropagateConstantType<BinaryType::Subtract, T1, T2>
		{
			using Op = typename Subtraction<T1, T2>;
		};

#define EnableOptimisation(Op, T1, T2) template<> struct Op<T1, T2> : Op##Base<T1, T2> {}

		EnableOptimisation(CompEq, bool, bool);
		EnableOptimisation(CompEq, double, double);
		EnableOptimisation(CompEq, float, float);
		EnableOptimisation(CompEq, Nz::Int32, Nz::Int32);
		EnableOptimisation(CompEq, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(CompEq, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(CompEq, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(CompEq, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(CompEq, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(CompEq, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(CompGe, bool, bool);
		EnableOptimisation(CompGe, double, double);
		EnableOptimisation(CompGe, float, float);
		EnableOptimisation(CompGe, Nz::Int32, Nz::Int32);
		EnableOptimisation(CompGe, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(CompGe, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(CompGe, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(CompGe, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(CompGe, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(CompGe, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(CompGt, bool, bool);
		EnableOptimisation(CompGt, double, double);
		EnableOptimisation(CompGt, float, float);
		EnableOptimisation(CompGt, Nz::Int32, Nz::Int32);
		EnableOptimisation(CompGt, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(CompGt, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(CompGt, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(CompGt, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(CompGt, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(CompGt, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(CompLe, bool, bool);
		EnableOptimisation(CompLe, double, double);
		EnableOptimisation(CompLe, float, float);
		EnableOptimisation(CompLe, Nz::Int32, Nz::Int32);
		EnableOptimisation(CompLe, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(CompLe, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(CompLe, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(CompLe, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(CompLe, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(CompLe, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(CompLt, bool, bool);
		EnableOptimisation(CompLt, double, double);
		EnableOptimisation(CompLt, float, float);
		EnableOptimisation(CompLt, Nz::Int32, Nz::Int32);
		EnableOptimisation(CompLt, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(CompLt, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(CompLt, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(CompLt, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(CompLt, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(CompLt, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(CompNe, bool, bool);
		EnableOptimisation(CompNe, double, double);
		EnableOptimisation(CompNe, float, float);
		EnableOptimisation(CompNe, Nz::Int32, Nz::Int32);
		EnableOptimisation(CompNe, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(CompNe, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(CompNe, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(CompNe, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(CompNe, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(CompNe, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(Addition, double, double);
		EnableOptimisation(Addition, float, float);
		EnableOptimisation(Addition, Nz::Int32, Nz::Int32);
		EnableOptimisation(Addition, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(Addition, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(Addition, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(Addition, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(Addition, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(Addition, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(Division, double, double);
		EnableOptimisation(Division, double, Nz::Vector2d);
		EnableOptimisation(Division, double, Nz::Vector3d);
		EnableOptimisation(Division, double, Nz::Vector4d);
		EnableOptimisation(Division, float, float);
		EnableOptimisation(Division, float, Nz::Vector2f);
		EnableOptimisation(Division, float, Nz::Vector3f);
		EnableOptimisation(Division, float, Nz::Vector4f);
		EnableOptimisation(Division, Nz::Int32, Nz::Int32);
		EnableOptimisation(Division, Nz::Int32, Nz::Vector2i32);
		EnableOptimisation(Division, Nz::Int32, Nz::Vector3i32);
		EnableOptimisation(Division, Nz::Int32, Nz::Vector4i32);
		EnableOptimisation(Division, Nz::Vector2f, float);
		EnableOptimisation(Division, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(Division, Nz::Vector3f, float);
		EnableOptimisation(Division, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(Division, Nz::Vector4f, float);
		EnableOptimisation(Division, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(Division, Nz::Vector2d, double);
		EnableOptimisation(Division, Nz::Vector2d, Nz::Vector2d);
		EnableOptimisation(Division, Nz::Vector3d, double);
		EnableOptimisation(Division, Nz::Vector3d, Nz::Vector3d);
		EnableOptimisation(Division, Nz::Vector4d, double);
		EnableOptimisation(Division, Nz::Vector4d, Nz::Vector4d);
		EnableOptimisation(Division, Nz::Vector2i32, Nz::Int32);
		EnableOptimisation(Division, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(Division, Nz::Vector3i32, Nz::Int32);
		EnableOptimisation(Division, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(Division, Nz::Vector4i32, Nz::Int32);
		EnableOptimisation(Division, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(Multiplication, double, double);
		EnableOptimisation(Multiplication, double, Nz::Vector2d);
		EnableOptimisation(Multiplication, double, Nz::Vector3d);
		EnableOptimisation(Multiplication, double, Nz::Vector4d);
		EnableOptimisation(Multiplication, float, float);
		EnableOptimisation(Multiplication, float, Nz::Vector2f);
		EnableOptimisation(Multiplication, float, Nz::Vector3f);
		EnableOptimisation(Multiplication, float, Nz::Vector4f);
		EnableOptimisation(Multiplication, Nz::Int32, Nz::Int32);
		EnableOptimisation(Multiplication, Nz::Int32, Nz::Vector2i32);
		EnableOptimisation(Multiplication, Nz::Int32, Nz::Vector3i32);
		EnableOptimisation(Multiplication, Nz::Int32, Nz::Vector4i32);
		EnableOptimisation(Multiplication, Nz::Vector2f, float);
		EnableOptimisation(Multiplication, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(Multiplication, Nz::Vector3f, float);
		EnableOptimisation(Multiplication, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(Multiplication, Nz::Vector4f, float);
		EnableOptimisation(Multiplication, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(Multiplication, Nz::Vector2d, double);
		EnableOptimisation(Multiplication, Nz::Vector2d, Nz::Vector2d);
		EnableOptimisation(Multiplication, Nz::Vector3d, double);
		EnableOptimisation(Multiplication, Nz::Vector3d, Nz::Vector3d);
		EnableOptimisation(Multiplication, Nz::Vector4d, double);
		EnableOptimisation(Multiplication, Nz::Vector4d, Nz::Vector4d);
		EnableOptimisation(Multiplication, Nz::Vector2i32, Nz::Int32);
		EnableOptimisation(Multiplication, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(Multiplication, Nz::Vector3i32, Nz::Int32);
		EnableOptimisation(Multiplication, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(Multiplication, Nz::Vector4i32, Nz::Int32);
		EnableOptimisation(Multiplication, Nz::Vector4i32, Nz::Vector4i32);

		EnableOptimisation(Subtraction, double, double);
		EnableOptimisation(Subtraction, float, float);
		EnableOptimisation(Subtraction, Nz::Int32, Nz::Int32);
		EnableOptimisation(Subtraction, Nz::Vector2f, Nz::Vector2f);
		EnableOptimisation(Subtraction, Nz::Vector3f, Nz::Vector3f);
		EnableOptimisation(Subtraction, Nz::Vector4f, Nz::Vector4f);
		EnableOptimisation(Subtraction, Nz::Vector2i32, Nz::Vector2i32);
		EnableOptimisation(Subtraction, Nz::Vector3i32, Nz::Vector3i32);
		EnableOptimisation(Subtraction, Nz::Vector4i32, Nz::Vector4i32);

#undef EnableOptimisation
	}

	StatementPtr AstOptimizer::Optimise(StatementPtr& statement)
	{
		return CloneStatement(statement);
	}

	StatementPtr AstOptimizer::Optimise(StatementPtr& statement, UInt64 enabledConditions)
	{
		m_enabledConditions = enabledConditions;

		return CloneStatement(statement);
	}

	void AstOptimizer::Visit(BinaryExpression& node)
	{
		auto lhs = CloneExpression(node.left);
		auto rhs = CloneExpression(node.right);

		if (lhs->GetType() == NodeType::ConstantExpression && rhs->GetType() == NodeType::ConstantExpression)
		{
			auto lhsConstant = static_unique_pointer_cast<ConstantExpression>(std::move(lhs));
			auto rhsConstant = static_unique_pointer_cast<ConstantExpression>(std::move(rhs));

			switch (node.op)
			{
				case BinaryType::Add:
					return PropagateConstant<BinaryType::Add>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::Subtract:
					return PropagateConstant<BinaryType::Subtract>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::Multiply:
					return PropagateConstant<BinaryType::Multiply>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::Divide:
					return PropagateConstant<BinaryType::Divide>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::CompEq:
					return PropagateConstant<BinaryType::CompEq>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::CompGe:
					return PropagateConstant<BinaryType::CompGe>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::CompGt:
					return PropagateConstant<BinaryType::CompGt>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::CompLe:
					return PropagateConstant<BinaryType::CompLe>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::CompLt:
					return PropagateConstant<BinaryType::CompLt>(std::move(lhsConstant), std::move(rhsConstant));

				case BinaryType::CompNe:
					return PropagateConstant<BinaryType::CompNe>(std::move(lhsConstant), std::move(rhsConstant));
			}
		}

		AstCloner::Visit(node);
	}

	void AstOptimizer::Visit(BranchStatement& node)
	{
		std::vector<BranchStatement::ConditionalStatement> statements;
		StatementPtr elseStatement;

		for (auto& condStatement : node.condStatements)
		{
			auto cond = CloneExpression(condStatement.condition);

			if (cond->GetType() == NodeType::ConstantExpression)
			{
				auto& constant = static_cast<ConstantExpression&>(*cond);

				assert(IsPrimitiveType(GetExpressionType(constant)));
				assert(std::get<PrimitiveType>(GetExpressionType(constant)) == PrimitiveType::Boolean);

				bool cValue = std::get<bool>(constant.value);
				if (!cValue)
					continue;

				if (statements.empty())
				{
					// First condition is true, dismiss the branch
					condStatement.statement->Visit(*this);
					return;
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
			{
				node.elseStatement->Visit(*this);
				return;
			}
			else
				return PushStatement(ShaderBuilder::NoOp());
		}

		if (!elseStatement)
			elseStatement = CloneStatement(node.elseStatement);

		PushStatement(ShaderBuilder::Branch(std::move(statements), std::move(elseStatement)));
	}

	void AstOptimizer::Visit(ConditionalExpression& node)
	{
		return AstCloner::Visit(node);

		/*if (!m_shaderAst)
			return ShaderAstCloner::Visit(node);

		std::size_t conditionIndex = m_shaderAst->FindConditionByName(node.conditionName);
		assert(conditionIndex != InvalidCondition);

		if (TestBit<Nz::UInt64>(m_enabledConditions, conditionIndex))
			Visit(node.truePath);
		else
			Visit(node.falsePath);*/
	}

	void AstOptimizer::Visit(ConditionalStatement& node)
	{
		return AstCloner::Visit(node);

		/*if (!m_shaderAst)
			return ShaderAstCloner::Visit(node);

		std::size_t conditionIndex = m_shaderAst->FindConditionByName(node.conditionName);
		assert(conditionIndex != InvalidCondition);

		if (TestBit<Nz::UInt64>(m_enabledConditions, conditionIndex))
			Visit(node.statement);*/
	}

	template<BinaryType Type>
	void AstOptimizer::PropagateConstant(std::unique_ptr<ConstantExpression>&& lhs, std::unique_ptr<ConstantExpression>&& rhs)
	{
		ExpressionPtr optimized;
		std::visit([&](auto&& arg1)
		{
			using T1 = std::decay_t<decltype(arg1)>;

			std::visit([&](auto&& arg2)
			{
				using T2 = std::decay_t<decltype(arg2)>;
				using PCType = PropagateConstantType<Type, T1, T2>;

				if constexpr (is_complete_v<PCType>)
				{
					using Op = typename PCType::Op;
					if constexpr (is_complete_v<Op>)
						optimized = Op{}(arg1, arg2);
				}

			}, rhs->value);
		}, lhs->value);

		if (optimized)
			PushExpression(std::move(optimized));
		else
			PushExpression(ShaderBuilder::Binary(Type, std::move(lhs), std::move(rhs)));
	}
}
