// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MODEL_HPP
#define NAZARA_GRAPHICS_MODEL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/Result.hpp>
#include <functional>
#include <memory>

namespace Nz
{
	struct NAZARA_GRAPHICS_API ModelParams : ResourceParameters
	{
		std::function<Result<void, ResourceLoadingError>(const std::shared_ptr<Mesh>& mesh)> meshCallback = {};
		MeshParams mesh;
		bool loadMaterials = true;

		bool IsValid() const;
	};

	class Model;

	using ModelLibrary = ObjectLibrary<Model>;
	using ModelLoader = ResourceLoader<Model, ModelParams>;
	using ModelManager = ResourceManager<Model, ModelParams>;
	using ModelSaver = ResourceSaver<Model, ModelParams>;

	class NAZARA_GRAPHICS_API Model : public InstancedRenderable, public Resource
	{
		public:
			using Params = ModelParams;

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

			inline void SetIndexCount(std::size_t subMeshIndex, std::size_t indexCount);
			inline void SetMaterial(std::size_t subMeshIndex, std::shared_ptr<MaterialInstance> material);

			Model& operator=(const Model&) = delete;
			Model& operator=(Model&&) noexcept = default;

			static std::shared_ptr<Model> BuildFromMesh(const Mesh& mesh);
			static std::shared_ptr<Model> LoadFromFile(const std::filesystem::path& filePath, const ModelParams& params = ModelParams());
			static std::shared_ptr<Model> LoadFromMemory(const void* data, std::size_t size, const ModelParams& params = ModelParams());
			static std::shared_ptr<Model> LoadFromStream(Stream& stream, const ModelParams& params = ModelParams());

		private:
			struct SubMeshData
			{
				std::size_t indexCount = 0; //< if != 0 overrides GraphicalMesh index count
				std::shared_ptr<MaterialInstance> material;
				std::vector<RenderPipelineInfo::VertexBufferData> vertexBufferData;
			};

			NazaraSlot(GraphicalMesh, OnInvalidated, m_onInvalidated);

			std::shared_ptr<GraphicalMesh> m_graphicalMesh;
			HybridVector<SubMeshData, 3> m_submeshes;
	};
}

#include <Nazara/Graphics/Model.inl>

#endif // NAZARA_GRAPHICS_MODEL_HPP
