// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Model::Model(std::shared_ptr<GraphicalMesh> graphicalMesh) :
	m_graphicalMesh(std::move(graphicalMesh))
	{
		m_subMeshes.reserve(m_graphicalMesh->GetSubMeshCount());
		for (std::size_t i = 0; i < m_graphicalMesh->GetSubMeshCount(); ++i)
		{
			auto& subMeshData = m_subMeshes.emplace_back();
			//subMeshData.material = DefaultMaterial; //< TODO
			subMeshData.vertexBufferData = {
				{
					0,
					m_graphicalMesh->GetVertexDeclaration(i)
				}
			};
		}
	}

	const std::shared_ptr<AbstractBuffer>& Model::GetIndexBuffer(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetIndexBuffer(subMeshIndex);
	}

	std::size_t Model::GetIndexCount(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetIndexCount(subMeshIndex);
	}

	const std::shared_ptr<RenderPipeline>& Model::GetRenderPipeline(std::size_t subMeshIndex) const
	{
		assert(subMeshIndex < m_subMeshes.size());
		const auto& subMeshData = m_subMeshes[subMeshIndex];
		return subMeshData.material->GetPipeline()->GetRenderPipeline(subMeshData.vertexBufferData);
	}

	const std::shared_ptr<AbstractBuffer>& Model::GetVertexBuffer(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetVertexBuffer(subMeshIndex);
	}

}
