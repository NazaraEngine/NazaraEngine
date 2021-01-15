// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstOptimizer.hpp>
#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
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


		template<ShaderNodes::BinaryType Type, typename T1, typename T2>
		struct PropagateConstantType;

		// CompEq
		template<typename T1, typename T2>
		struct CompEqBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs == rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompEq;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::CompEq, T1, T2>
		{
			using Op = typename CompEq<T1, T2>;
		};

		// CompGe
		template<typename T1, typename T2>
		struct CompGeBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs >= rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompGe;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::CompGe, T1, T2>
		{
			using Op = typename CompGe<T1, T2>;
		};

		// CompGt
		template<typename T1, typename T2>
		struct CompGtBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs > rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompGt;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::CompGt, T1, T2>
		{
			using Op = typename CompGt<T1, T2>;
		};

		// CompLe
		template<typename T1, typename T2>
		struct CompLeBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs <= rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompLe;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::CompLe, T1, T2>
		{
			using Op = typename CompLe<T1, T2>;
		};

		// CompLt
		template<typename T1, typename T2>
		struct CompLtBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs < rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompLt;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::CompLt, T1, T2>
		{
			using Op = typename CompLe<T1, T2>;
		};

		// CompNe
		template<typename T1, typename T2>
		struct CompNeBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs != rhs);
			}
		};

		template<typename T1, typename T2>
		struct CompNe;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::CompNe, T1, T2>
		{
			using Op = typename CompNe<T1, T2>;
		};

		// Addition
		template<typename T1, typename T2>
		struct AdditionBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs + rhs);
			}
		};

		template<typename T1, typename T2>
		struct Addition;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::Add, T1, T2>
		{
			using Op = typename Addition<T1, T2>;
		};

		// Division
		template<typename T1, typename T2>
		struct DivisionBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs / rhs);
			}
		};

		template<typename T1, typename T2>
		struct Division;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::Divide, T1, T2>
		{
			using Op = typename Division<T1, T2>;
		};

		// Multiplication
		template<typename T1, typename T2>
		struct MultiplicationBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs * rhs);
			}
		};

		template<typename T1, typename T2>
		struct Multiplication;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::Multiply, T1, T2>
		{
			using Op = typename Multiplication<T1, T2>;
		};

		// Subtraction
		template<typename T1, typename T2>
		struct SubtractionBase
		{
			ShaderNodes::ExpressionPtr operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs - rhs);
			}
		};

		template<typename T1, typename T2>
		struct Subtraction;

		template<typename T1, typename T2>
		struct PropagateConstantType<ShaderNodes::BinaryType::Subtract, T1, T2>
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

	ShaderNodes::StatementPtr ShaderAstOptimizer::Optimise(const ShaderNodes::StatementPtr& statement)
	{
		m_shaderAst = nullptr;

		return CloneStatement(statement);
	}

	ShaderNodes::StatementPtr ShaderAstOptimizer::Optimise(const ShaderNodes::StatementPtr& statement, const ShaderAst& shader, UInt64 enabledConditions)
	{
		m_shaderAst = &shader;
		m_enabledConditions = enabledConditions;

		return CloneStatement(statement);
	}

	void ShaderAstOptimizer::Visit(ShaderNodes::BinaryOp& node)
	{
		auto lhs = CloneExpression(node.left);
		auto rhs = CloneExpression(node.right);

		if (lhs->GetType() == ShaderNodes::NodeType::Constant && rhs->GetType() == ShaderNodes::NodeType::Constant)
		{
			auto lhsConstant = std::static_pointer_cast<ShaderNodes::Constant>(lhs);
			auto rhsConstant = std::static_pointer_cast<ShaderNodes::Constant>(rhs);

			switch (node.op)
			{
				case ShaderNodes::BinaryType::Add:
					return PropagateConstant<ShaderNodes::BinaryType::Add>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::Subtract:
					return PropagateConstant<ShaderNodes::BinaryType::Subtract>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::Multiply:
					return PropagateConstant<ShaderNodes::BinaryType::Multiply>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::Divide:
					return PropagateConstant<ShaderNodes::BinaryType::Divide>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::CompEq:
					return PropagateConstant<ShaderNodes::BinaryType::CompEq>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::CompGe:
					return PropagateConstant<ShaderNodes::BinaryType::CompGe>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::CompGt:
					return PropagateConstant<ShaderNodes::BinaryType::CompGt>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::CompLe:
					return PropagateConstant<ShaderNodes::BinaryType::CompLe>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::CompLt:
					return PropagateConstant<ShaderNodes::BinaryType::CompLt>(lhsConstant, rhsConstant);

				case ShaderNodes::BinaryType::CompNe:
					return PropagateConstant<ShaderNodes::BinaryType::CompNe>(lhsConstant, rhsConstant);
			}
		}

		ShaderAstCloner::Visit(node);
	}

	void ShaderAstOptimizer::Visit(ShaderNodes::Branch& node)
	{
		std::vector<ShaderNodes::Branch::ConditionalStatement> statements;
		ShaderNodes::StatementPtr elseStatement;

		for (auto& condStatement : node.condStatements)
		{
			auto cond = CloneExpression(condStatement.condition);

			if (cond->GetType() == ShaderNodes::NodeType::Constant)
			{
				auto constant = std::static_pointer_cast<ShaderNodes::Constant>(cond);

				assert(IsBasicType(cond->GetExpressionType()));
				assert(std::get<ShaderNodes::BasicType>(cond->GetExpressionType()) == ShaderNodes::BasicType::Boolean);

				bool cValue = std::get<bool>(constant->value);
				if (!cValue)
					continue;

				if (statements.empty())
				{
					// First condition is true, dismiss the branch
					Visit(condStatement.statement);
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
				return Visit(node.elseStatement);
			else
				return PushStatement(ShaderNodes::NoOp::Build());
		}

		if (!elseStatement)
			elseStatement = CloneStatement(node.elseStatement);

		PushStatement(ShaderNodes::Branch::Build(std::move(statements), std::move(elseStatement)));
	}

	void ShaderAstOptimizer::Visit(ShaderNodes::ConditionalExpression& node)
	{
		if (!m_shaderAst)
			return ShaderAstCloner::Visit(node);

		std::size_t conditionIndex = m_shaderAst->FindConditionByName(node.conditionName);
		assert(conditionIndex != ShaderAst::InvalidCondition);

		if (TestBit<Nz::UInt64>(m_enabledConditions, conditionIndex))
			Visit(node.truePath);
		else
			Visit(node.falsePath);
	}

	void ShaderAstOptimizer::Visit(ShaderNodes::ConditionalStatement& node)
	{
		if (!m_shaderAst)
			return ShaderAstCloner::Visit(node);

		std::size_t conditionIndex = m_shaderAst->FindConditionByName(node.conditionName);
		assert(conditionIndex != ShaderAst::InvalidCondition);

		if (TestBit<Nz::UInt64>(m_enabledConditions, conditionIndex))
			Visit(node.statement);
	}

	template<ShaderNodes::BinaryType Type>
	void ShaderAstOptimizer::PropagateConstant(const std::shared_ptr<ShaderNodes::Constant>& lhs, const std::shared_ptr<ShaderNodes::Constant>& rhs)
	{
		ShaderNodes::ExpressionPtr optimized;
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
			PushExpression(optimized);
		else
			PushExpression(ShaderNodes::BinaryOp::Build(Type, lhs, rhs));
	}
}
