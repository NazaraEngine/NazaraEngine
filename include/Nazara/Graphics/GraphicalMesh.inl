// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<AbstractBuffer>& GraphicalMesh::GetIndexBuffer(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].indexBuffer;
	}

	inline std::size_t GraphicalMesh::GetIndexCount(std::size_t subMesh) const
	{
		assert(subMesh < m_subMeshes.size());
		return m_subMeshes[subMesh].indexCount;
	}

	inline const std::shared_ptr<AbstractBuffer>& GraphicalMesh::GetVertexBuffer(std::size_t subMesh) const
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
}

#include <Nazara/Graphics/DebugOff.hpp>
