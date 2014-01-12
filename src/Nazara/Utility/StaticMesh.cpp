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
	unsigned int vertexCount = mapper.GetVertexCount();
	for (unsigned int i = 0; i < vertexCount; ++i)
		mapper.SetPosition(i, mapper.GetPosition(i) - offset);

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
	m_vertexBuffer->AddResourceListener(this);

	return true;
}

void NzStaticMesh::Destroy()
{
	if (m_vertexBuffer)
	{
		NotifyDestroy();

		if (m_indexBuffer)
		{
			m_indexBuffer->RemoveResourceListener(this);
			m_indexBuffer = nullptr;
		}

		m_vertexBuffer->RemoveResourceListener(this);
		m_vertexBuffer = nullptr;
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
	if (m_indexBuffer)
		m_indexBuffer->RemoveResourceListener(this);

	if (indexBuffer)
		indexBuffer->AddResourceListener(this);

	m_indexBuffer = indexBuffer;
}

void NzStaticMesh::OnResourceReleased(const NzResource* resource, int index)
{
	NazaraUnused(index);

	if (resource == m_indexBuffer)
		m_indexBuffer = nullptr;
	else if (resource == m_vertexBuffer)
		m_vertexBuffer = nullptr;
	else
		NazaraInternalError("Not listening to " + NzString::Pointer(resource));
}
