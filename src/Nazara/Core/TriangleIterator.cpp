// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/TriangleIterator.hpp>
#include <Nazara/Core/SubMesh.hpp>

namespace Nz
{
	TriangleIterator::TriangleIterator(PrimitiveMode primitiveMode, IndexBuffer& indexBuffer) :
	m_primitiveMode(primitiveMode),
	m_indexMapper(indexBuffer)
	{
		m_currentIndex = 3;
		m_triangleIndices[0] = m_indexMapper.Get(0);
		m_triangleIndices[1] = m_indexMapper.Get(1);
		m_triangleIndices[2] = m_indexMapper.Get(2);

		m_indexCount = m_indexMapper.GetIndexCount();
	}

	TriangleIterator::TriangleIterator(SubMesh& subMesh) :
	m_primitiveMode(subMesh.GetPrimitiveMode()),
	m_indexMapper(subMesh)
	{
		m_currentIndex = 3;
		m_triangleIndices[0] = m_indexMapper.Get(0);
		m_triangleIndices[1] = m_indexMapper.Get(1);
		m_triangleIndices[2] = m_indexMapper.Get(2);

		m_indexCount = m_indexMapper.GetIndexCount();
	}

	bool TriangleIterator::Advance()
	{
		if (m_currentIndex >= m_indexCount)
		{
			Unmap();
			return false;
		}

		switch (m_primitiveMode)
		{
			case PrimitiveMode::TriangleFan:
				m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
				break;

			case PrimitiveMode::TriangleList:
				m_triangleIndices[0] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
				break;

			case PrimitiveMode::TriangleStrip:
				m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[0] = m_triangleIndices[1];
				m_triangleIndices[1] = m_triangleIndices[2];
				break;

			default:
				return false;
		}

		return true;
	}

	UInt32 TriangleIterator::operator[](std::size_t i) const
	{
		NazaraAssertMsg(i < 3, "Index out of range");

		return m_triangleIndices[i];
	}

	void TriangleIterator::Unmap()
	{
		// Peut très bien être appellé plusieurs fois de suite, seul le premier appel sera pris en compte
		m_indexMapper.Unmap();
	}
}
