// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_FIELDOFFSETS_HPP
#define NAZARA_UTILITY_FIELDOFFSETS_HPP

#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API FieldOffsets
	{
		public:
			inline FieldOffsets(StructLayout layout);
			FieldOffsets(const FieldOffsets&) = default;
			FieldOffsets(FieldOffsets&&) = default;
			~FieldOffsets() = default;

			std::size_t AddField(StructFieldType type);
			std::size_t AddFieldArray(StructFieldType type, std::size_t arraySize);
			std::size_t AddMatrix(StructFieldType cellType, unsigned int columns, unsigned int rows, bool columnMajor);
			std::size_t AddMatrixArray(StructFieldType cellType, unsigned int columns, unsigned int rows, bool columnMajor, std::size_t arraySize);
			std::size_t AddStruct(const FieldOffsets& fieldStruct);
			std::size_t AddStructArray(const FieldOffsets& fieldStruct, std::size_t arraySize);

			inline std::size_t GetAlignedSize() const;
			inline std::size_t GetLargestFieldAlignement() const;
			inline std::size_t GetSize() const;

			FieldOffsets& operator=(const FieldOffsets&) = default;
			FieldOffsets& operator=(FieldOffsets&&) = default;

			static std::size_t GetAlignement(StructLayout layout, StructFieldType fieldType);
			static std::size_t GetCount(StructFieldType fieldType);
			static std::size_t GetSize(StructFieldType fieldType);

		private:
			std::size_t m_largestFieldAlignment;
			std::size_t m_offsetRounding;
			std::size_t m_size;
			StructLayout m_layout;
	};
}

#include <Nazara/Utility/FieldOffsets.inl>

#endif // NAZARA_UTILITY_FIELDOFFSETS_HPP
