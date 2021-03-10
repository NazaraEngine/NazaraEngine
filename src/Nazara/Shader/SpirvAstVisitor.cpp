// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	UInt32 SpirvAstVisitor::EvaluateExpression(ShaderAst::ExpressionPtr& expr)
	{
		expr->Visit(*this);

		assert(m_resultIds.size() == 1);
		return PopResultId();
	}

	void SpirvAstVisitor::Visit(ShaderAst::AccessMemberExpression& node)
	{
		SpirvExpressionLoad accessMemberVisitor(m_writer, *m_currentBlock);
		PushResultId(accessMemberVisitor.Evaluate(node));
	}

	void SpirvAstVisitor::Visit(ShaderAst::AssignExpression& node)
	{
		UInt32 resultId = EvaluateExpression(node.right);

		SpirvExpressionStore storeVisitor(m_writer, *m_currentBlock);
		storeVisitor.Store(node.left, resultId);

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::BinaryExpression& node)
	{
		ShaderAst::ShaderExpressionType resultExprType = ShaderAst::GetExpressionType(node);
		assert(IsBasicType(resultExprType));

		ShaderAst::ShaderExpressionType leftExprType = ShaderAst::GetExpressionType(*node.left);
		assert(IsBasicType(leftExprType));

		ShaderAst::ShaderExpressionType rightExprType = ShaderAst::GetExpressionType(*node.right);
		assert(IsBasicType(rightExprType));

		ShaderAst::BasicType resultType = std::get<ShaderAst::BasicType>(resultExprType);
		ShaderAst::BasicType leftType = std::get<ShaderAst::BasicType>(leftExprType);
		ShaderAst::BasicType rightType = std::get<ShaderAst::BasicType>(rightExprType);


		UInt32 leftOperand = EvaluateExpression(node.left);
		UInt32 rightOperand = EvaluateExpression(node.right);
		UInt32 resultId = m_writer.AllocateResultId();

		bool swapOperands = false;

		SpirvOp op = [&]
		{
			switch (node.op)
			{
				case ShaderAst::BinaryType::Add:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFAdd;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpIAdd;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::Subtract:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFSub;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpISub;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::Divide:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFDiv;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
							return SpirvOp::OpSDiv;

						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpUDiv;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::CompEq:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Boolean:
							return SpirvOp::OpLogicalEqual;

						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFOrdEqual;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpIEqual;

						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompGe:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFOrdGreaterThan;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
							return SpirvOp::OpSGreaterThan;

						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpUGreaterThan;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompGt:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFOrdGreaterThanEqual;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
							return SpirvOp::OpSGreaterThanEqual;

						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpUGreaterThanEqual;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompLe:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFOrdLessThanEqual;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
							return SpirvOp::OpSLessThanEqual;

						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpULessThanEqual;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompLt:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFOrdLessThan;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
							return SpirvOp::OpSLessThan;

						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpULessThan;

						case ShaderAst::BasicType::Boolean:
						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompNe:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Boolean:
							return SpirvOp::OpLogicalNotEqual;

						case ShaderAst::BasicType::Float1:
						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						case ShaderAst::BasicType::Mat4x4:
							return SpirvOp::OpFOrdNotEqual;

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpINotEqual;

						case ShaderAst::BasicType::Sampler2D:
						case ShaderAst::BasicType::Void:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::Multiply:
				{
					switch (leftType)
					{
						case ShaderAst::BasicType::Float1:
						{
							switch (rightType)
							{
								case ShaderAst::BasicType::Float1:
									return SpirvOp::OpFMul;

								case ShaderAst::BasicType::Float2:
								case ShaderAst::BasicType::Float3:
								case ShaderAst::BasicType::Float4:
									swapOperands = true;
									return SpirvOp::OpVectorTimesScalar;

								case ShaderAst::BasicType::Mat4x4:
									swapOperands = true;
									return SpirvOp::OpMatrixTimesScalar;

								default:
									break;
							}

							break;
						}

						case ShaderAst::BasicType::Float2:
						case ShaderAst::BasicType::Float3:
						case ShaderAst::BasicType::Float4:
						{
							switch (rightType)
							{
								case ShaderAst::BasicType::Float1:
									return SpirvOp::OpVectorTimesScalar;

								case ShaderAst::BasicType::Float2:
								case ShaderAst::BasicType::Float3:
								case ShaderAst::BasicType::Float4:
									return SpirvOp::OpFMul;

								case ShaderAst::BasicType::Mat4x4:
									return SpirvOp::OpVectorTimesMatrix;

								default:
									break;
							}

							break;
						}

						case ShaderAst::BasicType::Int1:
						case ShaderAst::BasicType::Int2:
						case ShaderAst::BasicType::Int3:
						case ShaderAst::BasicType::Int4:
						case ShaderAst::BasicType::UInt1:
						case ShaderAst::BasicType::UInt2:
						case ShaderAst::BasicType::UInt3:
						case ShaderAst::BasicType::UInt4:
							return SpirvOp::OpIMul;

						case ShaderAst::BasicType::Mat4x4:
						{
							switch (rightType)
							{
								case ShaderAst::BasicType::Float1: return SpirvOp::OpMatrixTimesScalar;
								case ShaderAst::BasicType::Float4: return SpirvOp::OpMatrixTimesVector;
								case ShaderAst::BasicType::Mat4x4: return SpirvOp::OpMatrixTimesMatrix;

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

		m_currentBlock->Append(op, m_writer.GetTypeId(resultType), resultId, leftOperand, rightOperand);
		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::BranchStatement& node)
	{
		assert(!node.condStatements.empty());
		auto& firstCond = node.condStatements.front();

		UInt32 previousConditionId = EvaluateExpression(firstCond.condition);
		SpirvBlock previousContentBlock(m_writer);
		m_currentBlock = &previousContentBlock;

		firstCond.statement->Visit(*this);

		SpirvBlock mergeBlock(m_writer);
		m_blocks.back().Append(SpirvOp::OpSelectionMerge, mergeBlock.GetLabelId(), SpirvSelectionControl::None);

		std::optional<std::size_t> nextBlock;
		for (std::size_t statementIndex = 1; statementIndex < node.condStatements.size(); ++statementIndex)
		{
			auto& statement = node.condStatements[statementIndex];

			SpirvBlock contentBlock(m_writer);

			m_blocks.back().Append(SpirvOp::OpBranchConditional, previousConditionId, previousContentBlock.GetLabelId(), contentBlock.GetLabelId());

			previousConditionId = EvaluateExpression(statement.condition);
			m_blocks.emplace_back(std::move(previousContentBlock));
			previousContentBlock = std::move(contentBlock);

			m_currentBlock = &previousContentBlock;

			statement.statement->Visit(*this);
		}

		if (node.elseStatement)
		{
			SpirvBlock elseBlock(m_writer);

			m_currentBlock = &elseBlock;
			node.elseStatement->Visit(*this);

			elseBlock.Append(SpirvOp::OpBranch, mergeBlock.GetLabelId()); //< FIXME: Shouldn't terminate twice

			m_blocks.back().Append(SpirvOp::OpBranchConditional, previousConditionId, previousContentBlock.GetLabelId(), elseBlock.GetLabelId());
			m_blocks.emplace_back(std::move(previousContentBlock));
			m_blocks.emplace_back(std::move(elseBlock));
		}
		else
		{
			m_blocks.back().Append(SpirvOp::OpBranchConditional, previousConditionId, previousContentBlock.GetLabelId(), mergeBlock.GetLabelId());
			m_blocks.emplace_back(std::move(previousContentBlock));
		}

		m_blocks.emplace_back(std::move(mergeBlock));

		m_currentBlock = &m_blocks.back();
	}

	void SpirvAstVisitor::Visit(ShaderAst::CastExpression& node)
	{
		const ShaderAst::ShaderExpressionType& targetExprType = node.targetType;
		assert(IsBasicType(targetExprType));

		ShaderAst::BasicType targetType = std::get<ShaderAst::BasicType>(targetExprType);

		StackVector<UInt32> exprResults = NazaraStackVector(UInt32, node.expressions.size());

		for (auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			exprResults.push_back(EvaluateExpression(exprPtr));
		}

		UInt32 resultId = m_writer.AllocateResultId();

		m_currentBlock->AppendVariadic(SpirvOp::OpCompositeConstruct, [&](const auto& appender)
		{
			appender(m_writer.GetTypeId(targetType));
			appender(resultId);

			for (UInt32 exprResultId : exprResults)
				appender(exprResultId);
		});

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::ConditionalExpression& node)
	{
		if (m_writer.IsConditionEnabled(node.conditionName))
			node.truePath->Visit(*this);
		else
			node.falsePath->Visit(*this);
	}

	void SpirvAstVisitor::Visit(ShaderAst::ConditionalStatement& node)
	{
		if (m_writer.IsConditionEnabled(node.conditionName))
			node.statement->Visit(*this);
	}

	void SpirvAstVisitor::Visit(ShaderAst::ConstantExpression& node)
	{
		std::visit([&] (const auto& value)
		{
			PushResultId(m_writer.GetConstantId(value));
		}, node.value);
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareVariableStatement& node)
	{
		if (node.initialExpression)
			m_writer.WriteLocalVariable(node.varName, EvaluateExpression(node.initialExpression));
	}

	void SpirvAstVisitor::Visit(ShaderAst::DiscardStatement& /*node*/)
	{
		m_currentBlock->Append(SpirvOp::OpKill);
	}

	void SpirvAstVisitor::Visit(ShaderAst::ExpressionStatement& node)
	{
		node.expression->Visit(*this);

		PopResultId();
	}

	void SpirvAstVisitor::Visit(ShaderAst::IdentifierExpression& node)
	{
		SpirvExpressionLoad loadVisitor(m_writer, *m_currentBlock);
		PushResultId(loadVisitor.Evaluate(node));
	}

	void SpirvAstVisitor::Visit(ShaderAst::IntrinsicExpression& node)
	{
		switch (node.intrinsic)
		{
			case ShaderAst::IntrinsicType::DotProduct:
			{
				const ShaderAst::ShaderExpressionType& vecExprType = GetExpressionType(*node.parameters[0]);
				assert(IsBasicType(vecExprType));

				ShaderAst::BasicType vecType = std::get<ShaderAst::BasicType>(vecExprType);

				UInt32 typeId = m_writer.GetTypeId(ShaderAst::GetComponentType(vecType));

				UInt32 vec1 = EvaluateExpression(node.parameters[0]);
				UInt32 vec2 = EvaluateExpression(node.parameters[1]);

				UInt32 resultId = m_writer.AllocateResultId();

				m_currentBlock->Append(SpirvOp::OpDot, typeId, resultId, vec1, vec2);
				PushResultId(resultId);
				break;
			}

			case ShaderAst::IntrinsicType::CrossProduct:
			default:
				throw std::runtime_error("not yet implemented");
		}
	}

	void SpirvAstVisitor::Visit(ShaderAst::NoOpStatement& /*node*/)
	{
		// nothing to do
	}

	void SpirvAstVisitor::Visit(ShaderAst::ReturnStatement& node)
	{
		if (node.returnExpr)
			m_currentBlock->Append(SpirvOp::OpReturnValue, EvaluateExpression(node.returnExpr));
		else
			m_currentBlock->Append(SpirvOp::OpReturn);
	}

	void SpirvAstVisitor::Visit(ShaderAst::MultiStatement& node)
	{
		for (auto& statement : node.statements)
			statement->Visit(*this);
	}

	void SpirvAstVisitor::Visit(ShaderAst::SwizzleExpression& node)
	{
		const ShaderAst::ShaderExpressionType& targetExprType = ShaderAst::GetExpressionType(node);
		assert(IsBasicType(targetExprType));

		ShaderAst::BasicType targetType = std::get<ShaderAst::BasicType>(targetExprType);

		UInt32 exprResultId = EvaluateExpression(node.expression);
		UInt32 resultId = m_writer.AllocateResultId();

		if (node.componentCount > 1)
		{
			// Swizzling is implemented via SpirvOp::OpVectorShuffle using the same vector twice as operands
			m_currentBlock->AppendVariadic(SpirvOp::OpVectorShuffle, [&](const auto& appender)
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

			m_currentBlock->Append(SpirvOp::OpCompositeExtract, m_writer.GetTypeId(targetType), resultId, exprResultId, UInt32(node.components[0]) - UInt32(ShaderAst::SwizzleComponent::First) );
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
