// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

NzVertexMapper::NzVertexMapper(NzSubMesh* subMesh)
{
	#ifdef NAZARA_DEBUG
	m_vertices = nullptr; // Pour détecter les erreurs
	#endif

	switch (subMesh->GetAnimationType())
	{
		case nzAnimationType_Skeletal:
			m_vertices = reinterpret_cast<NzMeshVertex*>(static_cast<NzSkeletalMesh*>(subMesh)->GetBindPoseBuffer());
			break;

		case nzAnimationType_Static:
			if (!m_mapper.Map(static_cast<NzStaticMesh*>(subMesh)->GetVertexBuffer(), nzBufferAccess_ReadWrite))
				NazaraError("Failed to map buffer"); ///TODO: Unexpected

			m_vertices = reinterpret_cast<NzMeshVertex*>(m_mapper.GetPointer());
			break;
	}

	#ifdef NAZARA_DEBUG
	if (!m_vertices)
		NazaraInternalError("No vertices"); ///TODO: Internal, Unexpected
	#endif

	m_vertexCount = subMesh->GetVertexCount();
}

NzVertexMapper::~NzVertexMapper() = default;

NzVector3f NzVertexMapper::GetNormal(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return NzVector3f();
	}
	#endif

	return m_vertices[i].normal;
}

NzVector3f NzVertexMapper::GetPosition(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return NzVector3f();
	}
	#endif

	return m_vertices[i].position;
}

NzVector3f NzVertexMapper::GetTangent(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return NzVector3f();
	}
	#endif

	return m_vertices[i].tangent;
}

NzVector2f NzVertexMapper::GetTexCoord(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return NzVector2f();
	}
	#endif

	return m_vertices[i].uv;
}

unsigned int NzVertexMapper::GetVertexCount()
{
	return m_vertexCount;
}

void NzVertexMapper::SetNormal(unsigned int i, const NzVector3f& normal)
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return;
	}
	#endif

	m_vertices[i].normal = normal;
}

void NzVertexMapper::SetPosition(unsigned int i, const NzVector3f& position)
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return;
	}
	#endif

	m_vertices[i].position = position;
}

void NzVertexMapper::SetTangent(unsigned int i, const NzVector3f& tangent)
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return;
	}
	#endif

	m_vertices[i].tangent = tangent;
}

void NzVertexMapper::SetTexCoord(unsigned int i, const NzVector2f& texCoord)
{
	#if NAZARA_UTILITY_SAFE
	if (i >= m_vertexCount)
	{
		NazaraError("Vertex index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_vertexCount) + ')');
		return;
	}
	#endif

	m_vertices[i].uv = texCoord;
}

void NzVertexMapper::Unmap()
{
	m_mapper.Unmap();
}
