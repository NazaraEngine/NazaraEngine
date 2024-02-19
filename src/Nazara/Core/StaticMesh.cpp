// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/VertexMapper.hpp>

namespace Nz
{
	StaticMesh::StaticMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) :
	m_aabb(Nz::Boxf::Zero()),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer))
	{
		NazaraAssert(m_vertexBuffer, "Invalid vertex buffer");
	}

	void StaticMesh::Center()
	{
		Vector3f offset(m_aabb.x + m_aabb.width/2.f, m_aabb.y + m_aabb.height/2.f, m_aabb.z + m_aabb.depth/2.f);

		VertexMapper mapper(*m_vertexBuffer);
		SparsePtr<Vector3f> position = mapper.GetComponentPtr<Vector3f>(VertexComponent::Position);

		UInt32 vertexCount = m_vertexBuffer->GetVertexCount();
		for (UInt32 i = 0; i < vertexCount; ++i)
			*position++ -= offset;

		m_aabb.x -= offset.x;
		m_aabb.y -= offset.y;
		m_aabb.z -= offset.z;
	}

	bool StaticMesh::GenerateAABB()
	{
		// On lock le buffer pour itérer sur toutes les positions et composer notre AABB
		VertexMapper mapper(*m_vertexBuffer);
		SetAABB(ComputeAABB(mapper.GetComponentPtr<const Vector3f>(VertexComponent::Position), m_vertexBuffer->GetVertexCount()));

		return true;
	}

	const Boxf& StaticMesh::GetAABB() const
	{
		return m_aabb;
	}

	AnimationType StaticMesh::GetAnimationType() const
	{
		return AnimationType::Static;
	}

	const std::shared_ptr<IndexBuffer>& StaticMesh::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}

	const std::shared_ptr<VertexBuffer>& StaticMesh::GetVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	UInt32 StaticMesh::GetVertexCount() const
	{
		return m_vertexBuffer->GetVertexCount();
	}

	bool StaticMesh::IsAnimated() const
	{
		return false;
	}

	bool StaticMesh::IsValid() const
	{
		return m_vertexBuffer != nullptr;
	}

	void StaticMesh::SetAABB(const Boxf& aabb)
	{
		m_aabb = aabb;

		OnSubMeshInvalidateAABB(this);
	}

	void StaticMesh::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		m_indexBuffer = std::move(indexBuffer);
	}
}
