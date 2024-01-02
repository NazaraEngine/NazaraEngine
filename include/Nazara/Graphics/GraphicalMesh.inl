// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t GraphicalMesh::AddSubMesh(SubMesh subMesh)
	{
		std::size_t subMeshIndex = m_subMeshes.size();
		m_subMeshes.emplace_back(std::move(subMesh));

		OnInvalidated(this);

		return subMeshIndex;
	}

	inline void GraphicalMesh::Clear()
	{
		m_subMeshes.clear();

		OnInvalidated(this);
	}

	inline const Boxf& GraphicalMesh::GetAABB() const
	{
		return m_aabb;
	}

	inline const std::shared_ptr<RenderBuffer>& GraphicalMesh::GetIndexBuffer(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].indexBuffer;
	}

	inline UInt32 GraphicalMesh::GetIndexCount(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].indexCount;
	}

	inline IndexType GraphicalMesh::GetIndexType(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].indexType;
	}

	inline const std::shared_ptr<RenderBuffer>& GraphicalMesh::GetVertexBuffer(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].vertexBuffer;
	}

	inline const std::shared_ptr<const VertexDeclaration>& GraphicalMesh::GetVertexDeclaration(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].vertexDeclaration;
	}

	inline std::size_t GraphicalMesh::GetSubMeshCount() const
	{
		return m_subMeshes.size();
	}

	inline void GraphicalMesh::UpdateAABB(const Boxf& aabb)
	{
		m_aabb = aabb;

		OnInvalidated(this);
	}

	inline void GraphicalMesh::UpdateSubMeshIndexCount(std::size_t subMeshIndex, UInt32 indexCount)
	{
		NazaraAssert(subMeshIndex < m_subMeshes.size(), "invalid submesh index");
		m_subMeshes[subMeshIndex].indexCount = indexCount;

		OnInvalidated(this);
	}

	inline std::shared_ptr<GraphicalMesh> GraphicalMesh::Build(const Primitive& primitive, const MeshParams& params)
	{
		Mesh mesh;
		mesh.CreateStatic();
		mesh.BuildSubMesh(primitive, params);

		return BuildFromMesh(mesh);
	}

	inline std::shared_ptr<GraphicalMesh> GraphicalMesh::Build(const PrimitiveList& primitiveList, const MeshParams& params)
	{
		Mesh mesh;
		mesh.CreateStatic();
		mesh.BuildSubMeshes(primitiveList, params);

		return BuildFromMesh(mesh);
	}

}

#include <Nazara/Graphics/DebugOff.hpp>
