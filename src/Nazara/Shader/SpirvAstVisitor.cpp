// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	UInt32 SpirvAstVisitor::EvaluateExpression(const ShaderNodes::ExpressionPtr& expr)
	{
		Visit(expr);

		assert(m_resultIds.size() == 1);
		return PopResultId();
	}

	void SpirvAstVisitor::Visit(ShaderNodes::AccessMember& node)
	{
		SpirvExpressionLoad accessMemberVisitor(m_writer);
		PushResultId(accessMemberVisitor.Evaluate(node));
	}

	void SpirvAstVisitor::Visit(ShaderNodes::AssignOp& node)
	{
		UInt32 resultId = EvaluateExpression(node.right);

		SpirvExpressionStore storeVisitor(m_writer);
		storeVisitor.Store(node.left, resultId);

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::BinaryOp& node)
	{
		ShaderExpressionType resultExprType = node.GetExpressionType();
		assert(IsBasicType(resultExprType));

		const ShaderExpressionType& leftExprType = node.left->GetExpressionType();
		assert(IsBasicType(leftExprType));

		const ShaderExpressionType& rightExprType = node.right->GetExpressionType();
		assert(IsBasicType(rightExprType));

		ShaderNodes::BasicType resultType = std::get<ShaderNodes::BasicType>(resultExprType);
		ShaderNodes::BasicType leftType = std::get<ShaderNodes::BasicType>(leftExprType);
		ShaderNodes::BasicType rightType = std::get<ShaderNodes::BasicType>(rightExprType);


		UInt32 leftOperand = EvaluateExpression(node.left);
		UInt32 rightOperand = EvaluateExpression(node.right);
		UInt32 resultId = m_writer.AllocateResultId();

		bool swapOperands = false;

		SpirvOp op = [&]
		{
			switch (node.op)
			{
				case ShaderNodes::BinaryType::Add:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFAdd;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpIAdd;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderNodes::BinaryType::Substract:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFSub;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpISub;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderNodes::BinaryType::Divide:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFDiv;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpSDiv;

						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpUDiv;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderNodes::BinaryType::CompEq:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Boolean:
							return SpirvOp::OpLogicalEqual;

						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdEqual;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpIEqual;

						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderNodes::BinaryType::CompGe:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdGreaterThan;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpSGreaterThan;

						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpUGreaterThan;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderNodes::BinaryType::CompGt:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdGreaterThanEqual;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpSGreaterThanEqual;

						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpUGreaterThanEqual;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderNodes::BinaryType::CompLe:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdLessThanEqual;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpSLessThanEqual;

						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpULessThanEqual;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderNodes::BinaryType::CompLt:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdLessThan;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpSLessThan;

						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpULessThan;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderNodes::BinaryType::CompNe:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Boolean:
							return SpirvOp::OpLogicalNotEqual;

						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdNotEqual;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpINotEqual;

						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderNodes::BinaryType::Multiply:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						{
							switch (rightType)
							{
								case ShaderNodes::BasicType::Float1:
									return SpirvOp::OpFMul;

								case ShaderNodes::BasicType::Float2:
								case ShaderNodes::BasicType::Float3:
								case ShaderNodes::BasicType::Float4:
									swapOperands = true;
									return SpirvOp::OpVectorTimesScalar;

								case ShaderNodes::BasicType::Mat4x4:
									swapOperands = true;
									return SpirvOp::OpMatrixTimesScalar;

								default:
									break;
							}

							break;
						}

						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						{
							switch (rightType)
							{
								case ShaderNodes::BasicType::Float1:
									return SpirvOp::OpVectorTimesScalar;

								case ShaderNodes::BasicType::Float2:
								case ShaderNodes::BasicType::Float3:
								case ShaderNodes::BasicType::Float4:
									return SpirvOp::OpFMul;

								case ShaderNodes::BasicType::Mat4x4:
									return SpirvOp::OpVectorTimesMatrix;

								default:
									break;
							}

							break;
						}

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpIMul;

						case ShaderNodes::BasicType::Mat4x4:
						{
							switch (rightType)
							{
								case ShaderNodes::BasicType::Float1: return SpirvOp::OpMatrixTimesScalar;
								case ShaderNodes::BasicType::Float4: return SpirvOp::OpMatrixTimesVector;
								case ShaderNodes::BasicType::Mat4x4: return SpirvOp::OpMatrixTimesMatrix;

								default:
									break;
							}

							break;
						}

						default:
							break;
					}

					break;
				}
			}

			assert(false);
			throw std::runtime_error("unexpected binary operation");
		}();

		if (swapOperands)
			std::swap(leftOperand, rightOperand);

		m_writer.GetInstructions().Append(op, m_writer.GetTypeId(resultType), resultId, leftOperand, rightOperand);
		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::Cast& node)
	{
		const ShaderExpressionType& targetExprType = node.exprType;
		assert(IsBasicType(targetExprType));

		ShaderNodes::BasicType targetType = std::get<ShaderNodes::BasicType>(targetExprType);

		StackVector<UInt32> exprResults = NazaraStackVector(UInt32, node.expressions.size());

		for (const auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			exprResults.push_back(EvaluateExpression(exprPtr));
		}

		UInt32 resultId = m_writer.AllocateResultId();

		m_writer.GetInstructions().AppendVariadic(SpirvOp::OpCompositeConstruct, [&](const auto& appender)
		{
			appender(m_writer.GetTypeId(targetType));
			appender(resultId);

			for (UInt32 exprResultId : exprResults)
				appender(exprResultId);
		});

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::ConditionalExpression& node)
	{
		if (m_writer.IsConditionEnabled(node.conditionName))
			Visit(node.truePath);
		else
			Visit(node.falsePath);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::ConditionalStatement& node)
	{
		if (m_writer.IsConditionEnabled(node.conditionName))
			Visit(node.statement);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::Constant& node)
	{
		std::visit([&] (const auto& value)
		{
			PushResultId(m_writer.GetConstantId(value));
		}, node.value);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::DeclareVariable& node)
	{
		if (node.expression)
		{
			assert(node.variable->GetType() == ShaderNodes::VariableType::LocalVariable);

			const auto& localVar = static_cast<const ShaderNodes::LocalVariable&>(*node.variable);
			m_writer.WriteLocalVariable(localVar.name, EvaluateExpression(node.expression));
		}
	}

	void SpirvAstVisitor::Visit(ShaderNodes::Discard& /*node*/)
	{
		m_writer.GetInstructions().Append(SpirvOp::OpKill);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);
		PopResultId();
	}

	void SpirvAstVisitor::Visit(ShaderNodes::Identifier& node)
	{
		SpirvExpressionLoad loadVisitor(m_writer);
		PushResultId(loadVisitor.Evaluate(node));
	}

	void SpirvAstVisitor::Visit(ShaderNodes::IntrinsicCall& node)
	{
		switch (node.intrinsic)
		{
			case ShaderNodes::IntrinsicType::DotProduct:
			{
				const ShaderExpressionType& vecExprType = node.parameters[0]->GetExpressionType();
				assert(IsBasicType(vecExprType));

				ShaderNodes::BasicType vecType = std::get<ShaderNodes::BasicType>(vecExprType);

				UInt32 typeId = m_writer.GetTypeId(node.GetComponentType(vecType));

				UInt32 vec1 = EvaluateExpression(node.parameters[0]);
				UInt32 vec2 = EvaluateExpression(node.parameters[1]);

				UInt32 resultId = m_writer.AllocateResultId();

				m_writer.GetInstructions().Append(SpirvOp::OpDot, typeId, resultId, vec1, vec2);
				PushResultId(resultId);
				break;
			}

			case ShaderNodes::IntrinsicType::CrossProduct:
			default:
				throw std::runtime_error("not yet implemented");
		}
	}

	void SpirvAstVisitor::Visit(ShaderNodes::Sample2D& node)
	{
		UInt32 typeId = m_writer.GetTypeId(ShaderNodes::BasicType::Float4);

		UInt32 samplerId = EvaluateExpression(node.sampler);
		UInt32 coordinatesId = EvaluateExpression(node.coordinates);
		UInt32 resultId = m_writer.AllocateResultId();

		m_writer.GetInstructions().Append(SpirvOp::OpImageSampleImplicitLod, typeId, resultId, samplerId, coordinatesId);
		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);
	}

	void SpirvAstVisitor::Visit(ShaderNodes::SwizzleOp& node)
	{
		const ShaderExpressionType& targetExprType = node.GetExpressionType();
		assert(IsBasicType(targetExprType));

		ShaderNodes::BasicType targetType = std::get<ShaderNodes::BasicType>(targetExprType);

		UInt32 exprResultId = EvaluateExpression(node.expression);
		UInt32 resultId = m_writer.AllocateResultId();

		if (node.componentCount > 1)
		{
			// Swizzling is implemented via SpirvOp::OpVectorShuffle using the same vector twice as operands
			m_writer.GetInstructions().AppendVariadic(SpirvOp::OpVectorShuffle, [&](const auto& appender)
			{
				appender(m_writer.GetTypeId(targetType));
				appender(resultId);
				appender(exprResultId);
				appender(exprResultId);

				for (std::size_t i = 0; i < node.componentCount; ++i)
					appender(UInt32(node.components[0]) - UInt32(node.components[i]));
			});
		}
		else
		{
			// Extract a single component from the vector
			assert(node.componentCount == 1);

			m_writer.GetInstructions().Append(SpirvOp::OpCompositeExtract, m_writer.GetTypeId(targetType), resultId, exprResultId, UInt32(node.components[0]) - UInt32(ShaderNodes::SwizzleComponent::First) );
		}

		PushResultId(resultId);
	}

	void SpirvAstVisitor::PushResultId(UInt32 value)
	{
		m_resultIds.push_back(value);
	}

	UInt32 SpirvAstVisitor::PopResultId()
	{
		if (m_resultIds.empty())
			throw std::runtime_error("invalid operation");

		UInt32 resultId = m_resultIds.back();
		m_resultIds.pop_back();

		return resultId;
	}
}
