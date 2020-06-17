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

	ExpressionType Identifier::GetExpressionType() const
	{
		assert(var);
		return var->type;
	}

	void Identifier::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionType AssignOp::GetExpressionType() const
	{
		return left->GetExpressionType();
	}

	void AssignOp::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionType BinaryOp::GetExpressionType() const
	{
		ShaderNodes::ExpressionType exprType = ShaderNodes::ExpressionType::Void;

		switch (op)
		{
			case ShaderNodes::BinaryType::Add:
			case ShaderNodes::BinaryType::Substract:
				exprType = left->GetExpressionType();
				break;

			case ShaderNodes::BinaryType::Divide:
			case ShaderNodes::BinaryType::Multiply:
				//FIXME
				exprType = static_cast<ExpressionType>(std::max(UnderlyingCast(left->GetExpressionType()), UnderlyingCast(right->GetExpressionType())));
				break;

			case ShaderNodes::BinaryType::Equality:
				exprType = ExpressionType::Boolean;
				break;
		}

		NazaraAssert(exprType != ShaderNodes::ExpressionType::Void, "Unhandled builtin");

		return exprType;
	}

	void BinaryOp::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	void Branch::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionType Constant::GetExpressionType() const
	{
		return exprType;
	}

	void Constant::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}

	ExpressionType Cast::GetExpressionType() const
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

	ExpressionType SwizzleOp::GetExpressionType() const
	{
		return static_cast<ExpressionType>(UnderlyingCast(GetComponentType(expression->GetExpressionType())) + componentCount - 1);
	}

	void SwizzleOp::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionType Sample2D::GetExpressionType() const
	{
		return ExpressionType::Float4;
	}

	void Sample2D::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	ExpressionType IntrinsicCall::GetExpressionType() const
	{
		switch (intrinsic)
		{
			case IntrinsicType::CrossProduct:
				return parameters.front()->GetExpressionType();

			case IntrinsicType::DotProduct:
				return ExpressionType::Float1;
		}

		NazaraAssert(false, "Unhandled builtin");
		return ExpressionType::Void;
	}

	void IntrinsicCall::Visit(ShaderVisitor& visitor)
	{
		visitor.Visit(*this);
	}
}
