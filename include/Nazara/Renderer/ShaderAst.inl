// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

				case ExpressionType::Mat4x4:
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

				case ExpressionType::Mat4x4:
					return ExpressionType::Float4;

				default:
					return type;
			}
		}

		inline ExpressionStatement::ExpressionStatement(ExpressionPtr expr) :
		expression(std::move(expr))
		{
		}

		inline ConditionalStatement::ConditionalStatement(const String& condition, StatementPtr statementPtr) :
		conditionName(condition),
		statement(std::move(statementPtr))
		{
		}

		template<typename... Args>
		StatementBlock::StatementBlock(Args&& ...args) :
		statements({std::forward<Args>(args)...})
		{
		}

		inline Variable::Variable(VariableType varKind, ExpressionType varType) :
		type(varType),
		kind(varKind)
		{
		}

		inline BuiltinVariable::BuiltinVariable(BuiltinEntry variable, ExpressionType varType) :
		Variable(VariableType::Builtin, varType),
		var(variable)
		{
		}

		inline NamedVariable::NamedVariable(VariableType varKind, const Nz::String& varName, ExpressionType varType) :
		Variable(varKind, varType),
		name(varName)
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
			ExpressionType leftType = left->GetExpressionType();
			ExpressionType rightType = right->GetExpressionType();

			if (leftType != rightType)
			{
				switch (op)
				{
					case BinaryType::Add:
					case BinaryType::Equality:
					case BinaryType::Substract:
					{
						//TODO: AstParseError
						throw std::runtime_error("Left expression type must match right expression type");
					}

					case BinaryType::Multiply:
					case BinaryType::Divide:
					{
						switch (leftType)
						{
							case ExpressionType::Float2:
							case ExpressionType::Float3:
							case ExpressionType::Float4:
							{
								if (rightType != ExpressionType::Float1)
									throw std::runtime_error("Left expression type is not compatible with right expression type");

								break;
							}

							case ExpressionType::Mat4x4:
							{
								switch (rightType)
								{
									case ExpressionType::Float1:
									case ExpressionType::Float4:
									case ExpressionType::Mat4x4:
										break;

									//TODO: AstParseError
									default:
										throw std::runtime_error("Left expression type is not compatible with right expression type");
								}

								break;
							}

							default:
								//TODO: AstParseError
								throw std::runtime_error("Left expression type must match right expression type");
						}
					}
				}
			}
		}

		inline Branch::Branch(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement)
		{
			condStatements.emplace_back(ConditionalStatement{ std::move(condition), std::move(trueStatement) });
			elseStatement = std::move(falseStatement);
		}

		inline Cast::Cast(ExpressionType castTo, ExpressionPtr first, ExpressionPtr second, ExpressionPtr third, ExpressionPtr fourth) :
		exprType(castTo),
		expressions({ {first, second, third, fourth} })
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

		inline Constant::Constant(bool value) :
		exprType(ExpressionType::Boolean)
		{
			values.bool1 = value;
		}

		inline Constant::Constant(float value) :
		exprType(ExpressionType::Float1)
		{
			values.vec1 = value;
		}

		inline Constant::Constant(const Vector2f& value) :
		exprType(ExpressionType::Float2)
		{
			values.vec2 = value;
		}

		inline Constant::Constant(const Vector3f& value) :
		exprType(ExpressionType::Float3)
		{
			values.vec3 = value;
		}

		inline Constant::Constant(const Vector4f& value) :
		exprType(ExpressionType::Float4)
		{
			values.vec4 = value;
		}

		inline SwizzleOp::SwizzleOp(ExpressionPtr expressionPtr, std::initializer_list<SwizzleComponent> swizzleComponents) :
		componentCount(swizzleComponents.size()),
		expression(expressionPtr)
		{
			if (componentCount > 4)
				throw std::runtime_error("Cannot swizzle more than four elements");

			switch (expressionPtr->GetExpressionType())
			{
				case ExpressionType::Float1:
				case ExpressionType::Float2:
				case ExpressionType::Float3:
				case ExpressionType::Float4:
					break;

				default:
					throw std::runtime_error("Cannot swizzle this type");
			}

			std::copy(swizzleComponents.begin(), swizzleComponents.end(), components.begin());
		}
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
