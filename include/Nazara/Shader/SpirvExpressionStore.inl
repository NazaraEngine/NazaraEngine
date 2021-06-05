// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvExpressionStore::SpirvExpressionStore(SpirvWriter& writer, SpirvAstVisitor& visitor, SpirvBlock& block) :
	m_block(block),
	m_writer(writer),
	m_visitor(visitor)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
