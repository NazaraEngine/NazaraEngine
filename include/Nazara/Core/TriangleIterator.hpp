// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TRIANGLEITERATOR_HPP
#define NAZARA_CORE_TRIANGLEITERATOR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/IndexMapper.hpp>

namespace Nz
{
	class SubMesh;

	class NAZARA_CORE_API TriangleIterator
	{
		public:
			TriangleIterator(PrimitiveMode primitiveMode, IndexBuffer& indexBuffer);
			TriangleIterator(SubMesh& subMesh);
			~TriangleIterator() = default;

			bool Advance();

			UInt32 operator[](std::size_t i) const;

			void Unmap();

		private:
			PrimitiveMode m_primitiveMode;
			UInt32 m_triangleIndices[3];
			IndexMapper m_indexMapper;
			std::size_t m_currentIndex;
			std::size_t m_indexCount;
	};
}

#endif // NAZARA_CORE_TRIANGLEITERATOR_HPP
