// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MODEL_HPP
#define NAZARA_GRAPHICS_MODEL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>

namespace Nz
{
	class Material;

	class NAZARA_GRAPHICS_API Model : public InstancedRenderable
	{
		public:
			Model(std::shared_ptr<GraphicalMesh> graphicalMesh);
			Model(const Model&) = delete;
			Model(Model&&) noexcept = default;
			~Model() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			const std::shared_ptr<RenderBuffer>& GetIndexBuffer(std::size_t subMeshIndex) const;
			std::size_t GetIndexCount(std::size_t subMeshIndex) const;
			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t subMeshIndex) const override;
			std::size_t GetMaterialCount() const override;
			inline std::size_t GetSubMeshCount() const;
			const std::vector<RenderPipelineInfo::VertexBufferData>& GetVertexBufferData(std::size_t subMeshIndex) const;
			const std::shared_ptr<RenderBuffer>& GetVertexBuffer(std::size_t subMeshIndex) const;

			inline void SetMaterial(std::size_t subMeshIndex, std::shared_ptr<MaterialInstance> material);

			Model& operator=(const Model&) = delete;
			Model& operator=(Model&&) noexcept = default;

		private:
			struct SubMeshData
			{
				std::shared_ptr<MaterialInstance> material;
				std::vector<RenderPipelineInfo::VertexBufferData> vertexBufferData;
			};

			NazaraSlot(GraphicalMesh, OnInvalidated, m_onInvalidated);

			std::shared_ptr<GraphicalMesh> m_graphicalMesh;
			std::vector<SubMeshData> m_submeshes;
	};
}

#include <Nazara/Graphics/Model.inl>

#endif // NAZARA_GRAPHICS_MODEL_HPP
