// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GraphicalMesh.hpp>
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

	inline void GraphicalMesh::UpdateSubMeshIndexCount(std::size_t subMeshIndex, UInt32 indexCount)
	{
		NazaraAssert(subMeshIndex < m_subMeshes.size(), "invalid submesh index");
		m_subMeshes[subMeshIndex].indexCount = indexCount;

		OnInvalidated(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
