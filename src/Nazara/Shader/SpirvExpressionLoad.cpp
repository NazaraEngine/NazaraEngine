// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
		template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
	}

	UInt32 SpirvExpressionLoad::Evaluate(ShaderNodes::Expression& node)
	{
		node.Visit(*this);

		return std::visit(overloaded
		{
			[this](const Pointer& pointer) -> UInt32
			{
				UInt32 resultId = m_writer.AllocateResultId();

				m_writer.GetInstructions().Append(SpirvOp::OpLoad, pointer.pointedTypeId, resultId, pointer.resultId);

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

	void SpirvExpressionLoad::Visit(ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);

		std::visit(overloaded
		{
			[&](const Pointer& pointer)
			{
				UInt32 resultId = m_writer.AllocateResultId();
				UInt32 pointerType = m_writer.RegisterPointerType(node.exprType, pointer.storage); //< FIXME
				UInt32 typeId = m_writer.GetTypeId(node.exprType);

				m_writer.GetInstructions().AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
				{
					appender(pointerType);
					appender(resultId);
					appender(pointer.resultId);

					for (std::size_t index : node.memberIndices)
						appender(m_writer.GetConstantId(Int32(index)));
				});

				m_value = Pointer { pointer.storage, resultId, typeId };
			},
			[&](const Value& value)
			{
				UInt32 resultId = m_writer.AllocateResultId();
				UInt32 typeId = m_writer.GetTypeId(node.exprType);

				m_writer.GetInstructions().AppendVariadic(SpirvOp::OpCompositeExtract, [&](const auto& appender)
				{
					appender(typeId);
					appender(resultId);
					appender(value.resultId);

					for (std::size_t index : node.memberIndices)
						appender(m_writer.GetConstantId(Int32(index)));
				});

				m_value = Value { resultId };
			},
			[this](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionLoad::Visit(ShaderNodes::Identifier& node)
	{
		Visit(node.var);
	}

	void SpirvExpressionLoad::Visit(ShaderNodes::InputVariable& var)
	{
		auto inputVar = m_writer.GetInputVariable(var.name);

		if (auto resultIdOpt = m_writer.ReadVariable(inputVar, SpirvWriter::OnlyCache{}))
			m_value = Value{ *resultIdOpt };
		else
			m_value = Pointer{ SpirvStorageClass::Input, inputVar.varId, inputVar.typeId };
	}

	void SpirvExpressionLoad::Visit(ShaderNodes::LocalVariable& var)
	{
		m_value = Value{ m_writer.ReadLocalVariable(var.name) };
	}

	void SpirvExpressionLoad::Visit(ShaderNodes::UniformVariable& var)
	{
		auto uniformVar = m_writer.GetUniformVariable(var.name);

		if (auto resultIdOpt = m_writer.ReadVariable(uniformVar, SpirvWriter::OnlyCache{}))
			m_value = Value{ *resultIdOpt };
		else
			m_value = Pointer{ SpirvStorageClass::Uniform, uniformVar.varId, uniformVar.typeId };
	}
}
