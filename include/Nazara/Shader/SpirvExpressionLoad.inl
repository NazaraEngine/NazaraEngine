// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvExpressionLoad::SpirvExpressionLoad(SpirvWriter& writer, SpirvAstVisitor& visitor, SpirvBlock& block) :
	m_visitor(visitor),
	m_block(block),
	m_writer(writer)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
