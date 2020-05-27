// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionCategory Expression::GetExpressionCategory() const
	{
		return ExpressionCategory::RValue;
	}

	void ExpressionStatement::Register(ShaderWriter& visitor)
	{
		expression->Register(visitor);
	}

	void ExpressionStatement::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	void ConditionalStatement::Register(ShaderWriter& visitor)
	{
		if (visitor.IsConditionEnabled(conditionName))
			statement->Register(visitor);
	}

	void ConditionalStatement::Visit(ShaderWriter& visitor)
	{
		if (visitor.IsConditionEnabled(conditionName))
			statement->Visit(visitor);
	}


	void StatementBlock::Register(ShaderWriter& visitor)
	{
		for (auto& statementPtr : statements)
			statementPtr->Register(visitor);
	}

	void StatementBlock::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}

	ExpressionCategory Variable::GetExpressionCategory() const
	{
		return ExpressionCategory::LValue;
	}

	ExpressionType Variable::GetExpressionType() const
	{
		return type;
	}

	void NamedVariable::Register(ShaderWriter& visitor)
	{
		visitor.RegisterVariable(kind, name, type);
	}

	void NamedVariable::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	void DeclareVariable::Register(ShaderWriter& visitor)
	{
		variable->Register(visitor);

		if (expression)
			expression->Register(visitor);
	}

	void DeclareVariable::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}

	void BuiltinVariable::Register(ShaderWriter& /*visitor*/)
	{
	}

	void BuiltinVariable::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	ExpressionType AssignOp::GetExpressionType() const
	{
		return left->GetExpressionType();
	}

	void AssignOp::Register(ShaderWriter& visitor)
	{
		left->Register(visitor);
		right->Register(visitor);
	}

	void AssignOp::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	ExpressionType BinaryOp::GetExpressionType() const
	{
		ShaderAst::ExpressionType exprType = ShaderAst::ExpressionType::Void;

		switch (op)
		{
			case ShaderAst::BinaryType::Add:
			case ShaderAst::BinaryType::Divide:
			case ShaderAst::BinaryType::Multiply:
			case ShaderAst::BinaryType::Substract:
				exprType = left->GetExpressionType();
				break;

			case ShaderAst::BinaryType::Equality:
				exprType = ExpressionType::Boolean;
				break;
		}

		NazaraAssert(exprType != ShaderAst::ExpressionType::Void, "Unhandled builtin");

		return exprType;
	}

	void BinaryOp::Register(ShaderWriter& visitor)
	{
		left->Register(visitor);
		right->Register(visitor);
	}

	void BinaryOp::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	void Branch::Register(ShaderWriter& visitor)
	{
		for (ConditionalStatement& statement : condStatements)
		{
			statement.condition->Register(visitor);
			statement.statement->Register(visitor);
		}

		if (elseStatement)
			elseStatement->Register(visitor);
	}

	void Branch::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	ExpressionType Constant::GetExpressionType() const
	{
		return exprType;
	}

	void Constant::Register(ShaderWriter&)
	{
	}

	void Constant::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}

	ExpressionType Cast::GetExpressionType() const
	{
		return exprType;
	}

	void Cast::Register(ShaderWriter& visitor)
	{
		auto it = expressions.begin();
		(*it)->Register(visitor);

		for (; it != expressions.end(); ++it)
		{
			if (!*it)
				break;

			(*it)->Register(visitor);
		}
	}

	void Cast::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	ExpressionCategory SwizzleOp::GetExpressionCategory() const
	{
		return ExpressionCategory::LValue;
	}

	ExpressionType SwizzleOp::GetExpressionType() const
	{
		return static_cast<ExpressionType>(UnderlyingCast(GetComponentType(expression->GetExpressionType())) + componentCount - 1);
	}

	void SwizzleOp::Register(ShaderWriter& visitor)
	{
		expression->Register(visitor);
	}

	void SwizzleOp::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	ExpressionType Sample2D::GetExpressionType() const
	{
		return ExpressionType::Float4;
	}

	void Sample2D::Register(ShaderWriter& visitor)
	{
		sampler->Register(visitor);
		coordinates->Register(visitor);
	}

	void Sample2D::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}
}
