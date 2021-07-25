// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Model::Model(std::shared_ptr<GraphicalMesh> graphicalMesh) :
	m_graphicalMesh(std::move(graphicalMesh))
	{
		m_submeshes.reserve(m_graphicalMesh->GetSubMeshCount());
		for (std::size_t i = 0; i < m_graphicalMesh->GetSubMeshCount(); ++i)
		{
			auto& subMeshData = m_submeshes.emplace_back();
			//subMeshData.material = DefaultMaterial; //< TODO
			subMeshData.vertexBufferData = {
				{
					0,
					m_graphicalMesh->GetVertexDeclaration(i)
				}
			};
		}
	}

	void Model::BuildElement(const std::string& pass, WorldInstance& worldInstance, std::vector<std::unique_ptr<RenderElement>>& elements) const
	{
		for (std::size_t i = 0; i < m_submeshes.size(); ++i)
		{
			const auto& submeshData = m_submeshes[i];

			MaterialPass* materialPass = submeshData.material->GetPass(pass);
			if (!materialPass)
				continue;

			const auto& indexBuffer = m_graphicalMesh->GetIndexBuffer(i);
			const auto& vertexBuffer = m_graphicalMesh->GetVertexBuffer(i);
			const auto& renderPipeline = materialPass->GetPipeline()->GetRenderPipeline(submeshData.vertexBufferData);

			elements.emplace_back(std::make_unique<RenderSubmesh>(0, renderPipeline, m_graphicalMesh->GetIndexCount(i), indexBuffer, vertexBuffer, worldInstance.GetShaderBinding(), materialPass->GetShaderBinding()));
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

	const std::shared_ptr<Material>& Model::GetMaterial(std::size_t subMeshIndex) const
	{
		assert(subMeshIndex < m_submeshes.size());
		const auto& subMeshData = m_submeshes[subMeshIndex];
		return subMeshData.material;
	}

	std::size_t Model::GetMaterialCount() const
	{
		return m_submeshes.size();
	}

	const std::vector<RenderPipelineInfo::VertexBufferData>& Model::GetVertexBufferData(std::size_t subMeshIndex) const
	{
		assert(subMeshIndex < m_submeshes.size());
		const auto& subMeshData = m_submeshes[subMeshIndex];
		return subMeshData.vertexBufferData;
	}

	const std::shared_ptr<AbstractBuffer>& Model::GetVertexBuffer(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetVertexBuffer(subMeshIndex);
	}
}
