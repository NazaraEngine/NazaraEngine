// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionStore.hpp>
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

	void SpirvExpressionStore::Store(const ShaderNodes::ExpressionPtr& node, UInt32 resultId)
	{
		Visit(node);
		
		std::visit(overloaded
		{
			[&](const Pointer& pointer)
			{
				m_writer.GetInstructions().Append(SpirvOp::OpStore, pointer.resultId, resultId);
			},
			[&](const LocalVar& value)
			{
				m_writer.WriteLocalVariable(value.varName, resultId);
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionStore::Visit(ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);

		std::visit(overloaded
		{
			[&](const Pointer& pointer) -> UInt32
			{
				UInt32 resultId = m_writer.AllocateResultId();
				UInt32 pointerType = m_writer.RegisterPointerType(node.exprType, pointer.storage); //< FIXME

				m_writer.GetInstructions().AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
				{
					appender(pointerType);
					appender(resultId);
					appender(pointer.resultId);

					for (std::size_t index : node.memberIndices)
						appender(m_writer.GetConstantId(Int32(index)));
				});

				m_value = Pointer{ pointer.storage, resultId };

				return resultId;
			},
			[](const LocalVar& value) -> UInt32
			{
				throw std::runtime_error("not yet implemented");
			},
			[](std::monostate) -> UInt32
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionStore::Visit(ShaderNodes::Identifier& node)
	{
		Visit(node.var);
	}

	void SpirvExpressionStore::Visit(ShaderNodes::SwizzleOp& node)
	{
		throw std::runtime_error("not yet implemented");
	}

	void SpirvExpressionStore::Visit(ShaderNodes::BuiltinVariable& var)
	{
		const auto& outputVar = m_writer.GetBuiltinVariable(var.entry);

		m_value = Pointer{ SpirvStorageClass::Output, outputVar.varId };
	}

	void SpirvExpressionStore::Visit(ShaderNodes::LocalVariable& var)
	{
		m_value = LocalVar{ var.name };
	}

	void SpirvExpressionStore::Visit(ShaderNodes::OutputVariable& var)
	{
		const auto& outputVar = m_writer.GetOutputVariable(var.name);

		m_value = Pointer{ SpirvStorageClass::Output, outputVar.varId };
	}
}
