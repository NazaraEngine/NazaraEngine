// Copyright (C) 2019 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/FieldOffsets.hpp>
#include <cassert>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	std::size_t FieldOffsets::AddField(StructFieldType type)
	{
		if (m_previousType != StructFieldType_None)
		{
			std::size_t currentTypeAlignment = GetAlignement(m_layout, type);
			std::size_t previousTypeSize = GetSize(m_previousType);

			m_offset += previousTypeSize;

			// Align offset according to current type alignment
			assert(m_offset > 0);
			m_offset = (m_offset + currentTypeAlignment - 1) / currentTypeAlignment * currentTypeAlignment;
		}

		m_previousType = type;

		return m_offset;
	}
}
