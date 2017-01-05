// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace ShaderAst
	{
		inline unsigned int Node::GetComponentCount(ExpressionType type)
		{
			switch (type)
			{
				case ExpressionType::Float2:
					return 2;

				case ExpressionType::Float3:
					return 3;

				case ExpressionType::Float4:
					return 4;

				default:
					return 1;
			}
		}

		inline ExpressionType Node::GetComponentType(ExpressionType type)
		{
			switch (type)
			{
				case ExpressionType::Float2:
				case ExpressionType::Float3:
				case ExpressionType::Float4:
					return ExpressionType::Float1;

				default:
					return type;
			}
		}

		inline ExpressionStatement::ExpressionStatement(ExpressionPtr expr) :
		expression(std::move(expr))
		{
		}

		template<typename... Args>
		StatementBlock::StatementBlock(Args&& ...args) :
		statements({std::forward<Args>(args)...})
		{
		}

		inline Variable::Variable(VariableType varKind, ExpressionType varType) :
		kind(varKind),
		type(varType)
		{
		}

		inline NamedVariable::NamedVariable(VariableType varKind, const Nz::String& varName, ExpressionType varType) :
		Variable(varKind, varType),
		name(varName)
		{
		}

		inline BuiltinVariable::BuiltinVariable(Builtin variable, ExpressionType varType) :
		Variable(VariableType::Builtin, varType),
		var(variable)
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
			//TODO: AstParseError
			if (left->GetExpressionType() != right->GetExpressionType())
				throw std::runtime_error("Left expression type must match right expression type");
		}

		inline Branch::Branch(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement)
		{
			condStatements.emplace_back(ConditionalStatement{ std::move(condition), std::move(trueStatement) });
			elseStatement = std::move(falseStatement);
		}

		inline Cast::Cast(ExpressionType castTo, ExpressionPtr first, ExpressionPtr second, ExpressionPtr third, ExpressionPtr fourth) :
		exprType(castTo),
		expressions({first, second, third, fourth})
		{
			unsigned int componentCount = 0;
			unsigned int requiredComponents = GetComponentCount(exprType);
			for (const auto& exprPtr : expressions)
			{
				if (!exprPtr)
					break;

				componentCount += GetComponentCount(exprPtr->GetExpressionType());
			}

			//TODO: AstParseError
			if (componentCount != requiredComponents)
				throw std::runtime_error("Component count doesn't match required component count");
		}

		inline Constant::Constant(float value) :
		exprType(ExpressionType::Float1)
		{
			values.vec1 = value;
		}
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
