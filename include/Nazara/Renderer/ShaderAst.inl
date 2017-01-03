// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace ShaderAst
	{
		inline ExpressionStatement::ExpressionStatement(ExpressionPtr expr) :
		expression(std::move(expr))
		{
		}

		template<typename... Args>
		StatementBlock::StatementBlock(Args&& ...args) :
		statements({std::forward<Args>(args)...})
		{
		}

		inline Variable::Variable(VariableType varKind, const Nz::String& varName, ExpressionType varType) :
		kind(varKind),
		name(varName),
		type(varType)
		{
		}

		inline AssignOp::AssignOp(AssignType Op, VariablePtr Var, ExpressionPtr Right) :
		op(Op),
		variable(std::move(Var)),
		right(std::move(Right))
		{
		}

		inline BinaryOp::BinaryOp(BinaryType Op, ExpressionPtr Left, ExpressionPtr Right) :
		op(Op),
		left(std::move(Left)),
		right(std::move(Right))
		{
		}

		inline Branch::Branch(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement)
		{
			condStatements.emplace_back(ConditionalStatement{ std::move(condition), std::move(trueStatement) });
			elseStatement = std::move(falseStatement);
		}

		inline Constant::Constant(float value) :
		exprType(ExpressionType::Float1)
		{
			values.vec1 = value;
		}
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
