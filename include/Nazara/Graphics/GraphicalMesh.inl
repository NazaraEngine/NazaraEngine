// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline std::size_t GraphicalMesh::AddSubmesh(Submesh submesh)
	{
		std::size_t submeshIndex = m_submeshes.size();
		m_submeshes.emplace_back(std::move(submesh));

		OnInvalidated(this);

		return submeshIndex;
	}

	inline void GraphicalMesh::Clear()
	{
		m_submeshes.clear();

		OnInvalidated(this);
	}

	inline const Boxf& GraphicalMesh::GetAABB(std::size_t submeshIndex) const
	{
		assert(submeshIndex < m_submeshes.size());
		return m_submeshes[submeshIndex].aabb;
	}

	inline const std::shared_ptr<GpuBuffer>& GraphicalMesh::GetIndexBuffer(std::size_t submeshIndex) const
	{
		assert(submeshIndex < m_submeshes.size());
		return m_submeshes[submeshIndex].indexBuffer;
	}

	inline UInt32 GraphicalMesh::GetIndexCount(std::size_t submeshIndex) const
	{
		assert(submeshIndex < m_submeshes.size());
		return m_submeshes[submeshIndex].indexCount;
	}

	inline IndexType GraphicalMesh::GetIndexType(std::size_t submeshIndex) const
	{
		assert(submeshIndex < m_submeshes.size());
		return m_submeshes[submeshIndex].indexType;
	}

	inline const std::shared_ptr<GpuBuffer>& GraphicalMesh::GetVertexBuffer(std::size_t submeshIndex) const
	{
		assert(submeshIndex < m_submeshes.size());
		return m_submeshes[submeshIndex].vertexBuffer;
	}

	inline const std::shared_ptr<const VertexDeclaration>& GraphicalMesh::GetVertexDeclaration(std::size_t submeshIndex) const
	{
		assert(submeshIndex < m_submeshes.size());
		return m_submeshes[submeshIndex].vertexDeclaration;
	}

	inline std::size_t GraphicalMesh::GetSubmeshCount() const
	{
		return m_submeshes.size();
	}

	inline void GraphicalMesh::UpdateSubmeshIndexCount(std::size_t submeshIndex, UInt32 indexCount)
	{
		NazaraAssertMsg(submeshIndex < m_submeshes.size(), "invalid submesh index");
		m_submeshes[submeshIndex].indexCount = indexCount;

		OnInvalidated(this);
	}

	inline std::shared_ptr<GraphicalMesh> GraphicalMesh::Build(const Primitive& primitive, const MeshParams& params)
	{
		Mesh mesh;
		mesh.CreateStatic();
		mesh.BuildSubmesh(primitive, params);

		return BuildFromMesh(mesh);
	}

	inline std::shared_ptr<GraphicalMesh> GraphicalMesh::Build(const PrimitiveList& primitiveList, const MeshParams& params)
	{
		Mesh mesh;
		mesh.CreateStatic();
		mesh.BuildSubmeshes(primitiveList, params);

		return BuildFromMesh(mesh);
	}

}
