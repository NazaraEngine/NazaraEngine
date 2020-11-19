// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline bool SpirvWriter::IsConditionEnabled(const std::string& condition) const
	{
		return m_context.states->enabledConditions.find(condition) != m_context.states->enabledConditions.end();
	}
}

#include <Nazara/Shader/DebugOff.hpp>
