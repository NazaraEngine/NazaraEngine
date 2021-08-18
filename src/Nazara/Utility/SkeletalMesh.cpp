// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SkeletalMesh::SkeletalMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<const IndexBuffer> indexBuffer) :
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

	const std::shared_ptr<const IndexBuffer>& SkeletalMesh::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}

	const std::shared_ptr<VertexBuffer>& SkeletalMesh::GetVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	std::size_t SkeletalMesh::GetVertexCount() const
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

	void SkeletalMesh::SetIndexBuffer(std::shared_ptr<const IndexBuffer> indexBuffer)
	{
		m_indexBuffer = std::move(indexBuffer);
	}
}
