// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline bool SpirvWriter::IsConditionEnabled(const std::string& condition) const
	{
		std::size_t conditionIndex = m_context.shader->FindConditionByName(condition);
		assert(conditionIndex != ShaderAst::InvalidCondition);

		return TestBit<Nz::UInt64>(m_context.states->enabledConditions, conditionIndex);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
