// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
		template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
	}

	UInt32 SpirvExpressionLoad::Evaluate(ShaderAst::Expression& node)
	{
		node.Visit(*this);

		return std::visit(overloaded
		{
			[this](const Pointer& pointer) -> UInt32
			{
				UInt32 resultId = m_visitor.AllocateResultId();
				m_block.Append(SpirvOp::OpLoad, pointer.pointedTypeId, resultId, pointer.pointerId);

				return resultId;
			},
			[](const Value& value) -> UInt32
			{
				return value.resultId;
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

		const ShaderAst::ExpressionType& exprType = GetExpressionType(node);

		UInt32 resultId = m_visitor.AllocateResultId();
		UInt32 typeId = m_writer.GetTypeId(exprType);

		std::visit(overloaded
		{
			[&](const Pointer& pointer)
			{
				UInt32 pointerType = m_writer.RegisterPointerType(exprType, pointer.storage); //< FIXME

				m_block.AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
				{
					appender(pointerType);
					appender(resultId);
					appender(pointer.pointerId);

					for (std::size_t index : node.memberIndices)
						appender(m_writer.GetConstantId(Int32(index)));
				});

				m_value = Pointer { pointer.storage, resultId, typeId };
			},
			[&](const Value& value)
			{
				m_block.AppendVariadic(SpirvOp::OpCompositeExtract, [&](const auto& appender)
				{
					appender(typeId);
					appender(resultId);
					appender(value.resultId);

					for (std::size_t index : node.memberIndices)
						appender(m_writer.GetConstantId(Int32(index)));
				});

				m_value = Value { resultId };
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionLoad::Visit(ShaderAst::VariableExpression& node)
	{
		const auto& var = m_visitor.GetVariable(node.variableId);
		m_value = Pointer{ var.storage, var.pointerId, var.pointedTypeId };
	}
}
