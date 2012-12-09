// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Mesh.hpp>
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
	m_aabb.SetNull();

	if (m_indexBuffer)
	{
		m_indexBuffer->RemoveResourceListener(this);
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->RemoveResourceListener(this);
		m_vertexBuffer = nullptr;
	}
}

void NzStaticMesh::Finish()
{
	// La seule chose à faire ici est de calculer l'AABB
	GenerateAABB();
}

bool NzStaticMesh::GenerateAABB()
{
	if (!m_aabb.IsNull())
		return true;

	// On lock le buffer pour itérer sur toutes les positions et composer notre AABB
	NzMeshVertex* vertex = reinterpret_cast<NzMeshVertex*>(m_vertexBuffer->Map(nzBufferAccess_ReadOnly));
	if (!vertex)
	{
		NazaraWarning("Failed to lock vertex buffer");
		return false;
	}

	unsigned int vertexCount = m_vertexBuffer->GetVertexCount();
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		m_aabb.ExtendTo(vertex->position);
		vertex++;
	}

	if (!m_vertexBuffer->Unmap())
		NazaraWarning("Failed to unmap vertex buffer");

	return true;
}

const NzAxisAlignedBox& NzStaticMesh::GetAABB() const
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

const NzVertexBuffer* NzStaticMesh::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

bool NzStaticMesh::IsAnimated() const
{
	return false;
}

bool NzStaticMesh::IsValid() const
{
	return m_vertexBuffer != nullptr;
}

void NzStaticMesh::SetAABB(const NzAxisAlignedBox& aabb)
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

	resource->RemoveResourceListener(this);
}
