// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
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

	void Model::Draw(CommandBufferBuilder& commandBuffer) const
	{
		for (std::size_t i = 0; i < m_subMeshes.size(); ++i)
		{
			const auto& submeshData = m_subMeshes[i];
			const auto& indexBuffer = m_graphicalMesh->GetIndexBuffer(i);
			const auto& vertexBuffer = m_graphicalMesh->GetVertexBuffer(i);
			const auto& renderPipeline = submeshData.material->GetPipeline()->GetRenderPipeline(submeshData.vertexBufferData);

			commandBuffer.BindShaderBinding(Graphics::MaterialBindingSet, submeshData.material->GetShaderBinding());
			commandBuffer.BindIndexBuffer(indexBuffer.get());
			commandBuffer.BindVertexBuffer(0, vertexBuffer.get());
			commandBuffer.BindPipeline(*renderPipeline);

			commandBuffer.DrawIndexed(static_cast<Nz::UInt32>(m_graphicalMesh->GetIndexCount(i)));
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

	const std::shared_ptr<MaterialPass>& Model::GetMaterial(std::size_t subMeshIndex) const
	{
		assert(subMeshIndex < m_subMeshes.size());
		const auto& subMeshData = m_subMeshes[subMeshIndex];
		return subMeshData.material;
	}

	std::size_t Model::GetMaterialCount() const
	{
		return m_subMeshes.size();
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
