// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderNodes.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Shader/ShaderAstSerializer.hpp>
#include <Nazara/Shader/ShaderAstVisitor.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderNodes
{
	Node::~Node() = default;

	ExpressionCategory Expression::GetExpressionCategory() const
	{
		return ExpressionCategory::RValue;
	}

	void ExpressionStatement::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void ConditionalStatement::Visit(ShaderAstVisitor& visitor)
	{
		if (visitor.IsConditionEnabled(conditionName))
			statement->Visit(visitor);
	}


	void StatementBlock::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void DeclareVariable::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionCategory Identifier::GetExpressionCategory() const
	{
		return ExpressionCategory::LValue;
	}

	ShaderExpressionType Identifier::GetExpressionType() const
	{
		assert(var);
		return var->type;
	}

	void Identifier::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ExpressionCategory AccessMember::GetExpressionCategory() const
	{
		return structExpr->GetExpressionCategory();
	}

	ShaderExpressionType AccessMember::GetExpressionType() const
	{
		return exprType;
	}

	void AccessMember::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ShaderExpressionType AssignOp::GetExpressionType() const
	{
		return left->GetExpressionType();
	}

	void AssignOp::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ShaderExpressionType BinaryOp::GetExpressionType() const
	{
		std::optional<ShaderExpressionType> exprType;

		switch (op)
		{
			case BinaryType::Add:
			case BinaryType::Substract:
				exprType = left->GetExpressionType();
				break;

			case BinaryType::Divide:
			case BinaryType::Multiply:
			{
				const ShaderExpressionType& leftExprType = left->GetExpressionType();
				assert(std::holds_alternative<BasicType>(leftExprType));

				const ShaderExpressionType& rightExprType = right->GetExpressionType();
				assert(std::holds_alternative<BasicType>(rightExprType));

				switch (std::get<BasicType>(leftExprType))
				{
					case BasicType::Boolean:
					case BasicType::Float2:
					case BasicType::Float3:
					case BasicType::Float4:
					case BasicType::Int2:
					case BasicType::Int3:
					case BasicType::Int4:
						exprType = leftExprType;
						break;

					case BasicType::Float1:
					case BasicType::Int1:
					case BasicType::Mat4x4:
						exprType = rightExprType;
						break;

					case BasicType::Sampler2D:
					case BasicType::Void:
						break;
				}

				break;
			}

			case BinaryType::Equality:
				exprType = BasicType::Boolean;
				break;
		}

		NazaraAssert(exprType.has_value(), "Unhandled builtin");

		return *exprType;
	}

	void BinaryOp::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void Branch::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ShaderExpressionType Constant::GetExpressionType() const
	{
		return std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
				return ShaderNodes::BasicType::Boolean;
			else if constexpr (std::is_same_v<T, float>)
				return ShaderNodes::BasicType::Float1;
			else if constexpr (std::is_same_v<T, Int32>)
				return ShaderNodes::BasicType::Int1;
			else if constexpr (std::is_same_v<T, Vector2f>)
				return ShaderNodes::BasicType::Float2;
			else if constexpr (std::is_same_v<T, Vector3f>)
				return ShaderNodes::BasicType::Float3;
			else if constexpr (std::is_same_v<T, Vector4f>)
				return ShaderNodes::BasicType::Float4;
			else if constexpr (std::is_same_v<T, Vector2i32>)
				return ShaderNodes::BasicType::Int2;
			else if constexpr (std::is_same_v<T, Vector3i32>)
				return ShaderNodes::BasicType::Int3;
			else if constexpr (std::is_same_v<T, Vector4i32>)
				return ShaderNodes::BasicType::Int4;
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, value);
	}

	void Constant::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ShaderExpressionType Cast::GetExpressionType() const
	{
		return exprType;
	}

	void Cast::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionCategory SwizzleOp::GetExpressionCategory() const
	{
		return expression->GetExpressionCategory();
	}

	ShaderExpressionType SwizzleOp::GetExpressionType() const
	{
		const ShaderExpressionType& exprType = expression->GetExpressionType();
		assert(std::holds_alternative<BasicType>(exprType));

		return static_cast<BasicType>(UnderlyingCast(GetComponentType(std::get<BasicType>(exprType))) + componentCount - 1);
	}

	void SwizzleOp::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ShaderExpressionType Sample2D::GetExpressionType() const
	{
		return BasicType::Float4;
	}

	void Sample2D::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ShaderExpressionType IntrinsicCall::GetExpressionType() const
	{
		switch (intrinsic)
		{
			case IntrinsicType::CrossProduct:
				return parameters.front()->GetExpressionType();

			case IntrinsicType::DotProduct:
				return BasicType::Float1;
		}

		NazaraAssert(false, "Unhandled builtin");
		return BasicType::Void;
	}

	void IntrinsicCall::Visit(ShaderAstVisitor& visitor)
	{
		visitor.Visit(*this);
	}
}
