// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/FieldOffsets.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <cassert>
#include <memory>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline FieldOffsets::FieldOffsets(StructLayout layout) :
	m_largestFieldAlignment(1),
	m_offsetRounding(1),
	m_size(0),
	m_layout(layout)
	{
	}

	inline std::size_t FieldOffsets::GetLargestFieldAlignement() const
	{
		return m_largestFieldAlignment;
	}

	inline StructLayout FieldOffsets::GetLayout() const
	{
		return m_layout;
	}

	inline std::size_t FieldOffsets::GetAlignedSize() const
	{
		if (m_layout == StructLayout::Std140)
			return Align(m_size, m_largestFieldAlignment);
		else
			return m_size;
	}

	inline std::size_t FieldOffsets::GetSize() const
	{
		return m_size;
	}

	inline std::size_t FieldOffsets::GetAlignement(StructLayout layout, StructFieldType fieldType)
	{
		switch (layout)
		{
			case StructLayout::Packed:
				return 1;

			case StructLayout::Std140:
			{
				switch (fieldType)
				{
					case StructFieldType::Bool1:
					case StructFieldType::Float1:
					case StructFieldType::Int1:
					case StructFieldType::UInt1:
						return 4;

					case StructFieldType::Bool2:
					case StructFieldType::Float2:
					case StructFieldType::Int2:
					case StructFieldType::UInt2:
						return 2 * 4;

					case StructFieldType::Bool3:
					case StructFieldType::Float3:
					case StructFieldType::Int3:
					case StructFieldType::UInt3:
					case StructFieldType::Bool4:
					case StructFieldType::Float4:
					case StructFieldType::Int4:
					case StructFieldType::UInt4:
						return 4 * 4;

					case StructFieldType::Double1:
						return 8;

					case StructFieldType::Double2:
						return 2 * 8;

					case StructFieldType::Double3:
					case StructFieldType::Double4:
						return 4 * 8;
				}
			}
		}

		return 0;
	}

	inline std::size_t FieldOffsets::GetCount(StructFieldType fieldType)
	{
		switch (fieldType)
		{
			case StructFieldType::Bool1:
			case StructFieldType::Double1:
			case StructFieldType::Float1:
			case StructFieldType::Int1:
			case StructFieldType::UInt1:
				return 1;

			case StructFieldType::Bool2:
			case StructFieldType::Double2:
			case StructFieldType::Float2:
			case StructFieldType::Int2:
			case StructFieldType::UInt2:
				return 2;

			case StructFieldType::Bool3:
			case StructFieldType::Double3:
			case StructFieldType::Float3:
			case StructFieldType::Int3:
			case StructFieldType::UInt3:
				return 3;

			case StructFieldType::Bool4:
			case StructFieldType::Double4:
			case StructFieldType::Float4:
			case StructFieldType::Int4:
			case StructFieldType::UInt4:
				return 4;
		}

		return 0;
	}

	inline std::size_t FieldOffsets::GetSize(StructFieldType fieldType)
	{
		switch (fieldType)
		{
			case StructFieldType::Bool1:
			case StructFieldType::Float1:
			case StructFieldType::Int1:
			case StructFieldType::UInt1:
				return 4;

			case StructFieldType::Bool2:
			case StructFieldType::Float2:
			case StructFieldType::Int2:
			case StructFieldType::UInt2:
				return 2 * 4;

			case StructFieldType::Bool3:
			case StructFieldType::Float3:
			case StructFieldType::Int3:
			case StructFieldType::UInt3:
				return 3 * 4;

			case StructFieldType::Bool4:
			case StructFieldType::Float4:
			case StructFieldType::Int4:
			case StructFieldType::UInt4:
				return 4 * 4;

			case StructFieldType::Double1:
				return 8;

			case StructFieldType::Double2:
				return 2 * 8;

			case StructFieldType::Double3:
				return 3 * 8;

			case StructFieldType::Double4:
				return 4 * 8;
		}

		return 0;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
