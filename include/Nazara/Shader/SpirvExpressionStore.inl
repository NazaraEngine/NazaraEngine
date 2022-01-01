// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvExpressionStore::SpirvExpressionStore(SpirvWriter& writer, SpirvAstVisitor& visitor, SpirvBlock& block) :
	m_visitor(visitor),
	m_block(block),
	m_writer(writer)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
