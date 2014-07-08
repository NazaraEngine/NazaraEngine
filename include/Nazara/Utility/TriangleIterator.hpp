// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TRIANGLEITERATOR_HPP
#define NAZARA_TRIANGLEITERATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexMapper.hpp>

class NzSubMesh;

class NAZARA_API NzTriangleIterator
{
	public:
		NzTriangleIterator(nzPrimitiveMode primitiveMode, const NzIndexBuffer* indexBuffer);
		NzTriangleIterator(NzSubMesh* subMesh);
		~NzTriangleIterator() = default;

		bool Advance();

		nzUInt32 operator[](unsigned int i) const;

		void Unmap();

	private:
		nzPrimitiveMode m_primitiveMode;
		nzUInt32 m_triangleIndices[3];
		NzIndexMapper m_indexMapper;
		unsigned int m_currentIndex;
		unsigned int m_indexCount;
};

#endif // NAZARA_TRIANGLEITERATOR_HPP
