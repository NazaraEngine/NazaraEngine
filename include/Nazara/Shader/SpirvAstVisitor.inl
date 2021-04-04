// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvAstVisitor::SpirvAstVisitor(SpirvWriter& writer, std::vector<SpirvBlock>& blocks) :
	m_blocks(blocks),
	m_writer(writer)
	{
		m_currentBlock = &m_blocks.back();
	}

	inline const ShaderAst::ExpressionType& SpirvAstVisitor::GetExpressionType(ShaderAst::Expression& expr) const
	{
		assert(expr.cachedExpressionType);
		return expr.cachedExpressionType.value();
	}
}

#include <Nazara/Shader/DebugOff.hpp>
