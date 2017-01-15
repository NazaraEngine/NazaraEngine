// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz { namespace ShaderAst
{
	void ExpressionStatement::Register(ShaderWriter& visitor)
	{
		expression->Register(visitor);
	}

	void ExpressionStatement::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
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


	void BuiltinVariable::Register(ShaderWriter& visitor)
	{
	}

	void BuiltinVariable::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}


	ExpressionType AssignOp::GetExpressionType() const
	{
		return variable->GetExpressionType();
	}

	void AssignOp::Register(ShaderWriter& visitor)
	{
		variable->Register(visitor);
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


	ExpressionType ShaderAst::SwizzleOp::GetExpressionType() const
	{
		return GetComponentType(expression->GetExpressionType());
	}

	void SwizzleOp::Register(ShaderWriter& visitor)
	{
		expression->Register(visitor);
	}

	void SwizzleOp::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}
}
}
