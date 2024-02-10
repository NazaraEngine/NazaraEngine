// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/SkeletalMesh.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SkeletalMesh::SkeletalMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) :
	m_aabb(Nz::Boxf::Zero()),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer))
	{
		NazaraAssert(m_vertexBuffer, "Invalid vertex buffer");
	}

	const Boxf& SkeletalMesh::GetAABB() const
	{
		return m_aabb;
	}

	AnimationType SkeletalMesh::GetAnimationType() const
	{
		return AnimationType::Skeletal;
	}

	const std::shared_ptr<IndexBuffer>& SkeletalMesh::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}

	const std::shared_ptr<VertexBuffer>& SkeletalMesh::GetVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	UInt32 SkeletalMesh::GetVertexCount() const
	{
		return m_vertexBuffer->GetVertexCount();
	}

	bool SkeletalMesh::IsAnimated() const
	{
		return true;
	}

	bool SkeletalMesh::IsValid() const
	{
		return m_vertexBuffer != nullptr;
	}

	void SkeletalMesh::SetAABB(const Boxf& aabb)
	{
		m_aabb = aabb;

		OnSubMeshInvalidateAABB(this);
	}

	void SkeletalMesh::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		m_indexBuffer = std::move(indexBuffer);
	}
}
