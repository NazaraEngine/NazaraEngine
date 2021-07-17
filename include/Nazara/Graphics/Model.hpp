// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>

namespace Nz
{
	class GraphicalMesh;
	class Material;

	class NAZARA_GRAPHICS_API Model : public InstancedRenderable
	{
		public:
			Model(std::shared_ptr<GraphicalMesh> graphicalMesh);
			Model(const Model&) = delete;
			Model(Model&&) noexcept = default;
			~Model() = default;

			void Draw(const std::string& pass, CommandBufferBuilder& commandBuffer) const override;

			const std::shared_ptr<AbstractBuffer>& GetIndexBuffer(std::size_t subMeshIndex) const;
			std::size_t GetIndexCount(std::size_t subMeshIndex) const;
			const std::shared_ptr<Material>& GetMaterial(std::size_t subMeshIndex) const override;
			std::size_t GetMaterialCount() const override;
			const std::vector<RenderPipelineInfo::VertexBufferData>& GetVertexBufferData(std::size_t subMeshIndex) const;
			const std::shared_ptr<AbstractBuffer>& GetVertexBuffer(std::size_t subMeshIndex) const;
			inline std::size_t GetSubMeshCount() const;

			inline void SetMaterial(std::size_t subMeshIndex, std::shared_ptr<Material> material);

			Model& operator=(const Model&) = delete;
			Model& operator=(Model&&) noexcept = default;

		private:
			struct SubMeshData
			{
				std::shared_ptr<Material> material;
				std::vector<RenderPipelineInfo::VertexBufferData> vertexBufferData;
			};

			std::shared_ptr<GraphicalMesh> m_graphicalMesh;
			std::vector<SubMeshData> m_subMeshes;
	};
}

#include <Nazara/Graphics/Model.inl>

#endif
