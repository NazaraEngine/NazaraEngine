// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	StaticMesh::StaticMesh(const Mesh* parent) :
	SubMesh(parent)
	{
	}

	StaticMesh::~StaticMesh()
	{
		OnStaticMeshRelease(this);

		Destroy();
	}

	void StaticMesh::Center()
	{
		Vector3f offset(m_aabb.x + m_aabb.width/2.f, m_aabb.y + m_aabb.height/2.f, m_aabb.z + m_aabb.depth/2.f);

		VertexMapper mapper(m_vertexBuffer);
		SparsePtr<Vector3f> position = mapper.GetComponentPtr<Vector3f>(VertexComponent_Position);

		unsigned int vertexCount = m_vertexBuffer->GetVertexCount();
		for (unsigned int i = 0; i < vertexCount; ++i)
			*position++ -= offset;

		m_aabb.x -= offset.x;
		m_aabb.y -= offset.y;
		m_aabb.z -= offset.z;
	}

	bool StaticMesh::Create(VertexBuffer* vertexBuffer)
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

	void StaticMesh::Destroy()
	{
		if (m_vertexBuffer)
		{
			OnStaticMeshDestroy(this);

			m_indexBuffer.Reset();
			m_vertexBuffer.Reset();
		}
	}

	bool StaticMesh::GenerateAABB()
	{
		// On lock le buffer pour itérer sur toutes les positions et composer notre AABB
		VertexMapper mapper(m_vertexBuffer, BufferAccess_ReadOnly);
		m_aabb = ComputeAABB(mapper.GetComponentPtr<const Vector3f>(VertexComponent_Position), m_vertexBuffer->GetVertexCount());

		return true;
	}

	const Boxf& StaticMesh::GetAABB() const
	{
		return m_aabb;
	}

	AnimationType StaticMesh::GetAnimationType() const
	{
		return AnimationType_Static;
	}

	const IndexBuffer* StaticMesh::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}

	VertexBuffer* StaticMesh::GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	const VertexBuffer* StaticMesh::GetVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	unsigned int StaticMesh::GetVertexCount() const
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
	}

	void StaticMesh::SetIndexBuffer(const IndexBuffer* indexBuffer)
	{
		m_indexBuffer = indexBuffer;
}
}
