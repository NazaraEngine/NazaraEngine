// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Model::Model(std::shared_ptr<GraphicalMesh> graphicalMesh, const Boxf& aabb) :
	m_graphicalMesh(std::move(graphicalMesh))
	{
		Graphics* graphics = Graphics::Instance();

		m_submeshes.reserve(m_graphicalMesh->GetSubMeshCount());
		for (std::size_t i = 0; i < m_graphicalMesh->GetSubMeshCount(); ++i)
		{
			auto& subMeshData = m_submeshes.emplace_back();
			subMeshData.material = graphics->GetDefaultMaterials().basicDefault;
			subMeshData.vertexBufferData = {
				{
					0,
					m_graphicalMesh->GetVertexDeclaration(i)
				}
			};
		}

		m_onInvalidated.Connect(m_graphicalMesh->OnInvalidated, [this](GraphicalMesh*)
		{
			OnElementInvalidated(this);
		});

		UpdateAABB(aabb);
	}

	void Model::BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const
	{
		for (std::size_t i = 0; i < m_submeshes.size(); ++i)
		{
			const auto& submeshData = m_submeshes[i];

			const auto& materialPipeline = submeshData.material->GetPipeline(passIndex);
			if (!materialPipeline)
				continue;

			MaterialPassFlags passFlags = submeshData.material->GetPassFlags(passIndex);

			const auto& indexBuffer = m_graphicalMesh->GetIndexBuffer(i);
			const auto& vertexBuffer = m_graphicalMesh->GetVertexBuffer(i);
			const auto& renderPipeline = materialPipeline->GetRenderPipeline(submeshData.vertexBufferData.data(), submeshData.vertexBufferData.size());

			std::size_t indexCount = m_graphicalMesh->GetIndexCount(i);
			IndexType indexType = m_graphicalMesh->GetIndexType(i);

			elements.emplace_back(registry.AllocateElement<RenderSubmesh>(GetRenderLayer(), submeshData.material, passFlags, renderPipeline, *elementData.worldInstance, elementData.skeletonInstance, indexCount, indexType, indexBuffer, vertexBuffer, *elementData.scissorBox));
		}
	}

	const std::shared_ptr<RenderBuffer>& Model::GetIndexBuffer(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetIndexBuffer(subMeshIndex);
	}

	std::size_t Model::GetIndexCount(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetIndexCount(subMeshIndex);
	}

	const std::shared_ptr<MaterialInstance>& Model::GetMaterial(std::size_t subMeshIndex) const
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

	const std::shared_ptr<RenderBuffer>& Model::GetVertexBuffer(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetVertexBuffer(subMeshIndex);
	}
}
