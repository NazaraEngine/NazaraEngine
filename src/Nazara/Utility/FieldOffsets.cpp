// Copyright (C) 2019 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/FieldOffsets.hpp>
#include <algorithm>
#include <cassert>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	std::size_t FieldOffsets::AddArrayField(StructFieldType type, std::size_t arraySize)
	{
		std::size_t offset = m_size;
		if (offset > 0)
		{
			std::size_t fieldAlignement = GetAlignement(m_layout, type);
			if (m_layout == StructLayout_Std140)
				fieldAlignement = Align(fieldAlignement, GetAlignement(StructLayout_Std140, StructFieldType_Float4));

			m_largestfieldAlignment = std::max(fieldAlignement, m_largestfieldAlignment);

			offset = Align(offset, GetAlignement(m_layout, type) * arraySize);
		}

		m_size = offset + GetSize(type);

		return offset;
	}

	std::size_t FieldOffsets::AddArrayStruct(const FieldOffsets& fieldStruct, std::size_t arraySize)
	{
		std::size_t offset = m_size;
		if (offset > 0)
		{
			std::size_t fieldAlignement = fieldStruct.GetLargestFieldAlignement();
			if (m_layout == StructLayout_Std140)
				fieldAlignement = Align(fieldAlignement, GetAlignement(StructLayout_Std140, StructFieldType_Float4));

			m_largestfieldAlignment = std::max(m_largestfieldAlignment, fieldAlignement);

			offset = Align(offset, fieldAlignement);
		}

		m_size = offset + fieldStruct.GetSize();

		return offset;
	}

	std::size_t FieldOffsets::AddField(StructFieldType type)
	{
		std::size_t offset = m_size;
		if (offset > 0)
		{
			std::size_t fieldAlignement = GetAlignement(m_layout, type);
			m_largestfieldAlignment = std::max(m_largestfieldAlignment, fieldAlignement);

			offset = Align(offset, fieldAlignement);
		}

		m_size = offset + GetSize(type);

		return offset;
	}

	std::size_t FieldOffsets::AddStruct(const FieldOffsets& fieldStruct)
	{
		std::size_t offset = m_size;
		if (offset > 0)
		{
			std::size_t fieldAlignement = fieldStruct.GetLargestFieldAlignement();
			if (m_layout == StructLayout_Std140)
				fieldAlignement = Align(fieldAlignement, GetAlignement(StructLayout_Std140, StructFieldType_Float4));

			m_largestfieldAlignment = std::max(m_largestfieldAlignment, fieldAlignement);

			offset = Align(offset, fieldAlignement);
		}

		m_size = offset + fieldStruct.GetSize();

		return offset;
	}
}
