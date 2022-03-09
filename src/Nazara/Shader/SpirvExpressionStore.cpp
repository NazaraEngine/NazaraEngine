// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <numeric>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
		template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
	}

	void SpirvExpressionStore::Store(ShaderAst::ExpressionPtr& node, UInt32 resultId)
	{
		node->Visit(*this);
		
		std::visit(overloaded
		{
			[&](const Pointer& pointer)
			{
				m_block.Append(SpirvOp::OpStore, pointer.pointerId, resultId);
			},
			[&](const SwizzledPointer& swizzledPointer)
			{
				if (swizzledPointer.componentCount > 1)
				{
					std::size_t vectorSize = swizzledPointer.swizzledType.componentCount;

					UInt32 exprTypeId = m_writer.GetTypeId(swizzledPointer.swizzledType);

					// Load original value (which will then be shuffled with new value)
					UInt32 originalVecId = m_visitor.AllocateResultId();
					m_block.Append(SpirvOp::OpLoad, exprTypeId, originalVecId, swizzledPointer.pointerId);

					// Build a new composite type using OpVectorShuffle and store it
					StackArray<UInt32> indices = NazaraStackArrayNoInit(UInt32, vectorSize);
					std::iota(indices.begin(), indices.end(), UInt32(0u)); //< init with regular swizzle (0,1,2,3)

					// override with swizzle components
					for (std::size_t i = 0; i < swizzledPointer.componentCount; ++i)
						indices[swizzledPointer.swizzleIndices[i]] = SafeCast<UInt32>(vectorSize + i);

					UInt32 shuffleResultId = m_visitor.AllocateResultId();
					m_block.AppendVariadic(SpirvOp::OpVectorShuffle, [&](const auto& appender)
					{
						appender(exprTypeId);
						appender(shuffleResultId);

						appender(originalVecId);
						appender(resultId);

						for (UInt32 index : indices)
							appender(index);
					});

					// Store result
					m_block.Append(SpirvOp::OpStore, swizzledPointer.pointerId, shuffleResultId);
				}
				else
				{
					const ShaderAst::ExpressionType& exprType = GetExpressionType(*node);

					assert(swizzledPointer.componentCount == 1);

					UInt32 pointerType = m_writer.RegisterPointerType(exprType, swizzledPointer.storage); //< FIXME

					// Access chain
					UInt32 indexId = m_writer.GetConstantId(SafeCast<Int32>(swizzledPointer.swizzleIndices[0]));

					UInt32 pointerId = m_visitor.AllocateResultId();
					m_block.Append(SpirvOp::OpAccessChain, pointerType, pointerId, swizzledPointer.pointerId, indexId);
					m_block.Append(SpirvOp::OpStore, pointerId, resultId);
				}
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionStore::Visit(ShaderAst::AccessIndexExpression& node)
	{
		node.expr->Visit(*this);

		const ShaderAst::ExpressionType& exprType = GetExpressionType(node);

		std::visit(overloaded
		{
			[&](const Pointer& pointer)
			{
				UInt32 resultId = m_visitor.AllocateResultId();
				UInt32 pointerType = m_writer.RegisterPointerType(exprType, pointer.storage); //< FIXME

				assert(node.indices.size() == 1);
				UInt32 indexId = m_visitor.EvaluateExpression(node.indices.front());

				m_block.Append(SpirvOp::OpAccessChain, pointerType, resultId, pointer.pointerId, indexId); 

				m_value = Pointer { pointer.storage, resultId };
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionStore::Visit(ShaderAst::SwizzleExpression& node)
	{
		node.expression->Visit(*this);

		std::visit(overloaded
		{
			[&](const Pointer& pointer)
			{
				const auto& expressionType = GetExpressionType(*node.expression);
				assert(IsVectorType(expressionType));

				SwizzledPointer swizzledPointer;
				swizzledPointer.pointerId = pointer.pointerId;
				swizzledPointer.storage = pointer.storage;
				swizzledPointer.swizzledType = std::get<ShaderAst::VectorType>(expressionType);
				swizzledPointer.componentCount = node.componentCount;
				swizzledPointer.swizzleIndices = node.components;

				m_value = swizzledPointer;
			},
			[&](SwizzledPointer& swizzledPointer)
			{
				// Swizzle the swizzle, keep common components
				std::array<UInt32, 4> newIndices;
				newIndices.fill(0); //< keep compiler happy

				for (std::size_t i = 0; i < node.componentCount; ++i)
				{
					assert(node.components[i] < swizzledPointer.componentCount);
					newIndices[i] = swizzledPointer.swizzleIndices[node.components[i]];
				}

				swizzledPointer.componentCount = node.componentCount;
				swizzledPointer.swizzleIndices = newIndices;
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionStore::Visit(ShaderAst::VariableValueExpression& node)
	{
		const auto& var = m_visitor.GetVariable(node.variableId);
		m_value = Pointer{ var.storage, var.pointerId };
	}
}
