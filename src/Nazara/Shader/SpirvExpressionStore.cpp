// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
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
				m_block.Append(SpirvOp::OpStore, pointer.resultId, resultId);
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

	/*void SpirvExpressionStore::Visit(ShaderAst::AccessMemberExpression& node)
	{
		Visit(node.structExpr);

		std::visit(overloaded
		{
			[&](const Pointer& pointer) -> UInt32
			{
				UInt32 resultId = m_writer.AllocateResultId();
				UInt32 pointerType = m_writer.RegisterPointerType(node.exprType, pointer.storage); //< FIXME

				m_block.AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
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
	}*/

	void SpirvExpressionStore::Visit(ShaderAst::IdentifierExpression& node)
	{
		m_value = LocalVar{ node.identifier };
	}

	void SpirvExpressionStore::Visit(ShaderAst::SwizzleExpression& node)
	{
		throw std::runtime_error("not yet implemented");
	}
}
