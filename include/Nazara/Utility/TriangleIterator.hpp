// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TRIANGLEITERATOR_HPP
#define NAZARA_TRIANGLEITERATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexMapper.hpp>

namespace Nz
{
	class SubMesh;

	class NAZARA_UTILITY_API TriangleIterator
	{
		public:
			TriangleIterator(PrimitiveMode primitiveMode, const IndexBuffer* indexBuffer);
			TriangleIterator(const SubMesh* subMesh);
			~TriangleIterator() = default;

			bool Advance();

			UInt32 operator[](unsigned int i) const;

			void Unmap();

		private:
			PrimitiveMode m_primitiveMode;
			UInt32 m_triangleIndices[3];
			IndexMapper m_indexMapper;
			unsigned int m_currentIndex;
			unsigned int m_indexCount;
	};
}

#endif // NAZARA_TRIANGLEITERATOR_HPP
