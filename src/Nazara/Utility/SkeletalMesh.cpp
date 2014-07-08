// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

NzSkeletalMesh::NzSkeletalMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzSkeletalMesh::~NzSkeletalMesh()
{
	Destroy();
}

bool NzSkeletalMesh::Create(NzVertexBuffer* vertexBuffer)
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

void NzSkeletalMesh::Destroy()
{
	if (m_vertexBuffer)
	{
		NotifyDestroy();

		m_indexBuffer.Reset();
		m_vertexBuffer.Reset();
	}
}

const NzBoxf& NzSkeletalMesh::GetAABB() const
{
	return m_aabb;
}

nzAnimationType NzSkeletalMesh::GetAnimationType() const
{
	return nzAnimationType_Skeletal;
}

const NzIndexBuffer* NzSkeletalMesh::GetIndexBuffer() const
{
	return m_indexBuffer;
}

NzVertexBuffer* NzSkeletalMesh::GetVertexBuffer()
{
	return m_vertexBuffer;
}

const NzVertexBuffer* NzSkeletalMesh::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

unsigned int NzSkeletalMesh::GetVertexCount() const
{
	return m_vertexBuffer->GetVertexCount();
}

bool NzSkeletalMesh::IsAnimated() const
{
	return true;
}

bool NzSkeletalMesh::IsValid() const
{
	return m_vertexBuffer != nullptr;
}

void NzSkeletalMesh::SetAABB(const NzBoxf& aabb)
{
	m_aabb = aabb;
}

void NzSkeletalMesh::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	m_indexBuffer = indexBuffer;
}
