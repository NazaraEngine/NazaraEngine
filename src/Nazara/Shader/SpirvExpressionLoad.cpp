// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	UInt32 SpirvExpressionLoad::Evaluate(ShaderAst::Expression& node)
	{
		node.Visit(*this);

		return std::visit(Overloaded
		{
			[this](const Pointer& pointer) -> UInt32
			{
				UInt32 resultId = m_visitor.AllocateResultId();
				m_block.Append(SpirvOp::OpLoad, pointer.pointedTypeId, resultId, pointer.pointerId);

				return resultId;
			},
			[this](const PointerChainAccess& pointerChainAccess) -> UInt32
			{
				UInt32 pointerType = m_writer.RegisterPointerType(*pointerChainAccess.exprType, pointerChainAccess.storage); //< FIXME: We shouldn't register this so late

				UInt32 pointerId = m_visitor.AllocateResultId();
				
				m_block.AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
				{
					appender(pointerType);
					appender(pointerId);
					appender(pointerChainAccess.pointerId);

					for (UInt32 id : pointerChainAccess.indices)
						appender(id);
				});

				UInt32 resultId = m_visitor.AllocateResultId();
				m_block.Append(SpirvOp::OpLoad, m_writer.GetTypeId(*pointerChainAccess.exprType), resultId, pointerId);

				return resultId;
			},
			[](const Value& value) -> UInt32
			{
				return value.valueId;
			},
			[this](const ValueExtraction& extractedValue) -> UInt32
			{
				UInt32 resultId = m_visitor.AllocateResultId();

				m_block.AppendVariadic(SpirvOp::OpCompositeExtract, [&](const auto& appender)
				{
					appender(extractedValue.typeId);
					appender(resultId);
					appender(extractedValue.valueId);

					for (UInt32 id : extractedValue.indices)
						appender(id);
				});

				return resultId;
			},
			[](std::monostate) -> UInt32
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionLoad::Visit(ShaderAst::AccessIndexExpression& node)
	{
		node.expr->Visit(*this);

		const ShaderAst::ExpressionType* exprType = GetExpressionType(node);
		assert(exprType);

		UInt32 typeId = m_writer.GetTypeId(*exprType);

		assert(node.indices.size() == 1);
		UInt32 indexId = m_visitor.EvaluateExpression(node.indices.front());

		std::visit(Overloaded
		{
			[&](const Pointer& pointer)
			{
				PointerChainAccess pointerChainAccess;
				pointerChainAccess.exprType = exprType;
				pointerChainAccess.indices = { indexId };
				pointerChainAccess.pointedTypeId = pointer.pointedTypeId;
				pointerChainAccess.pointerId = pointer.pointerId;
				pointerChainAccess.storage = pointer.storage;

				m_value = std::move(pointerChainAccess);
			},
			[&](PointerChainAccess& pointerChainAccess)
			{
				pointerChainAccess.exprType = exprType;
				pointerChainAccess.indices.push_back(indexId);
			},
			[&](const Value& value)
			{
				ValueExtraction extractedValue;
				extractedValue.indices = { indexId };
				extractedValue.typeId = typeId;
				extractedValue.valueId = value.valueId;

				m_value = std::move(extractedValue);
			},
			[&](ValueExtraction& extractedValue)
			{
				extractedValue.indices.push_back(indexId);
				extractedValue.typeId = typeId;
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionLoad::Visit(ShaderAst::VariableValueExpression& node)
	{
		const auto& var = m_visitor.GetVariable(node.variableId);
		m_value = Pointer{ var.storage, var.pointerId, var.pointedTypeId };
	}
}
