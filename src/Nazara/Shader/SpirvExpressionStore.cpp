// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/SpirvAstVisitor.hpp>
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
				m_block.Append(SpirvOp::OpStore, pointer.pointerId, resultId);
			},
			[&](const LocalVar& value)
			{
				throw std::runtime_error("not yet implemented");
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

				StackArray<UInt32> indexIds = NazaraStackArrayNoInit(UInt32, node.indices.size());
				for (std::size_t i = 0; i < node.indices.size(); ++i)
					indexIds[i] = m_visitor.EvaluateExpression(node.indices[i]);

				m_block.AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
				{
					appender(pointerType);
					appender(resultId);
					appender(pointer.pointerId);

					for (UInt32 id : indexIds)
						appender(id);
				});

				m_value = Pointer { pointer.storage, resultId };
			},
			[&](const LocalVar& value)
			{
				throw std::runtime_error("not yet implemented");
			},
			[](std::monostate)
			{
				throw std::runtime_error("an internal error occurred");
			}
		}, m_value);
	}

	void SpirvExpressionStore::Visit(ShaderAst::SwizzleExpression& node)
	{
		throw std::runtime_error("not yet implemented");
	}

	void SpirvExpressionStore::Visit(ShaderAst::VariableExpression& node)
	{
		const auto& var = m_visitor.GetVariable(node.variableId);
		m_value = Pointer{ var.storage, var.pointerId };
	}
}
