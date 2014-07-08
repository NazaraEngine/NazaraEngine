// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzStaticMesh::NzStaticMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzStaticMesh::~NzStaticMesh()
{
	Destroy();
}

void NzStaticMesh::Center()
{
	NzVector3f offset(m_aabb.x + m_aabb.width/2.f, m_aabb.y + m_aabb.height/2.f, m_aabb.z + m_aabb.depth/2.f);

	NzVertexMapper mapper(this);
	NzSparsePtr<NzVector3f> position = mapper.GetAttributePtr<NzVector3f>(nzAttributeUsage_Position);

	unsigned int vertexCount = mapper.GetVertexCount();
	for (unsigned int i = 0; i < vertexCount; ++i)
		position[i] -= offset;

	m_aabb.x -= offset.x;
	m_aabb.y -= offset.y;
	m_aabb.z -= offset.z;
}

bool NzStaticMesh::Create(NzVertexBuffer* vertexBuffer)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (!vertexBuffer)
	{
		NazaraError("Invalid vertex buffer");
		return false;
	}
	#endif

	m_vertexBuffer = vertexBuffer;
	return true;
}

void NzStaticMesh::Destroy()
{
	if (m_vertexBuffer)
	{
		NotifyDestroy();

		m_indexBuffer.Reset();
		m_vertexBuffer.Reset();
	}
}

bool NzStaticMesh::GenerateAABB()
{
	// On lock le buffer pour itérer sur toutes les positions et composer notre AABB
	NzBufferMapper<NzVertexBuffer> mapper(m_vertexBuffer, nzBufferAccess_ReadOnly);
	m_aabb = NzComputeVerticesAABB(static_cast<const NzMeshVertex*>(mapper.GetPointer()), m_vertexBuffer->GetVertexCount());

	return true;
}

const NzBoxf& NzStaticMesh::GetAABB() const
{
	return m_aabb;
}

nzAnimationType NzStaticMesh::GetAnimationType() const
{
	return nzAnimationType_Static;
}

const NzIndexBuffer* NzStaticMesh::GetIndexBuffer() const
{
	return m_indexBuffer;
}

NzVertexBuffer* NzStaticMesh::GetVertexBuffer()
{
	return m_vertexBuffer;
}

const NzVertexBuffer* NzStaticMesh::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

unsigned int NzStaticMesh::GetVertexCount() const
{
	return m_vertexBuffer->GetVertexCount();
}

bool NzStaticMesh::IsAnimated() const
{
	return false;
}

bool NzStaticMesh::IsValid() const
{
	return m_vertexBuffer != nullptr;
}

void NzStaticMesh::SetAABB(const NzBoxf& aabb)
{
	m_aabb = aabb;
}

void NzStaticMesh::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	m_indexBuffer = indexBuffer;
}
