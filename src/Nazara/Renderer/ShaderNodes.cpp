// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderNodes.hpp>
#include <Nazara/Renderer/ShaderSerializer.hpp>
#include <Nazara/Renderer/ShaderVisitor.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz::ShaderNodes
{
	Node::~Node() = default;

	ExpressionCategory Expression::GetExpressionCategory() const
	{
		return ExpressionCategory::RValue;
	}

	void ExpressionStatement::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void ConditionalStatement::Visit(ShaderVisitor& visitor)
	{
		if (visitor.IsConditionEnabled(conditionName))
			statement->Visit(visitor);
	}


	void StatementBlock::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void DeclareVariable::Visit(ShaderVisitor& visitor)
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

	void Identifier::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ExpressionCategory ShaderNodes::AccessMember::GetExpressionCategory() const
	{
		return ExpressionCategory::LValue;
	}

	ShaderExpressionType AccessMember::GetExpressionType() const
	{
		return exprType;
	}

	void AccessMember::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ShaderExpressionType AssignOp::GetExpressionType() const
	{
		return left->GetExpressionType();
	}

	void AssignOp::Visit(ShaderVisitor& visitor)
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
						exprType = leftExprType;
						break;

					case BasicType::Float1:
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

	void BinaryOp::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void Branch::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ShaderExpressionType Constant::GetExpressionType() const
	{
		return exprType;
	}

	void Constant::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ShaderExpressionType Cast::GetExpressionType() const
	{
		return exprType;
	}

	void Cast::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionCategory SwizzleOp::GetExpressionCategory() const
	{
		return ExpressionCategory::LValue;
	}

	ShaderExpressionType SwizzleOp::GetExpressionType() const
	{
		const ShaderExpressionType& exprType = expression->GetExpressionType();
		assert(std::holds_alternative<BasicType>(exprType));

		return static_cast<BasicType>(UnderlyingCast(GetComponentType(std::get<BasicType>(exprType))) + componentCount - 1);
	}

	void SwizzleOp::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ShaderExpressionType Sample2D::GetExpressionType() const
	{
		return BasicType::Float4;
	}

	void Sample2D::Visit(ShaderVisitor& visitor)
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

	void IntrinsicCall::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}
}
