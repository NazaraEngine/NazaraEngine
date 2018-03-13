// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <map>
#include <set>
#include <unordered_map>

namespace Nz
{
	class AbstractViewer;

	class NAZARA_GRAPHICS_API ForwardRenderQueue : public AbstractRenderQueue
	{
		friend class ForwardRenderTechnique;

		public:
			struct BillboardData;

			ForwardRenderQueue() = default;
			~ForwardRenderQueue() = default;

			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) override;
			void AddDrawable(int renderOrder, const Drawable* drawable) override;
			void AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix, const Recti& scissorRect) override;
			void AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, std::size_t spriteCount, const Recti& scissorRect, const Texture* overlay = nullptr) override;

			void Clear(bool fully = false) override;

			inline const BillboardData* GetBillboardData(std::size_t billboardIndex) const;

			void Sort(const AbstractViewer* viewer);

			struct MaterialComparator
			{
				bool operator()(const Material* mat1, const Material* mat2) const;
			};

			struct MaterialPipelineComparator
			{
				bool operator()(const MaterialPipeline* pipeline1, const MaterialPipeline* pipeline2) const;
			};

			/// Billboards
			struct BillboardData
			{
				Color color;
				Vector3f center;
				Vector2f size;
				Vector2f sinCos;
			};

			struct BatchedBillboardEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				std::vector<BillboardData> billboards;
			};

			using BatchedBillboardContainer = std::map<const Material*, BatchedBillboardEntry, MaterialComparator>;

			struct BatchedBillboardPipelineEntry
			{
				BatchedBillboardContainer materialMap;
				bool enabled = false;
			};

			using BillboardPipelineBatches = std::map<const MaterialPipeline*, BatchedBillboardPipelineEntry, MaterialPipelineComparator>;

			/// Sprites
			struct SpriteChain_XYZ_Color_UV
			{
				const VertexStruct_XYZ_Color_UV* vertices;
				std::size_t spriteCount;
			};

			struct BatchedSpriteEntry
			{
				NazaraSlot(Texture, OnTextureRelease, textureReleaseSlot);

				std::vector<SpriteChain_XYZ_Color_UV> spriteChains;
			};

			using SpriteOverlayBatches = std::map<const Texture*, BatchedSpriteEntry>;

			struct BatchedBasicSpriteEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				SpriteOverlayBatches overlayMap;
				bool enabled = false;
			};

			using SpriteMaterialBatches = std::map<const Material*, BatchedBasicSpriteEntry, MaterialComparator>;

			struct BatchedSpritePipelineEntry
			{
				SpriteMaterialBatches materialMap;
				bool enabled = false;
			};

			using SpritePipelineBatches = std::map<const MaterialPipeline*, BatchedSpritePipelineEntry, MaterialPipelineComparator>;

			/// Meshes
			struct MeshDataComparator
			{
				bool operator()(const MeshData& data1, const MeshData& data2) const;
			};

			struct MeshInstanceEntry
			{
				NazaraSlot(IndexBuffer, OnIndexBufferRelease, indexBufferReleaseSlot);
				NazaraSlot(VertexBuffer, OnVertexBufferRelease, vertexBufferReleaseSlot);

				std::vector<Matrix4f> instances;
				Spheref squaredBoundingSphere;
			};

			using MeshInstanceContainer = std::map<MeshData, MeshInstanceEntry, MeshDataComparator>;

			struct BatchedModelEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				MeshInstanceContainer meshMap;
				bool enabled = false;
			};

			using MeshMaterialBatches = std::map<const Material*, BatchedModelEntry, MaterialComparator>;

			struct BatchedMaterialEntry
			{
				std::size_t maxInstanceCount = 0;
				MeshMaterialBatches materialMap;
			};

			using MeshPipelineBatches = std::map<const MaterialPipeline*, BatchedMaterialEntry, MaterialPipelineComparator>;

			struct UnbatchedModelData
			{
				Matrix4f transformMatrix;
				MeshData meshData;
				Spheref obbSphere;
				const Material* material;
			};

			struct UnbatchedSpriteData
			{
				std::size_t spriteCount;
				const Material* material;
				const Texture* overlay;
				const VertexStruct_XYZ_Color_UV* vertices;
			};

			struct Layer
			{
				BillboardPipelineBatches billboards;
				SpritePipelineBatches opaqueSprites;
				MeshPipelineBatches opaqueModels;
				std::vector<std::size_t> depthSortedMeshes;
				std::vector<std::size_t> depthSortedSprites;
				std::vector<UnbatchedModelData> depthSortedMeshData;
				std::vector<UnbatchedSpriteData> depthSortedSpriteData;
				std::vector<const Drawable*> otherDrawables;
				unsigned int clearCount = 0;
			};

			std::map<int, Layer> layers;

			struct Billboard
			{
				int layerIndex;
				MovablePtr<const Nz::Material> material;
				Nz::Recti scissorRect;
				BillboardData data;
			};

			struct BillboardChain
			{
				int layerIndex;
				MovablePtr<const Nz::Material> material;
				Nz::Recti scissorRect;
				std::size_t billboardCount;
				std::size_t billboardIndex;
			};

			RenderQueue<BillboardChain> billboards;
			RenderQueue<Billboard> depthSortedBillboards;

			struct Model
			{
				int layerIndex;
				MeshData meshData;
				MovablePtr<const Nz::Material> material;
				Nz::Matrix4f matrix;
				Nz::Recti scissorRect;
				Nz::Spheref obbSphere;
			};

			RenderQueue<Model> models;
			RenderQueue<Model> depthSortedModels;

			struct SpriteChain
			{
				int layerIndex;
				std::size_t spriteCount;
				MovablePtr<const Material> material;
				MovablePtr<const Texture> overlay;
				MovablePtr<const VertexStruct_XYZ_Color_UV> vertices;
				Nz::Recti scissorRect;
			};

			RenderQueue<SpriteChain> basicSprites;
			RenderQueue<SpriteChain> depthSortedSprites;

			/*struct RenderData
			{
				Nz::IndexBuffer indexBuffer;
				Nz::MaterialPipelineRef pipeline;
				Nz::MaterialRef material;
				Nz::Matrix4f matrix;
				Nz::Rectf scissor;
				Nz::ShaderRef shader;
				Nz::TextureRef texture; //< multiples actually
				Nz::UInt8 layer;
				Nz::VertexBuffer vertexBuffer;
			};*/

		private:
			Layer& GetLayer(int i); ///TODO: Inline

			inline Color ComputeColor(float alpha);
			inline Vector2f ComputeSinCos(float angle);
			inline Vector2f ComputeSize(float size);

			inline void RegisterLayer(int layerIndex);

			void SortBillboards(Layer& layer, const Planef& nearPlane);
			void SortForOrthographic(const AbstractViewer* viewer);
			void SortForPerspective(const AbstractViewer* viewer);

			void OnIndexBufferInvalidation(const IndexBuffer* indexBuffer);
			void OnMaterialInvalidation(const Material* material);
			void OnTextureInvalidation(const Texture* texture);
			void OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer);

			std::vector<BillboardData> m_billboards;
			std::vector<int> m_renderLayers;
	};
}

#include <Nazara/Graphics/ForwardrenderQueue.inl>

#endif // NAZARA_FORWARDRENDERQUEUE_HPP
