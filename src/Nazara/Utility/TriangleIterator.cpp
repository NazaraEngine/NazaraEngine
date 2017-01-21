// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	TriangleIterator::TriangleIterator(PrimitiveMode primitiveMode, const IndexBuffer* indexBuffer) :
	m_primitiveMode(primitiveMode),
	m_indexMapper(indexBuffer, BufferAccess_ReadOnly)
	{
		m_currentIndex = 3;
		m_triangleIndices[0] = m_indexMapper.Get(0);
		m_triangleIndices[1] = m_indexMapper.Get(1);
		m_triangleIndices[2] = m_indexMapper.Get(2);

		m_indexCount = m_indexMapper.GetIndexCount();
	}

	TriangleIterator::TriangleIterator(const SubMesh* subMesh) :
	m_primitiveMode(subMesh->GetPrimitiveMode()),
	m_indexMapper(subMesh, BufferAccess_ReadOnly)
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
			case PrimitiveMode_TriangleFan:
				m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
				break;

			case PrimitiveMode_TriangleList:
				m_triangleIndices[0] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
				break;

			case PrimitiveMode_TriangleStrip:
				m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
				m_triangleIndices[0] = m_triangleIndices[1];
				m_triangleIndices[1] = m_triangleIndices[2];
				break;

			default:
				return false;
		}

		return true;
	}

	UInt32 TriangleIterator::operator[](unsigned int i) const
	{
		#if NAZARA_UTILITY_SAFE
		if (i >= 3)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 3)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return m_triangleIndices[i];
	}

	void TriangleIterator::Unmap()
	{
		// Peut très bien être appellé plusieurs fois de suite, seul le premier appel sera pris en compte
		m_indexMapper.Unmap();
	}
}
