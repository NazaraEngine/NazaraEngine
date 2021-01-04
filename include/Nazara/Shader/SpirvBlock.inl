// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvBlock.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvBlock::SpirvBlock(SpirvWriter& writer)
	{
		m_labelId = writer.AllocateResultId();
		Append(SpirvOp::OpLabel, m_labelId);
	}

	inline UInt32 SpirvBlock::GetLabelId() const
	{
		return m_labelId;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
