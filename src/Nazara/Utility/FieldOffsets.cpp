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
		std::size_t fieldAlignement = GetAlignement(m_layout, type);
		if (m_layout == StructLayout_Std140)
			fieldAlignement = Align(fieldAlignement, GetAlignement(StructLayout_Std140, StructFieldType_Float4));

		m_largestFieldAlignment = std::max(fieldAlignement, m_largestFieldAlignment);

		std::size_t offset = Align(m_size, Align(fieldAlignement, m_offsetRounding));
		m_size = offset + GetSize(type) * arraySize;

		m_offsetRounding = 1;

		return offset;
	}

	std::size_t FieldOffsets::AddArrayStruct(const FieldOffsets& fieldStruct, std::size_t arraySize)
	{
		assert(arraySize > 0);

		std::size_t fieldAlignement = fieldStruct.GetLargestFieldAlignement();
		if (m_layout == StructLayout_Std140)
			fieldAlignement = Align(fieldAlignement, GetAlignement(StructLayout_Std140, StructFieldType_Float4));

		m_largestFieldAlignment = std::max(m_largestFieldAlignment, fieldAlignement);

		std::size_t offset = Align(m_size, Align(fieldAlignement, m_offsetRounding));
		m_size = offset
		       + fieldStruct.GetSize() * arraySize
		       + (Align(fieldStruct.GetSize(), fieldAlignement) - fieldStruct.GetSize()) * (arraySize - 1);

		m_offsetRounding = fieldAlignement;

		return offset;
	}

	std::size_t FieldOffsets::AddField(StructFieldType type)
	{
		std::size_t fieldAlignement = GetAlignement(m_layout, type);

		m_largestFieldAlignment = std::max(m_largestFieldAlignment, fieldAlignement);

		std::size_t offset = Align(m_size, Align(fieldAlignement, m_offsetRounding));
		m_size = offset + GetSize(type);

		m_offsetRounding = 1;

		return offset;
	}

	std::size_t FieldOffsets::AddStruct(const FieldOffsets& fieldStruct)
	{
		std::size_t fieldAlignement = fieldStruct.GetLargestFieldAlignement();
		if (m_layout == StructLayout_Std140)
			fieldAlignement = Align(fieldAlignement, GetAlignement(StructLayout_Std140, StructFieldType_Float4));

		m_largestFieldAlignment = std::max(m_largestFieldAlignment, fieldAlignement);

		std::size_t offset = Align(m_size, Align(fieldAlignement, m_offsetRounding));
		m_size = offset + fieldStruct.GetSize();

		m_offsetRounding = std::max<std::size_t>(Align(fieldStruct.GetSize(), fieldAlignement) - fieldStruct.GetSize(), 1);

		return offset;
	}
}
