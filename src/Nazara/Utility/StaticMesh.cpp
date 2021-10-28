// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	StaticMesh::StaticMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<const IndexBuffer> indexBuffer) :
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

		std::size_t vertexCount = m_vertexBuffer->GetVertexCount();
		for (std::size_t i = 0; i < vertexCount; ++i)
			*position++ -= offset;

		m_aabb.x -= offset.x;
		m_aabb.y -= offset.y;
		m_aabb.z -= offset.z;
	}

	bool StaticMesh::GenerateAABB()
	{
		// On lock le buffer pour itérer sur toutes les positions et composer notre AABB
		VertexMapper mapper(*m_vertexBuffer, BufferAccess::ReadOnly);
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

	const std::shared_ptr<const IndexBuffer>& StaticMesh::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}

	const std::shared_ptr<VertexBuffer>& StaticMesh::GetVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	std::size_t StaticMesh::GetVertexCount() const
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

	void StaticMesh::SetIndexBuffer(std::shared_ptr<const IndexBuffer> indexBuffer)
	{
		m_indexBuffer = std::move(indexBuffer);
	}
}
