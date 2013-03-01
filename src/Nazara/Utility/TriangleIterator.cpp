// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

NzTriangleIterator::NzTriangleIterator(NzSubMesh* subMesh, nzBufferAccess access) :
m_primitiveType(subMesh->GetPrimitiveType()),
m_indexMapper(subMesh->GetIndexBuffer(), nzBufferAccess_ReadOnly),
m_vertexMapper(subMesh)
{
	NazaraUnused(access);

	m_currentIndex = 3;
	m_triangleIndices[0] = m_indexMapper.Get(0);
	m_triangleIndices[1] = m_indexMapper.Get(1);
	m_triangleIndices[2] = m_indexMapper.Get(2);

	const NzIndexBuffer* indexBuffer = m_indexMapper.GetBuffer();
	if (indexBuffer)
		m_indexCount = indexBuffer->GetIndexCount();
	else
		m_indexCount = subMesh->GetVertexCount();
}

bool NzTriangleIterator::Advance()
{
	if (m_currentIndex >= m_indexCount)
	{
		Unmap();
		return false;
	}

	switch (m_primitiveType)
	{
		case nzPrimitiveType_TriangleFan:
			m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
			break;

		case nzPrimitiveType_TriangleList:
			m_triangleIndices[0] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[1] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
			break;

		case nzPrimitiveType_TriangleStrip:
			m_triangleIndices[2] = m_indexMapper.Get(m_currentIndex++);
			m_triangleIndices[1] = m_triangleIndices[2];
			m_triangleIndices[0] = m_triangleIndices[1];
			break;

		default:
			return false;
	}

	return true;
}

NzVector3f NzTriangleIterator::GetNormal(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return NzVector3f();
	}
	#endif

	return m_vertexMapper.GetNormal(m_triangleIndices[i]);
}

NzVector3f NzTriangleIterator::GetPosition(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return NzVector3f();
	}
	#endif

	return m_vertexMapper.GetPosition(m_triangleIndices[i]);
}

NzVector3f NzTriangleIterator::GetTangent(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return NzVector3f();
	}
	#endif

	return m_vertexMapper.GetTangent(m_triangleIndices[i]);
}

NzVector2f NzTriangleIterator::GetTexCoords(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return NzVector2f();
	}
	#endif

	return m_vertexMapper.GetTexCoords(m_triangleIndices[i]);
}

void NzTriangleIterator::SetNormal(unsigned int i, const NzVector3f& normal)
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return;
	}
	#endif

	m_vertexMapper.SetNormal(m_triangleIndices[i], normal);
}

void NzTriangleIterator::SetPosition(unsigned int i, const NzVector3f& position)
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return;
	}
	#endif

	m_vertexMapper.SetPosition(m_triangleIndices[i], position);
}

void NzTriangleIterator::SetTangent(unsigned int i, const NzVector3f& tangent)
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return;
	}
	#endif

	m_vertexMapper.SetTangent(m_triangleIndices[i], tangent);
}

void NzTriangleIterator::SetTexCoords(unsigned int i, const NzVector2f& texCoords)
{
	#if NAZARA_UTILITY_SAFE
	if (i > 2)
	{
		NazaraError("Index out of range: (" + NzString::Number(i) + " > 2)");
		return;
	}
	#endif

	m_vertexMapper.SetTexCoords(m_triangleIndices[i], texCoords);
}

void NzTriangleIterator::Unmap()
{
	// Peut très bien être appellé plusieurs fois de suite, seul le premier appel sera pris en compte
	m_indexMapper.Unmap();
	m_vertexMapper.Unmap();
}
