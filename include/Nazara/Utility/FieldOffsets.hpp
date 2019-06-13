// Copyright (C) 2019 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FIELDOFFSETS_HPP
#define NAZARA_FIELDOFFSETS_HPP

#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class FieldOffsets
	{
		public:
			inline FieldOffsets(StructLayout layout);
			FieldOffsets(const FieldOffsets&) = default;
			FieldOffsets(FieldOffsets&&) = default;
			~FieldOffsets() = default;

			std::size_t AddField(StructFieldType type);

			std::size_t GetSize() const;

			FieldOffsets& operator=(const FieldOffsets&) = default;
			FieldOffsets& operator=(FieldOffsets&&) = default;

			static std::size_t GetAlignement(StructLayout layout, StructFieldType fieldType);
			static std::size_t GetSize(StructFieldType fieldType);

		private:
			std::size_t m_offset;
			StructFieldType m_previousType;
			StructLayout m_layout;
	};
}

#include <Nazara/Utility/FieldOffsets.inl>

#endif // NAZARA_FIELDOFFSETS_HPP
