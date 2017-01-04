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


	void Variable::Register(ShaderWriter& visitor)
	{
		visitor.RegisterVariable(kind, name, type);
	}

	void Variable::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
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


	void Constant::Register(ShaderWriter&)
	{
	}

	void Constant::Visit(ShaderWriter& visitor)
	{
		visitor.Write(*this);
	}
}
}
