// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

NzTriangleIterator::NzTriangleIterator(nzPrimitiveMode primitiveMode, const NzIndexBuffer* indexBuffer) :
m_primitiveMode(primitiveMode),
m_indexMapper(indexBuffer, nzBufferAccess_ReadOnly)
{
	m_currentIndex = 3;
	m_triangleIndices[0] = m_indexMapper.Get(0);
	m_triangleIndices[1] = m_indexMapper.Get(1);
	m_triangleIndices[2] = m_indexMapper.Get(2);

	m_indexCount = indexBuffer->GetIndexCount();
}

NzTriangleIterator::NzTriangleIterator(NzSubMesh* subMesh) :
NzTriangleIterator(subMesh->GetPrimitiveMode(), subMesh->GetIndexBuffer())
{
}

bool NzTriangleIterator::Advance()
{
	if (m_currentIndex >= m_indexCount)
	{
		Unmap();
		return false;
	}

	switch (m_primitiveMode)
	{
		case nzPrimitiveMode_TriangleFan:
			m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
			break;

		case nzPrimitiveMode_TriangleList:
			m_triangleIndices[0] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
			break;

		case nzPrimitiveMode_TriangleStrip:
			m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[1] = m_triangleIndices[2];
			m_triangleIndices[0] = m_triangleIndices[1];
			break;

		default:
			return false;
	}

	return true;
}

nzUInt32 NzTriangleIterator::operator[](unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i >= 3)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 3)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return m_triangleIndices[i];
}

void NzTriangleIterator::Unmap()
{
	// Peut très bien être appellé plusieurs fois de suite, seul le premier appel sera pris en compte
	m_indexMapper.Unmap();
}
