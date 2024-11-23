// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>

namespace Nz
{
	bool ModelParams::IsValid() const
	{
		return mesh.IsValid();
	}

	Model::Model(std::shared_ptr<GraphicalMesh> graphicalMesh) :
	m_graphicalMesh(std::move(graphicalMesh))
	{
		m_submeshes.reserve(m_graphicalMesh->GetSubMeshCount());
		for (std::size_t i = 0; i < m_graphicalMesh->GetSubMeshCount(); ++i)
		{
			auto& subMeshData = m_submeshes.emplace_back();
			subMeshData.material = MaterialInstance::GetDefault(MaterialType::Basic);
			if (const auto& vertexDeclaration = m_graphicalMesh->GetVertexDeclaration(i))
			{
				subMeshData.vertexBufferData = {
					{
						0,
						vertexDeclaration
					}
				};
			}
		}

		m_onInvalidated.Connect(m_graphicalMesh->OnInvalidated, [this](GraphicalMesh*)
		{
			UpdateAABB(m_graphicalMesh->GetAABB());
			OnElementInvalidated(this);
		});

		UpdateAABB(m_graphicalMesh->GetAABB());
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

			std::size_t indexCount = (submeshData.indexCount != 0) ? submeshData.indexCount : m_graphicalMesh->GetIndexCount(i);
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
		NazaraAssertMsg(subMeshIndex < m_submeshes.size(), "material index out of range (%zu >= %zu)", subMeshIndex, m_submeshes.size());
		const auto& subMeshData = m_submeshes[subMeshIndex];
		return subMeshData.material;
	}

	std::size_t Model::GetMaterialCount() const
	{
		return m_submeshes.size();
	}

	const std::vector<RenderPipelineInfo::VertexBufferData>& Model::GetVertexBufferData(std::size_t subMeshIndex) const
	{
		NazaraAssertMsg(subMeshIndex < m_submeshes.size(), "submesh index out of range (%zu >= %zu)", subMeshIndex, m_submeshes.size());
		const auto& subMeshData = m_submeshes[subMeshIndex];
		return subMeshData.vertexBufferData;
	}

	const std::shared_ptr<RenderBuffer>& Model::GetVertexBuffer(std::size_t subMeshIndex) const
	{
		return m_graphicalMesh->GetVertexBuffer(subMeshIndex);
	}

	std::shared_ptr<Model> Model::LoadFromFile(const std::filesystem::path& filePath, const ModelParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetModelLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Model> Model::LoadFromMemory(const void* data, std::size_t size, const ModelParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetModelLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Model> Model::LoadFromStream(Stream& stream, const ModelParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetModelLoader().LoadFromStream(stream, params);
	}
}
